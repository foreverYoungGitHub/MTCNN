//
// Created by xileli on 11/1/16.
//

#ifndef FACE_DETECTION_ELLIPSE_TRANSFORM_H
#define FACE_DETECTION_ELLIPSE_TRANSFORM_H

#include "opencv2/opencv.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
class ellipse_transform {
public:
    void img_path_read(string path);
    void ellipse_read(string path);
    void ellipse_trans(int index);
    void rectangle_show(int index);
    void correct_rect(Rect rect, Mat img);
    void cascade_rect_read(string path);
    float IoM(cv::Rect rect1, cv::Rect rect2);
    void img_crop_create(int index);
    void run(vector<string> path);
    void txt_write(string type, string img_path, vector<Rect> rectangles);
    void img_write(int index, string type, string img_path, vector<Rect> rectangles);
    bool create_folder(string dir_path);

    vector<vector<string>> ellipses_;
    vector<vector<Rect>> cascade_rects_;
    vector<string> img_path_;
    vector<Rect> cur_rect_;
    string root_path_ = "/home/xileli/Documents/dateset/FDDB/modify_crop/";
    string root_img_path_ = "/home/xileli/Documents/dateset/FDDB/";
    int file_num_ = 0;




};


#endif //FACE_DETECTION_ELLIPSE_TRANSFORM_H
