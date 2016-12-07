//
// Created by xileli on 10/31/16.
//

#ifndef FACE_DETECTION_FDDB_DETECTION_H
#define FACE_DETECTION_FDDB_DETECTION_H

#include <string>
#include <vector>
#include <fstream>
#include "../../../detection/MTCNN.h"

class fddb_detection {

public:
    fddb_detection();
    fddb_detection(std::string path);
    fddb_detection(std::string dataset_path, MTCNN * cascadeCNN);

    void run();

    bool file_list_read();
    bool file_list_read(string dataset_path);
    bool img_path_read(int i);
    bool img_path_read(string path);
    bool img_read(std::string path);
    bool generate_txt();
    bool txt_write(std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path);
    bool img_write(std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path);
    std::string img_path_convert(std::string path);

    MTCNN * cascadeCNN_;
    std::string dataset_path_;
    std::vector<std::string> file_list_;
    std::vector<std::string> img_path_;
    cv::Mat cur_img_;
    int i_;
    int txt_write_state_ = 1;
    int img_write_state_ = 1;

//    std::string mean_file = "/home/xileli/Documents/library/caffe/data/ilsvrc12/imagenet_mean.binaryproto";

    std::vector<std::string> model_file = {
            "../../../model/det1.prototxt",
            "../../../model/det2.prototxt",
            "../../../model/det3.prototxt"
    };

    std::vector<std::string> trained_file = {
            "../../../model/det1.caffemodel",
            "../../../model/det2.caffemodel",
            "../../../model/det3.caffemodel"
    };
};


#endif //FACE_DETECTION_FDDB_DETECTION_H
