//
// Created by xileli on 11/3/16.
//

#ifndef FACE_DETECTION_GENSAMPLE_H
#define FACE_DETECTION_GENSAMPLE_H

#include "opencv2/opencv.hpp"
#define PI 3.14159265

using namespace std;
using namespace cv;

class GenSample {



    void run(vector<string> path);

    void read_in_param(vector<string> path);
    void img_path_read(string path, vector<string>& img_paths);
    void rect_info_read(string path, vector<string> img_path, vector<vector<string>>& rect_info);
    void ellipse_read(string path);
    void rectangle_read(string path);

    void read_in_cur_img(int index);
    void img_read(string img_path, Mat & img);
    void rect_read(vector<string> info, vector<Rect> & rect);
    void ellipse_read(vector<string> info, vector<Rect> & rects);

    void sample();
    void generate_positive();
    void generate_negtive();
    float IoM(cv::Rect rect1, cv::Rect rect2);
    void img_path_write();
    void img_info_write(string img_path, vector<Rect> rectangles);
    void img_crop_write(Mat img, vector<Rect> rectangles, string img_path);
    void img_write(Mat img, vector<Rect> rectangles, string img_path);
    void img_show(Mat img, string name);
    void draw_rectangles(Mat img, vector<Rect> rectangles, Scalar color);
    void correct_rect(Rect rect, Mat img);

    vector<vector<string>> benchmark_, testResult_;
    vector<string> img_path_;
    string dataset_dir_, write_dir_;
    vector<Rect> cur_bench_, cur_test_;
    string cur_img_path_;
    Mat img_;
    float threshold_;

};


#endif //FACE_DETECTION_GENSAMPLE_H
