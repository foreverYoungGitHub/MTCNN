#include "celeba_crop.h"

vector<string> celeba_crop::txt_read(string file) {

    vector<string> result;
    std::ifstream img_path_file(file);
    while (!img_path_file.eof()) {
        string line;
        getline(img_path_file, line);
        if (line == "")
            continue;
        result.push_back(line);
    }
    return result;
}


void celeba_crop::txt_write(vector<vector<float>> path, string name)
{
    ofstream file;
    file.open (txt_root_path_ + name + ".txt");
    for(int i = 0; i < path.size(); i++)
    {
        for(int j = 0; j < path[i].size(); j++)
        {
            file << path[i][j] << " ";
        }
        file << endl;
    }
    file.close();
}

void celeba_crop::seperate_path_bbox(vector<string> path)
{
    for(int i = 0; i < path.size(); i++) {
        char *cstr = new char[path[i].length() + 1];
        strcpy(cstr, path[i].c_str());
        char *token = std::strtok(cstr, " ");
        string a(token);
        img_path_.push_back(a);
        token = std::strtok(NULL, "\n");
        string b(token);
        rects_path_.push_back(b);
    }
}

void celeba_crop::create_landmark_path(vector<string> path)
{
    for(int i = 0; i < path.size(); i++) {
        char *cstr = new char[path[i].length() + 1];
        strcpy(cstr, path[i].c_str());
        char *token = std::strtok(cstr, " ");
        token = std::strtok(NULL, "\n");
        string b(token);
        landmark_path_.push_back(b);
    }
}

vector<vector<float>> celeba_crop::path_convert2_num(vector<string> path)
{
    vector<vector<float>> nums;
    for(int i = 0; i < path.size(); i++)
    {
        vector<float> num;
        char *cstr = new char[path[i].length() + 1];
        strcpy(cstr, path[i].c_str());
        char *token = std::strtok(cstr, " ");
        while(token != 0)
        {
            float p = atof(token);
            num.push_back(p);
            token = strtok(NULL, " ");
        }
        nums.push_back(num);
    }
    return nums;
}


void celeba_crop::create_regression()
{
    for(int i = 0; i < img_bbox_path_.size(); i++)
    {
        vector<float> v2;
        for(int j = 0; j < 4; j++)
        {
            float v1 = (rand() % 11 - 5) * 1.0 / 100;
            v2.push_back(v1);
        }
        regression_nums_.push_back(v2);
    }
}

void celeba_crop::regression_rects()
{
    for(int i = 0; i < img_bbox_path_.size(); i++)
    {
        Rect rect;
        rect.x = rects_num_[i][0] + rects_num_[i][2] * regression_nums_[i][0];
        rect.y = rects_num_[i][1] + rects_num_[i][3] * regression_nums_[i][1];
        rect.width = rects_num_[i][2] * (1 + regression_nums_[i][2]);
        rect.height = rects_num_[i][3] * (1 + regression_nums_[i][3]);
        rects_.push_back(rect);
    }
}

void celeba_crop::regression_landmark()
{
    for(int i = 0; i < landmark_.size(); i++)
    {
        for(int j = 0; j < landmark_[i].size(); j++)
        {
            if(j % 2 == 0)
                landmark_[i][j] = ( landmark_[i][j] - rects_[i].x ) / rects_[i].width;

            else
                landmark_[i][j] = ( landmark_[i][j] - rects_[i].y ) / rects_[i].height;
        }
    }
}

void celeba_crop::crop_img()
{
    ofstream file, file_label;
    file_label.open (txt_root_path_ + "label.txt");
    file.open (txt_root_path_ + "crop_image.txt");
    for(int i = 0; i < img_path_.size(); i++)
    {
        Mat img = imread(img_root_path_ + img_path_[i]);
        if(rects_[i].x < 0) rects_[i].x =0;
        if(rects_[i].y < 0) rects_[i].y =0;
        if(rects_[i].width + rects_[i].x > img.cols) rects_[i].width = img.cols - rects_[i].x ;
        if(rects_[i].height + rects_[i].y > img.rows) rects_[i].height = img.rows - rects_[i].y ;
        Mat img_crop = img(rects_[i]);
        imwrite(txt_root_path_ + "crop_image/" + img_path_[i], img_crop);
        file << txt_root_path_ + "crop_image/" + img_path_[i] << endl;
        file_label << "1" << endl;
    }
    file.close();
    file_label.close();
}

void celeba_crop::run()
{

    img_bbox_path_ = txt_read(img_txt_path_);
    seperate_path_bbox(img_bbox_path_);
    img_landmark_path_ = txt_read(landmark_txt_path_);
    create_landmark_path(img_landmark_path_);
    landmark_ = path_convert2_num(landmark_path_);
    rects_num_ = path_convert2_num(rects_path_);
    create_regression();
    regression_rects();
    regression_landmark();
    txt_write(landmark_, "landmark");
    txt_write(regression_nums_, "rectangle");
    crop_img();
}