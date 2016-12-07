//
// Created by 刘阳 on 2016/12/6.
//

#ifndef COMPARE_COMPARE_H
#define COMPARE_COMPARE_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Compare {

public:

    bool img_path_read(std::string path);
    bool ellipse_read(std::string path);
    bool rect_read(std::string path);
    bool ellipse_trans(vector<std::string> ellipses);
    bool img_read(string path);
    //void img_write();
    bool img_write(std::vector<vector<float>> groundTruth,std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path);

    void run(vector<string> path);

    vector<string> img_path_;
    vector<vector<string>> ellipse_;
    vector<vector<Rect>> rect_;
    vector<vector<float>> confidence_;

    cv::Mat cur_img_;
    vector<Rect> cur_rect_;
    vector<vector<float>> cur_ellipse_;

    string root_img_path_ = "/Users/Young/projects/FDDB/";


};


#endif //COMPARE_COMPARE_H
