//
// Created by xileli on 10/31/16.
//

#include "fddb_detection.h"

fddb_detection::fddb_detection() {}

fddb_detection::fddb_detection(std::string dataset_path)
{
    cascadeCNN_ = new MTCNN(model_file, trained_file);

    dataset_path_ = dataset_path;
}

fddb_detection::fddb_detection(std::string dataset_path, MTCNN * cascadeCNN)
{
    cascadeCNN_ = cascadeCNN;

    dataset_path_ = dataset_path;
}

void fddb_detection::run()
{
    if(!file_list_read())
    {
        std::cout << "Cannot find file_list.txt!" << std::endl;
        return;
    }


    for(int i = 0; i < file_list_.size(); i++)
    {
        i_ = i;

        std::cout << "Generating Fold: " << i_ + 1 << std::endl;


        if(!img_path_read(i))
        {
            std::cout << "Cannot find image path file!" << std::endl;
            return;
        }

        if(!generate_txt())
        {
            return;
        }
    }
}

bool fddb_detection::file_list_read()
{
    return file_list_read(dataset_path_ + "FDDB-folds/fileList.txt");
}

bool fddb_detection::file_list_read(string path)
{
    std::ifstream in(path);
    while(!in.eof())
    {
        string line;
        getline(in, line);
        if(line == "")
            continue;
        file_list_.push_back(line);
    }

    if(file_list_.size() != 0)
        return true;
    else
        return false;
}



bool fddb_detection::img_path_read(int i)
{
    return img_path_read(dataset_path_ + "FDDB-folds/fileList/" + file_list_[i]);
}

bool fddb_detection::img_path_read(string path)
{
    img_path_.clear();
    std::ifstream img_path_file(path);
    while(!img_path_file.eof())
    {
        string line;
        getline(img_path_file, line);
        if(line == "")
            continue;
        img_path_.push_back(line);
    }

    if(img_path_.size() != 0)
        return true;
    else
        return false;
}

bool fddb_detection::generate_txt()
{
    std::string cur_img_path;
//    int break_point = 100000;
    for(int j = 0; j < img_path_.size(); j++)
    {
//        if(img_path_[j] == "2002/08/03/big/img_462")
//        {
//            break_point = j;
//        }

//        if(j > break_point)
        {
            cur_img_path = dataset_path_ + img_path_[j] + ".jpg";
            if(!img_read(cur_img_path))
            {
                std::cout << "Cannot open " << cur_img_path << std::endl;
                continue;
            }

            std::vector<cv::Rect> cur_rect;
            std::vector<float> confidence;
            cascadeCNN_->detection(cur_img_, cur_rect, confidence);

            if(txt_write_state_ == 1)
            {
                txt_write(cur_rect, confidence, img_path_[j]);
            }

            if(img_write_state_ == 1)
            {
                img_write(cur_rect, confidence, img_path_[j]);
            }
            std::cout << "Successful Detect Image:" << cur_img_path << std::endl;
        }
    }

    return true;
}

bool fddb_detection::img_read(string path)
{
    cur_img_ = cv::imread(path);
    if(cur_img_.rows)
        return true;
    else
        return false;
}

bool fddb_detection::txt_write(std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path)
{
    string write_path = img_path_convert(dataset_path_ + "FDDB-folds/fileList/" + file_list_[i_]);
    std::fstream file(write_path + "-MTCNN.txt", ios::app);
    file << img_path << std::endl
         << rects.size() << std::endl;

    for(int k = 0; k < rects.size(); k++)
    {
        file << rects[k].x << " "
             << rects[k].y << " "
             << rects[k].width << " "
             << rects[k].height << " "
             << confidence[k] << std::endl;
    }

//    for(int k = 0; k < rects.size(); k++)
//    {
//        file << rects[k].x << " "
//             << rects[k].y << " "
//             << rects[k].width << " "
//             << rects[k].width << " "
//             << "1.0" << std::endl;
//    }

    return true;

}

bool fddb_detection::img_write(std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path)
{
    cv::Mat img;
    cur_img_.copyTo(img);
    //cv::imwrite("/home/xileli/Documents/program/CascadeCNN/" + name + "test.jpg", img);
    for(int i = 0; i < rects.size(); i++)
    {
        rectangle(img, rects[i], cv::Scalar(255, 0, 0));
        cv::putText(img, std::to_string(confidence[i]), cvPoint(rects[i].x + 3, rects[i].y + 13),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 0), 1, CV_AA);
    }

    cv::imwrite(dataset_path_ + "MTCNN/" + img_path + "-CascadeCNN.jpg", img);

    return true;
}

string fddb_detection::img_path_convert(string path)
{
    char * cstr = new char[path.length()+1];
    strcpy(cstr, path.c_str());
    char * p = strtok(cstr, ".");
    return p;
}