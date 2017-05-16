//
// Created by Young on 2016/11/27.
//

/*
 * TO DO : change the P-net and update the generat box
 */

#include "MTCNN.h"

MTCNN::MTCNN(){}

MTCNN::MTCNN(const std::vector<std::string> model_file, const std::vector<std::string> trained_file)
{
    #ifdef CPU_ONLY
        Caffe::set_mode(Caffe::CPU);
    #else
        Caffe::set_mode(Caffe::GPU);
    #endif

    for(int i = 0; i < model_file.size(); i++)
    {
        std::shared_ptr<Net<float>> net;

        cv::Size input_geometry;
        int num_channel;

        net.reset(new Net<float>(model_file[i], TEST));
        net->CopyTrainedLayersFrom(trained_file[i]);

        Blob<float>* input_layer = net->input_blobs()[0];
        num_channel = input_layer->channels();
        input_geometry = cv::Size(input_layer->width(), input_layer->height());

        nets_.push_back(net);
        input_geometry_.push_back(input_geometry);
        if(i == 0)
            num_channels_ = num_channel;
        else if(num_channels_ != num_channel)
            std::cout << "Error: The number channels of the nets are different!" << std::endl;
    }
}

MTCNN::~MTCNN(){}

void MTCNN::detection(const cv::Mat& img, std::vector<cv::Rect>& rectangles)
{
    Preprocess(img);
    P_Net();
    local_NMS();
    R_Net();
    local_NMS();
    O_Net();
    global_NMS();


    rectangles.clear();
    for(auto &bounding_box : bounding_box_)
    {
        rectangles.push_back(cv::Rect(bounding_box.y, bounding_box.x, bounding_box.height, bounding_box.width));
    }
}

void MTCNN::detection(const cv::Mat& img, std::vector<cv::Rect>& rectangles, std::vector<float>& confidence)
{
    detection(img, rectangles);

    confidence = confidence_;
}

void MTCNN::detection(const cv::Mat& img, std::vector<cv::Rect>& rectangles, std::vector<float>& confidence, std::vector<std::vector<cv::Point>>& alignment)
{
    detection(img, rectangles, confidence);

    alignment.clear();
    for(auto &i : alignment_)
    {
        std::vector<cv::Point> temp_alignment;
        for(auto &j : i)
        {
            temp_alignment.push_back(cv::Point(j.y, j.x));
        }
        alignment.push_back(std::move(temp_alignment));
    }

}

void MTCNN::detection_TEST(const cv::Mat& img, std::vector<cv::Rect>& rectangles)
{
    Preprocess(img);
    P_Net();
    img_show_T(img, "P-Net");
    local_NMS();
    img_show_T(img, "P-Net_nms");
    R_Net();
    img_show_T(img, "R-Net");
    local_NMS();
    img_show_T(img, "R-Net_nms");
    O_Net();
    img_show_T(img, "O-Net");
    global_NMS();
    img_show_T(img, "O-Net_nms");

}

void MTCNN::Preprocess(const cv::Mat &img)
{
    /* Convert the input image to the input image format of the network. */
    cv::Mat sample;
    if (img.channels() == 3 && num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4 && num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
    else if (img.channels() == 4 && num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
    else if (img.channels() == 1 && num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
    else
        sample = img;

    cv::Mat sample_float;
    if (num_channels_ == 3)
        sample.convertTo(sample_float, CV_32FC3);
    else
        sample.convertTo(sample_float, CV_32FC1);


    cv::cvtColor(sample_float,sample_float,cv::COLOR_BGR2RGB);
    sample_float = sample_float.t();

    img_ = sample_float;
}

void MTCNN::P_Net()
{
    resize_img();

    for(auto img_resized : img_resized_){
        Predict(img_resized, 0);
        GenerateBoxs(img_resized);
    }
}

void MTCNN::R_Net()
{
    detect_net(1);
}

void MTCNN::O_Net()
{
    detect_net(2);
}

void MTCNN::detect_net(int i)
{
    float thresh = threshold_[i];
    std::vector<cv::Rect> bounding_box;
    std::vector<float> confidence;
    std::vector<cv::Mat> cur_imgs;
    std::vector<std::vector<cv::Point>> alignment;

    if(bounding_box_.size() == 0)
        return;

    for (int j = 0; j < bounding_box_.size(); j++) {
        cv::Mat img = crop(img_, bounding_box_[j]);
        if (img.size() == cv::Size(0,0))
            continue;
        if (img.rows == 0 || img.cols == 0)
            continue;
        if (img.size() != input_geometry_[i])
            cv::resize(img, img, input_geometry_[i]);
        img.convertTo(img, CV_32FC3, 0.0078125,-127.5*0.0078125);
        cur_imgs.push_back(img);
    }

//    std::vector<cv::Mat> cur_imgs_test;
//    cur_imgs_test.push_back(cur_imgs[0]);

    Predict(cur_imgs, i);

    for(int j = 0; j < confidence_temp_.size()/2; j++)
    {
        float conf = confidence_temp_[2*j+1];
        if (conf > thresh) {

            if(conf>1)
                int a = 0;

            //bounding box
            cv::Rect bbox;

            //regression box : y x height width
            bbox.y = bounding_box_[j].y + regression_box_temp_[4*j] * bounding_box_[j].height;
            bbox.x = bounding_box_[j].x + regression_box_temp_[4*j+1] * bounding_box_[j].width ;
            bbox.height = bounding_box_[j].height + regression_box_temp_[4*j+2] * bounding_box_[j].height;
            bbox.width = bounding_box_[j].width + regression_box_temp_[4*j+3] * bounding_box_[j].width;

//            bbox.y = bounding_box_[j].y + regression_box_temp_[4*j] * bounding_box_[j].height - regression_box_temp_[4*j+2] * bounding_box_[j].height *  0.5;
//            bbox.x = bounding_box_[j].x + regression_box_temp_[4*j+1] * bounding_box_[j].width - regression_box_temp_[4*j+3] * bounding_box_[j].width * 0.5;
//            bbox.height = bounding_box_[j].height + regression_box_temp_[4*j+2] * bounding_box_[j].height;
//            bbox.width = bounding_box_[j].width + regression_box_temp_[4*j+3] * bounding_box_[j].width;

            if(i == 2)
            {
                //face alignment
                std::vector<cv::Point> align(5);
                for(int k = 0; k < 5; k++)
                {
//                    align[k].x = bbox.x + bbox.width * alignment_temp_[10*j+5+k] - 1;
//                    align[k].y = bbox.y + bbox.height * alignment_temp_[10*j+k] - 1;

                    align[k].x = bounding_box_[j].x + bounding_box_[j].width * alignment_temp_[10*j+5+k] - 1;
                    align[k].y = bounding_box_[j].y + bounding_box_[j].height * alignment_temp_[10*j+k] - 1;
                }
                alignment.push_back(align);
            }

            confidence.push_back(conf);
            bounding_box.push_back(bbox);

        }
    }

    cur_imgs.clear();

    bounding_box_ = bounding_box;
    confidence_ = confidence;
    alignment_ = alignment;
}


void MTCNN::local_NMS()
{
    std::vector<cv::Rect> cur_rects = bounding_box_;
    std::vector<float> confidence = confidence_;
    float threshold = threshold_NMS_;

    for(int i = 0; i < cur_rects.size(); i++)
    {
        for(int j = i + 1; j < cur_rects.size(); )
        {
            if(IoU(cur_rects[i], cur_rects[j]) > threshold)
            {
                float a = IoU(cur_rects[i], cur_rects[j]);
//                if(confidence[i] == confidence[j])
//                {
//                    cur_rects.erase(cur_rects.begin() + j);
//                    confidence.erase(confidence.begin() + j);
//                }
                if(confidence[i] >= confidence[j] && confidence[j] < 0.96)
                {
                    cur_rects.erase(cur_rects.begin() + j);
                    confidence.erase(confidence.begin() + j);
                }
                else if (confidence[i] < confidence[j] && confidence[i] < 0.96)
                {
                    cur_rects.erase(cur_rects.begin() + i);
                    confidence.erase(confidence.begin() + i);
                    i--;
                    break;
                }
                else
                {
                    j++;
                }
            }
            else
            {
                j++;
            }

        }
    }

    bounding_box_ = cur_rects;
    confidence_ = confidence;

}

void MTCNN::global_NMS()
{
    std::vector<cv::Rect> cur_rects = bounding_box_;
    std::vector<float> confidence = confidence_;
    std::vector<std::vector<cv::Point>> alignment = alignment_;
    float threshold_IoM = threshold_NMS_;
    float threshold_IoU = threshold_NMS_ - 0.1;


    for(int i = 0; i < cur_rects.size(); i++)
    {
        for(int j = i + 1; j < cur_rects.size(); )
        {
            if(IoU(cur_rects[i], cur_rects[j]) > threshold_IoU || IoM(cur_rects[i], cur_rects[j]) > threshold_IoM)
            {
                if(confidence[i] >= confidence[j])// && confidence[j] < 0.85) //if confidence[i] == confidence[j], it keeps the small one
                {
                    cur_rects.erase(cur_rects.begin() + j);
                    alignment.erase(alignment.begin() + j);
                    confidence.erase(confidence.begin() + j);
                }
                else if(confidence[i] < confidence[j])// && confidence[i] < 0.85)
                {
                    cur_rects.erase(cur_rects.begin() + i);
                    alignment.erase(alignment.begin() + i);
                    confidence.erase(confidence.begin() + i);
                    i--;
                    break;
                }
                else
                {
                    j++;
                }
            }
            else
            {
                j++;
            }
        }
    }

    bounding_box_ = cur_rects;
    confidence_ = confidence;
    alignment_ = alignment;
}


/*
 * Predict function input is a image without crop
 * the reshape of input layer is image's height and width
 */
void MTCNN::Predict(const cv::Mat& img, int i)
{
    std::shared_ptr<Net<float>> net = nets_[i];

    Blob<float>* input_layer = net->input_blobs()[0];
    input_layer->Reshape(1, num_channels_,
                         img.rows, img.cols);
    /* Forward dimension change to all layers. */
    net->Reshape();

    std::vector<cv::Mat> input_channels;
    WrapInputLayer(img, &input_channels, i);
    net->Forward();

    /* Copy the output layer to a std::vector */
    Blob<float>* rect = net->output_blobs()[0];
    Blob<float>* confidence = net->output_blobs()[1];
    int count = confidence->count() / 2;

    const float* rect_begin = rect->cpu_data();
    const float* rect_end = rect_begin + rect->channels() * count;
    regression_box_temp_ = std::vector<float>(rect_begin, rect_end);

    const float* confidence_begin = confidence->cpu_data() + count;
    const float* confidence_end = confidence_begin + count;

    confidence_temp_ = std::vector<float>(confidence_begin, confidence_end);
}

/*
 * Predict(const std::vector<cv::Mat> imgs, int i) function
 * used to input is a group of image with crop from original image
 * the reshape of input layer of net is the number, channels, height and width of images.
 */
void MTCNN::Predict(const std::vector<cv::Mat> imgs, int i)
{
    std::shared_ptr<Net<float>> net = nets_[i];

    Blob<float>* input_layer = net->input_blobs()[0];
    input_layer->Reshape(imgs.size(), num_channels_,
                         input_geometry_[i].height, input_geometry_[i].width);
    int num = input_layer->num();
    /* Forward dimension change to all layers. */
    net->Reshape();

    std::vector<cv::Mat> input_channels;
    WrapInputLayer(imgs, &input_channels, i);

    net->Forward();
    
    /* Copy the output layer to a std::vector */
    //You can also try to use the blob_by_name()

    //confidence
    Blob<float>* confidence = net->output_blobs()[i];
    int count = confidence->count() / 2; //the channel of confidence is two
    const float* confidence_begin = confidence->cpu_data();
    const float* confidence_end = confidence_begin + count * 2;
    confidence_temp_ = std::vector<float>(confidence_begin, confidence_end);

    //regression_box
    Blob<float>* rect = net->output_blobs()[0];
    const float* rect_begin = rect->cpu_data();
    const float* rect_end = rect_begin + rect->channels() * count;
    regression_box_temp_ = std::vector<float>(rect_begin, rect_end);

    //landmarks
    if( i == 2){
        Blob<float>* points = net->output_blobs()[1];
        const float* points_begin = points->cpu_data();
        const float* points_end = points_begin + points->channels() * count;
        alignment_temp_ = std::vector<float>(points_begin, points_end);
    }
}

void MTCNN::WrapInputLayer(const cv::Mat& img, std::vector<cv::Mat> *input_channels, int i)
{
    Blob<float>* input_layer = nets_[i]->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int j = 0; j < input_layer->channels(); ++j)
    {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }

    //cv::Mat sample_normalized;
    //cv::subtract(img, mean_[i], img);
    /* This operation will write the separate BGR planes directly to the
     * input layer of the network because it is wrapped by the cv::Mat
     * objects in input_channels. */
    cv::split(img, *input_channels);

}

/*
 * WrapInputLayer(const vector<cv::Mat> imgs, std::vector<cv::Mat> *input_channels, int i) function
 * used to write the separate BGR planes directly to the input layer of the network
 */
void MTCNN::WrapInputLayer(const vector<cv::Mat> imgs, std::vector<cv::Mat> *input_channels, int i)
{
    Blob<float> *input_layer = nets_[i]->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    int num = input_layer->num();
    float *input_data = input_layer->mutable_cpu_data();

    for (int j = 0; j < num; j++) {
        //std::vector<cv::Mat> *input_channels;
        for (int k = 0; k < input_layer->channels(); ++k) {
            cv::Mat channel(height, width, CV_32FC1, input_data);
            input_channels->push_back(channel);
            input_data += width * height;
        }
        cv::Mat img = imgs[j];
        cv::split(img, *input_channels);
        input_channels->clear();
    }
}

float MTCNN::IoU(cv::Rect rect1, cv::Rect rect2)
{
    int x_overlap, y_overlap, intersection, unions;
    x_overlap = std::max(0, std::min((rect1.x + rect1.width), (rect2.x + rect2.width)) - std::max(rect1.x, rect2.x));
    y_overlap = std::max(0, std::min((rect1.y + rect1.height), (rect2.y + rect2.height)) - std::max(rect1.y, rect2.y));
    intersection = x_overlap * y_overlap;
    unions = rect1.width * rect1.height + rect2.width * rect2.height - intersection;
    return float(intersection)/unions;
}

float MTCNN::IoM(cv::Rect rect1, cv::Rect rect2)
{
    int x_overlap, y_overlap, intersection, min_area;
    x_overlap = std::max(0, std::min((rect1.x + rect1.width), (rect2.x + rect2.width)) - std::max(rect1.x, rect2.x));
    y_overlap = std::max(0, std::min((rect1.y + rect1.height), (rect2.y + rect2.height)) - std::max(rect1.y, rect2.y));
    intersection = x_overlap * y_overlap;
    min_area = std::min((rect1.width * rect1.height), (rect2.width * rect2.height));
    return float(intersection)/min_area;
}

void MTCNN::resize_img()
{
    cv::Mat img = img_;
    int height = img.rows;
    int width = img.cols;

    int minSize = minSize_;
    float factor = factor_;
    double scale = 12./minSize;
    int minWH = std::min(height, width) * scale;

    std::vector<cv::Mat> img_resized;

    while(minWH >= 12)
    {
        int resized_h = std::ceil(height*scale);
        int resized_w = std::ceil(width*scale);

        cv::Mat resized;
        cv::resize(img, resized, cv::Size(resized_w, resized_h), 0, 0, cv::INTER_AREA);
        resized.convertTo(resized, CV_32FC3, 0.0078125,-127.5*0.0078125);
        img_resized.push_back(resized);

        minWH *= factor;
        scale *= factor;
    }

    img_resized_ = img_resized;
}

void MTCNN::GenerateBoxs(cv::Mat img)
{
    int stride = 2;
    int cellSize = input_geometry_[0].width;
    int image_h = img.rows;
    int image_w = img.cols;
    double scale = double(image_w) / img_.cols ;
    int feature_map_h = std::ceil((image_h - cellSize)*1.0/stride)+1;
    int feature_map_w = std::ceil((image_w - cellSize)*1.0/stride)+1;
    int width = (cellSize) / scale;
    int count = confidence_temp_.size();
    float thresh = threshold_[0];

    std::vector<cv::Rect> bounding_box;
    std::vector<cv::Rect> regression_box;
//    cv::Rect regression_box;
    std::vector<float> confidence;

    for(int i = 0; i < count; i++)
    {
        if(confidence_temp_[i] < thresh)
            continue;

        confidence.push_back(confidence_temp_[i]);

        int y = i / feature_map_w;
        int x = i - feature_map_w * y;

        //the regression box from the neural network
        //regression box : y x height width
        regression_box.push_back(cv::Rect(regression_box_temp_[i + count] * width, regression_box_temp_[i] * width,
                                          regression_box_temp_[i + count*3] * width, regression_box_temp_[i + count*2] * width));
//        regression_box = cv::Rect(regression_box_temp_[i] * width, regression_box_temp_[i + count] * width,
//                                          regression_box_temp_[i + count*2] * width, regression_box_temp_[i + count*3] * width));
        //the bounding box combined with regression box
        bounding_box.push_back(cv::Rect((x*stride+1)/scale, (y*stride+1)/scale,
                                        width, width));

        if((x*stride+1)/scale < -1000 || (x*stride+1)/scale > 1000)
            int a = 0;

//        bounding_box.push_back(cv::Rect((x*stride+1)/scale + regression_box.x, (y*stride+1)/scale + regression_box.y,
//                                        width + regression_box.width, width + regression_box.height));

    }

    confidence_.insert(confidence_.end(), confidence.begin(), confidence.end());
    BoxRegress(bounding_box, regression_box);
    bounding_box_.insert(bounding_box_.end(), bounding_box.begin(), bounding_box.end());
//    regression_box_.insert(regression_box_.end(), regression_box.begin(), regression_box.end());
}

void MTCNN::BoxRegress(std::vector<cv::Rect>& bounding_box, std::vector<cv::Rect> regression_box)
{

    for(int i=0;i<bounding_box.size();i++)
    {
        bounding_box[i].x += regression_box[i].x;
        bounding_box[i].y += regression_box[i].y;
        bounding_box[i].width += regression_box[i].width;
        bounding_box[i].height += regression_box[i].height;
//        float width = regression_box[i].width;
//        float height = regression_box[i].height;
//        float side = height>width ? height:width;
//        bounding_box[i].x -= side*0.5;
//        bounding_box[i].y -= width-side*0.5;
//        bounding_box[i].width += side*0.5;
//        bounding_box[i].height -= width-side*0.5;

    }
}

void MTCNN::Padding(std::vector<cv::Rect>& bounding_box, int img_w,int img_h)
{
    for(int i=0;i<bounding_box.size();i++)
    {
        bounding_box[i].x = (bounding_box[i].x > 0)? bounding_box[i].x : 0;
        bounding_box[i].y = (bounding_box[i].y > 0)? bounding_box[i].y : 0;
        bounding_box[i].width = (bounding_box[i].x + bounding_box[i].width < img_w) ? bounding_box[i].width : img_w;
        bounding_box[i].height = (bounding_box[i].y + bounding_box[i].height < img_h) ? bounding_box[i].height : img_h;
    }
}

cv::Mat MTCNN::crop(cv::Mat img, cv::Rect& rect)
{
    cv::Rect rect_old = rect;

//    if(rect.width > rect.height)
//    {
//        int change_to_square = rect.width - rect.height;
//        rect.height += change_to_square;
//        rect.y -= change_to_square * 0.5;
//    }
//    else
//    {
//        int change_to_square = rect.height - rect.width;
//        rect.width += change_to_square;
//        rect.x -= change_to_square * 0.5;
//    }

    cv::Rect padding;

    if(rect.x < 0)
    {
        padding.x = -rect.x;
        rect.x = 0;
    }
    if(rect.y < 0)
    {
        padding.y = -rect.y;
        rect.y = 0;
    }
    if(img.cols < (rect.x + rect.width))
    {
        padding.width = rect.x + rect.width - img.cols;
        rect.width = img.cols-rect.x;
    }
    if(img.rows < (rect.y + rect.height))
    {
        padding.height = rect.y + rect.height - img.rows;
        rect.height = img.rows - rect.y;
    }
    if(rect.width<0 || rect.height<0)
    {
        rect = cv::Rect(0,0,0,0);
        padding = cv::Rect(0,0,0,0);
    }
    cv::Mat img_cropped = img(rect);
    if(padding.x||padding.y||padding.width||padding.height)
    {
        cv::copyMakeBorder(img_cropped, img_cropped, padding.y, padding.height, padding.x, padding.width,cv::BORDER_CONSTANT,cv::Scalar(0));
        //here, the rect should be changed
        rect.x -= padding.x;
        rect.y -= padding.y;
        rect.width += padding.width + padding.x;
        rect.width += padding.height + padding.y;
    }

//    cv::imshow("crop", img_cropped);
//    cv::waitKey(0);

    return img_cropped;
}

void MTCNN::img_show(cv::Mat img, std::string name)
{
    cv::Mat img_show;
    img.copyTo(img_show);

    //cv::imwrite("../result/" + name + "test.jpg", img);

    for(int i = 0; i < bounding_box_.size(); i++)
    {
        rectangle(img_show, bounding_box_[i], cv::Scalar(0, 0, 255));
        cv::putText(img_show, std::to_string(confidence_[i]), cvPoint(bounding_box_[i].x + 3, bounding_box_[i].y + 13),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    }

    for(int i = 0; i < alignment_.size(); i++)
    {
        for(int j = 0; j < alignment_[i].size(); j++)
        {
            cv::circle(img_show, alignment_[i][j], 5, cv::Scalar(0, 255, 0));
        }
    }

    cv::imwrite("../result/" + name + ".jpg", img_show);
    //cv::waitKey(0);
}

void MTCNN::img_show_T(cv::Mat img, std::string name)
{
    cv::Mat img_show;
    img.copyTo(img_show);

    //cv::imwrite("../result/" + name + "test.jpg", img);

    for(int i = 0; i < bounding_box_.size(); i++)
    {
        rectangle(img_show, cv::Rect(bounding_box_[i].y, bounding_box_[i].x, bounding_box_[i].height, bounding_box_[i].width), cv::Scalar(0, 0, 255), 3);
        cv::putText(img_show, std::to_string(confidence_[i]), cvPoint(bounding_box_[i].y + 3, bounding_box_[i].x + 13),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    }

    for(int i = 0; i < alignment_.size(); i++)
    {
        for(int j = 0; j < alignment_[i].size(); j++)
        {
            cv::circle(img_show, cv::Point(alignment_[i][j].y, alignment_[i][j].x), 5, cv::Scalar(255, 255, 0), 3);
        }
    }

    cv::imwrite("../result/" + name + ".jpg", img_show);
    //cv::waitKey(0);
}
