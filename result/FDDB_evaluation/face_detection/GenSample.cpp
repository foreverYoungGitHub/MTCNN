//
// Created by xileli on 11/3/16.
//

#include "GenSample.h"


void GenSample::run(vector<string> path)
{
    read_in_param(path);

    for(int i = 0; i < img_path_.size(); i++)
    {
        read_in_cur_img(i);
    }

}

void GenSample::read_in_param(vector<string> path)
{
    img_path_read(path[0], img_path_);
    ellipse_read(path[1]);
    rectangle_read(path[2]);
    dataset_dir_ = path[3];
    write_dir_ = path[4];
}

void GenSample::img_path_read(string path, vector<string>& img_paths)
{
    img_paths.clear();
    std::ifstream img_path_file(path);
    while(!img_path_file.eof())
    {
        string line;
        getline(img_path_file, line);
        if (line == "")
            continue;
        img_paths.push_back(line);
    }
}

void GenSample::ellipse_read(string path)
{
    rect_info_read(path, img_path_, benchmark_);
}

void GenSample::rectangle_read(string path)
{
    rect_info_read(path, img_path_, testResult_);
}

void GenSample::rect_info_read(string path, vector<string> img_path, vector<vector<string>>& rect_info)
{
    int index = 0;
    std::ifstream img_path_file(path);// ???
    while(!img_path_file.eof())
    {
        string cur_img_path = img_path[index];
        string line;
        getline(img_path_file, line);
        if(cur_img_path == line)
        {
            string line_count;
            getline(img_path_file, line_count);

            //the num for the rect info
            int count = atoi(line_count.c_str());

            vector<string> cur_ellip;
            for(int i = 0; i < count; i ++)
            {
                string line_ellipse;
                getline(img_path_file, line_ellipse);
                cur_ellip.push_back(line_ellipse);
            }
            rect_info.push_back(cur_ellip);
        }

        index++;
    }
}

void GenSample::read_in_cur_img(int index)
{
    cur_img_path_ = img_path_[index];

    string img_path = dataset_dir_ + cur_img_path_ + ".jpg";
    img_read(img_path, img_);

    rect_read(benchmark_[index], cur_test_);
    ellipse_read(benchmark_[index], cur_bench_);
}

void GenSample::img_read(string img_path, Mat & img)
{
    img = imread(img_path);
}

void GenSample::rect_read(vector<string> info, vector<Rect> & rect)
{
    rect.clear();
    for (int i = 0; i < info.size(); i++)
    {
        char *cstr = new char[info[i].length() + 1];
        strcpy(cstr, info[i].c_str());
        char *p = strtok(cstr, " ");
        vector<float> cur_rect_list;
        while (p != 0) {
            float pos = atof(p);
            cur_rect_list.push_back(pos);
            p = strtok(NULL, " ");
        }
        Rect cur_rect = Rect(cur_rect_list[0], cur_rect_list[1], cur_rect_list[2], cur_rect_list[3]);
        rect.push_back(cur_rect);
    }
}

void GenSample::ellipse_read(vector<string> info, vector<Rect> & rects)
{
    rects.clear();
    for(int i = 0; i < info.size(); i++)
    {
        char *cstr = new char[info[i].length() + 1];
        strcpy(cstr, info[i].c_str());
        char *p = strtok(cstr, " ");
        vector<float> ellip;
        while(p != 0)
        {
            float pos = atof(p);
            ellip.push_back(pos);
            p = strtok(NULL, " ");
        }

        //transfer ellipse to rect
        Rect rect;
        float sin2 = sin(ellip[2] * PI / 180.0 ) * sin(ellip[2] * PI / 180.0 );
        float cos2 = cos(ellip[2] * PI / 180.0 ) * cos(ellip[2] * PI / 180.0 );
        float A2 = ellip[0] * ellip[0] ;
        float B2 = ellip[1] * ellip[1] ;

        rect.width = sqrt(1 / (sin2 / A2 + cos2 / B2)) * 2;
        rect.height = sqrt(1 / (cos2 / A2 + sin2 / B2)) * 2;

        rect.x = ellip[3] - rect.width / 2 ;
        rect.y = ellip[4] - rect.height / 2;
        rects.push_back(rect);
    }
}

void GenSample::sample()
{
    generate_positive();
    generate_negtive();
}

void GenSample::generate_positive()
{
    img_path_write();
    img_crop_write(img_, cur_bench_, cur_img_path_);
}

void GenSample::generate_negtive()
{
    vector<Rect> wrong_rect = cur_test_;

    for (int i = 0; i < wrong_rect.size(); i++) {
        float score = 0;
        for (int j = 0; j < cur_bench_.size(); j++) {
            score = max(score, IoM(wrong_rect[i], cur_bench_[j]));
        }
        if(score > threshold_)
        {
            wrong_rect.erase(wrong_rect.begin() + i);
            i--;
        }
    }

    img_path_write();
    img_crop_write(img_, wrong_rect, cur_img_path_);
}

float GenSample::IoM(cv::Rect rect1, cv::Rect rect2)
{
    int x_overlap, y_overlap, intersection, min_area;
    x_overlap = std::max(0, std::min((rect1.x + rect1.width), (rect2.x + rect2.width)) - std::max(rect1.x, rect2.x));
    y_overlap = std::max(0, std::min((rect1.y + rect1.height), (rect2.y + rect2.height)) - std::max(rect1.y, rect2.y));
    intersection = x_overlap * y_overlap;
    min_area = std::min((rect1.width * rect1.height), (rect2.width * rect2.height));
    return float(intersection)/min_area;
}

void GenSample::draw_rectangles(Mat img, vector<Rect> rectangles, Scalar color)
{
    for(int i = 0; i < rectangles.size(); i++)
    {
        correct_rect(rectangles[i], img);
        rectangle(img, rectangles[i], color);
    }
}

void GenSample::correct_rect(Rect rect, Mat img)
{
    rect.x = max(0, rect.x);
    rect.y = max(0, rect.y);
    rect.width = min(rect.width, img.cols);
    rect.height = min(rect.height, img.rows);
}

void GenSample::img_write(Mat img, vector<Rect> rectangles, string img_path)
{

}

void GenSample::img_path_write()
{

}

void GenSample::img_crop_write(Mat img, vector<Rect> rectangles, string img_path)
{

}