//
// Created by Young on 2016/11/27.
//

#define CPU_ONLY

#ifndef MTCNN_MTCNN_H
#define MTCNN_MTCNN_H

#include <caffe/caffe.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace caffe;

class MTCNN {

public:

    MTCNN();
    MTCNN(const std::string &dir);
    MTCNN(const std::vector<std::string> model_file, const std::vector<std::string> trained_file);
    ~MTCNN();

    void detection(const cv::Mat& img, std::vector<cv::Rect>& rectangles);
    void detection_TEST(const cv::Mat& img, std::vector<cv::Rect>& rectangles);

    void Preprocess(const cv::Mat &img);
    void P_Net();
    void R_Net();
    void O_Net();
    void detect_net(int i);

    void local_NMS();
    void global_NMS();

    void Predict(const cv::Mat& img, int i);
    void Predict(const std::vector<cv::Mat> imgs, int i);
    void WrapInputLayer(const cv::Mat& img, std::vector<cv::Mat> *input_channels, int i);
    void WrapInputLayer(const vector<cv::Mat> imgs, std::vector<cv::Mat> *input_channels, int i);

    float IoU(cv::Rect rect1, cv::Rect rect2);
    float IoM(cv::Rect rect1, cv::Rect rect2);
    void resize_img();
    void GenerateBoxs(cv::Mat img);
    void BoxRegress(std::vector<cv::Rect>& bounding_box, std::vector<cv::Rect> regression_box);
    void Padding(std::vector<cv::Rect>& bounding_box, int img_w,int img_h);
    cv::Mat crop(cv::Mat img, cv::Rect& rect);

    void img_show(cv::Mat img, std::string name);
    void img_show_T(cv::Mat img, std::string name);
    //param for P, R, O, L net
    std::vector<std::shared_ptr<Net<float>>> nets_;
    std::vector<cv::Size> input_geometry_;
    int num_channels_;

    //variable for the image
    cv::Mat img_;
    std::vector<cv::Mat> img_resized_;
    std::vector<double> scale_;

    //variable for the output of the neural network
//    std::vector<cv::Rect> regression_box_;
    std::vector<float> regression_box_temp_;
    std::vector<cv::Rect> bounding_box_;
    std::vector<float> confidence_;
    std::vector<float> confidence_temp_;
    std::vector<std::vector<cv::Point>> alignment_;
    std::vector<float> alignment_temp_;

    //paramter for the threshold
    int minSize_ = 40;
    float factor_ = 0.709;
    float threshold_[3] = {0.6, 0.7, 0.7};
    float threshold_NMS_ = 0.5;
};


#endif //MTCNN_MTCNN_H
