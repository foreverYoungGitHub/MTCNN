#ifndef CELEBA_CROP_CELEBA_CROP_H
#define CELEBA_CROP_CELEBA_CROP_H

#endif //CELEBA_CROP_CELEBA_CROP_H

#include <string>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


class celeba_crop
{
public:



    vector<string> img_bbox_path_;
    vector<string> img_landmark_path_;
    vector<string> img_path_;
    vector<string> rects_path_;
    vector<string> landmark_path_;
    vector<vector<float>> landmark_;
    vector<vector<float>> rects_num_;
    vector<Rect> rects_;
    vector<vector<float>> regression_nums_;
    string img_txt_path_ = "/Users/Young/Documents/dataset/celeba/bbox_100.txt";
    string landmark_txt_path_ = "/Users/Young/Documents/dataset/celeba/landmarks_100.txt";
    string img_root_path_ = "/Users/Young/Documents/dataset/celeba/img/";
    string txt_root_path_ = "/Users/Young/Documents/dataset/celeba/";


    vector<string> txt_read(string file);
    void txt_write(vector<vector<float>> path, string name);
    void seperate_path_bbox(vector<string> string);
    void create_landmark_path(vector<string> string);
    vector<vector<float>> path_convert2_num(vector<string> path);
    void create_regression();
    void regression_rects();
    void regression_landmark();
    void crop_img();
    void run();
};