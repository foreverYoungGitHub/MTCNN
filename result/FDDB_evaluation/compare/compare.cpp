//
// Created by 刘阳 on 2016/12/6.
//

#include "compare.h"


bool Compare::img_path_read(string path)
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

bool Compare::ellipse_read(string path)
{
    int index = 0;
    std::ifstream img_path_file(path);// ???
    while(!img_path_file.eof())
    {
        string cur_img_path = img_path_[index];
        string line;
        getline(img_path_file, line);
        if(cur_img_path == line)
        {
            string line_count;
            getline(img_path_file, line_count);
            int count = atoi(line_count.c_str());

            vector<string> cur_ellip;
            for(int i = 0; i < count; i ++)
            {
                string line_ellipse;
                getline(img_path_file, line_ellipse);
                cur_ellip.push_back(line_ellipse);
            }
            ellipse_.push_back(cur_ellip);
        }

        index++;
    }
    ellipse_.pop_back();
    //vector<string> cur_ellip = ellipse_[2845];
    if(ellipse_.size() != 0)
        return true;
    else
        return false;
}

bool Compare::rect_read(string path)
{
    int index = 0;
    std::ifstream img_path_file(path);
    while(!img_path_file.eof())
    {
        string cur_img_path = img_path_[index];
        string line;
        getline(img_path_file, line);
        if (cur_img_path == line)
        {
            string line_count;
            getline(img_path_file, line_count);
            int count = atoi(line_count.c_str());

            vector<Rect> cascade_cur_rect;
            vector<float> confidence;
            for (int i = 0; i < count; i++)
            {
                string line_rect;
                getline(img_path_file, line_rect);
                char *cstr = new char[line_rect.length() + 1];
                strcpy(cstr, line_rect.c_str());
                char *p = strtok(cstr, " ");
                vector<float> cur_rect_list;
                while (p != 0) {
                    float pos = atof(p);
                    cur_rect_list.push_back(pos);
                    p = strtok(NULL, " ");
                }
                Rect cur_rect = Rect(cur_rect_list[0], cur_rect_list[1], cur_rect_list[2], cur_rect_list[3]);
                cascade_cur_rect.push_back(cur_rect);
                confidence.push_back(cur_rect_list[4]);
            }
            rect_.push_back(cascade_cur_rect);
            confidence_.push_back(confidence);
        }
        index++;
    }

    if(rect_.size() != 0)
        return true;
    else
        return false;
}

bool Compare::ellipse_trans(vector<std::string> ellipses)
{
    cur_ellipse_.clear();
    for(int i = 0; i < ellipses.size(); i++) {
        char *cstr = new char[ellipses[i].length() + 1];
        strcpy(cstr, ellipses[i].c_str());
        char *p = strtok(cstr, " ");
        vector<float> ellip;
        while (p != 0) {
            float pos = atof(p);
            ellip.push_back(pos);
            p = strtok(NULL, " ");
        }

        cur_ellipse_.push_back(ellip);
    }

    if(cur_ellipse_.size() != 0)
        return true;
    else
        return false;

}

bool Compare::img_write(std::vector<vector<float>> groundTruth,std::vector<cv::Rect> rects, std::vector<float> confidence, std::string img_path)
{
    string cur_img_path = root_img_path_ + img_path + ".jpg";
    cv::Mat img;
    img_read(cur_img_path);
    cur_img_.copyTo(img);

    //cv::imwrite("/home/xileli/Documents/program/CascadeCNN/" + name + "test.jpg", img);
    for(int i = 0; i < rects.size(); i++)
    {
        rectangle(img, rects[i], cv::Scalar(255, 0, 0));
        cv::putText(img, std::to_string(confidence[i]), cvPoint(rects[i].x + 3, rects[i].y + 13),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 0), 1, CV_AA);
    }

    for(int i = 0; i < groundTruth.size(); i++)
    {
        ellipse(img, cvPointFrom32f(cvPoint2D32f(groundTruth[i][3], groundTruth[i][4])), cvSize((int)groundTruth[i][1], (int)groundTruth[i][0]), 180-groundTruth[i][2], 0,360, cvScalar(0, 0, 255), 1);
    }

    cv::imwrite(root_img_path_ + "compare/" + img_path + "-compare.jpg", img);
    //imshow("test", img);
    //waitKey(0);
    return true;
}

bool Compare::img_read(string path)
{
    cur_img_ = cv::imread(path);
    if(cur_img_.rows)
        return true;
    else
        return false;
}

void Compare::run(vector<string> path)
{
    img_path_read(path[0]);
    ellipse_read(path[1]);
    rect_read(path[2]);

    for(int i = 0; i < img_path_.size(); i++)
    {
        cur_rect_ = rect_[i];
        ellipse_trans(ellipse_[i]); // ellipse convert to rectangles
        img_write(cur_ellipse_, cur_rect_, confidence_[i], img_path_[i]);
    }
}