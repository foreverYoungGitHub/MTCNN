//
// Created by xileli on 11/1/16.
//

#include "ellipse_transform.h"
#define PI 3.14159265
void ellipse_transform::img_path_read(string path)
{
    img_path_.clear();
    std::ifstream img_path_file(path);
    while(!img_path_file.eof())
    {
        string line;
        getline(img_path_file, line);
        if (line == "")
            continue;
        img_path_.push_back(line);
    }
}

void ellipse_transform::ellipse_read(string path)
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
            ellipses_.push_back(cur_ellip);
        }

        index++;
    }
}

void ellipse_transform::cascade_rect_read(string path)
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
            }
            cascade_rects_.push_back(cascade_cur_rect);
        }
        index++;
    }
}


void ellipse_transform::ellipse_trans(int index)
{
    cur_rect_.clear();
    for(int i = 0; i < ellipses_[index].size(); i++)
    {
        char *cstr = new char[ellipses_[index][i].length() + 1];
        strcpy(cstr, ellipses_[index][i].c_str());
        char *p = strtok(cstr, " ");
        vector<float> ellip;
        while(p != 0)
        {
            float pos = atof(p);
            ellip.push_back(pos);
            p = strtok(NULL, " ");
        }
        Rect rect;
        float sin2 = sin(ellip[2] * PI / 180.0 ) * sin(ellip[2] * PI / 180.0 );
        float cos2 = cos(ellip[2] * PI / 180.0 ) * cos(ellip[2] * PI / 180.0 );
        float A2 = ellip[0] * ellip[0] ;
        float B2 = ellip[1] * ellip[1] ;
//
        rect.width = sqrt(1 / (sin2 / A2 + cos2 / B2)) * 2;
        rect.height = sqrt(1 / (cos2 / A2 + sin2 / B2)) * 2;

        //rect.width = max(ellip[3] * abs(sin(ellip[2] * PI / 180.0 )), abs(ellip[4] * cos(ellip[2] * PI / 180.0 )));
        //rect.height = max(ellip[3] * abs(cos(ellip[2] * PI / 180.0 )), abs(ellip[4] * sin(ellip[2] * PI / 180.0 )));
        rect.x = ellip[3] - rect.width / 2 ;
        rect.y = ellip[4] - rect.height / 2;
        //rect.width = ellip[4] * cos(ellip[2]);
        //rect.height = ellip[3] * cos(ellip[2]);
        cur_rect_.push_back(rect);
    }
    txt_write("groundtruth", img_path_[index], cur_rect_);
}


void ellipse_transform::rectangle_show(int index)
{
    Mat img = imread(root_img_path_ + img_path_[index] + ".jpg");
    for(int i = 0; i < cascade_rects_[index].size(); i++)
    {
        correct_rect(cascade_rects_[index][i], img);
        rectangle(img, cascade_rects_[index][i], Scalar(0, 0, 255));
    }
    for(int j = 0; j < cur_rect_.size(); j++)
    {
        correct_rect(cur_rect_[j], img);
        rectangle(img, cur_rect_[j], Scalar(0, 255, 0));
    }
    imshow("img", img);
    waitKey(0);
}

void ellipse_transform::correct_rect(Rect rect, Mat img)
{
    rect.x = max(0, rect.x);
    rect.y = max(0, rect.y);
    rect.width = min(rect.width, img.cols);
    rect.height = min(rect.height, img.rows);
}
float ellipse_transform::IoM(cv::Rect rect1, cv::Rect rect2)
{
    int x_overlap, y_overlap, intersection, min_area;
    x_overlap = std::max(0, std::min((rect1.x + rect1.width), (rect2.x + rect2.width)) - std::max(rect1.x, rect2.x));
    y_overlap = std::max(0, std::min((rect1.y + rect1.height), (rect2.y + rect2.height)) - std::max(rect1.y, rect2.y));
    intersection = x_overlap * y_overlap;
    min_area = std::min((rect1.width * rect1.height), (rect2.width * rect2.height));
    return float(intersection)/min_area;
}

void ellipse_transform::img_crop_create(int index) {

    vector<Rect> rect = cur_rect_, cas_rect = cascade_rects_[index];
    for (int i = 0; i < cur_rect_.size(); i++) {
        for (int j = 0; j < cascade_rects_[index].size(); j++) {
            if (IoM(cur_rect_[i], cascade_rects_[index][j]) >= 0.001) //threshold
            {
                cur_rect_.erase(cur_rect_.begin() + i);
                i--;
                cascade_rects_[index].erase(cascade_rects_[index].begin() + j);
                j--;
            }
        }
    }

    txt_write("positive", img_path_[index], cur_rect_);
    img_write(index, "positive", img_path_[index], cur_rect_);

    txt_write("negative", img_path_[index], cascade_rects_[index]);
    img_write(index, "negative", img_path_[index], cascade_rects_[index]);



}

void ellipse_transform::txt_write(string type, string img_path, vector<Rect> rectangles)
{

    fstream file(root_path_ + type + "/file_list.txt", ios::app);
    file << img_path << endl
                     <<rectangles.size() << endl;
    for(int i = 0; i < rectangles.size(); i++)
    {
        file << rectangles[i].x << " "
             << rectangles[i].y << " "
             << rectangles[i].width << " "
             << rectangles[i].height << " "
             //<< confidence[i] << " "
             << endl;
    }
}

/*
 * Is this function used to save the img for train?
 * If it is, it should not appear any rectangle
 * and the face img shoud be cropped!
 */
void ellipse_transform::img_write(int index, string type, string img_path, vector<Rect> rectangles)
{

    Mat img = imread(root_img_path_ + img_path_[index] + ".jpg");
    for(int i = 0; i < rectangles.size(); i++)
    {
        rectangle(img, rectangles[i], Scalar(255, 0, 0));
//        putText(img, to_string(confidence[i]), cvPoint(rectangles[i].x + 3, rectangles[i].y + 13),
//                cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 0), 1, CV_AA);
    }
    string img_aim_path = root_path_ + type + "/" + img_path_[index] + "/";
    if(create_folder(img_aim_path))
    imwrite(root_path_ + type + "/" + img_path_[index] + "-CascadeCNN.jpg", img); //notice!!
}



void ellipse_transform::run(vector<string> path)
{
    img_path_read(path[0]);
    ellipse_read(path[1]);
    cascade_rect_read(path[2]);

    for(int i = 0; i < img_path_.size(); i++)
    {
        ellipse_trans(i); // ellipse convert to rectangles
        rectangle_show(i); // converted rectangles and cascade rectangles show
        img_crop_create(i); // create txt and folder for both negative and positive cropped images
    }
}

bool ellipse_transform::create_folder(string dir_path)
{
    boost::filesystem::path dir(dir_path);
    if(boost::filesystem::is_directory(dir_path))
    {
        cout<<"Folder : "<< dir_path << " is existed" << endl;
        return true;
    }
    else
    {
        if(boost::filesystem::create_directory(dir))
        {
            cout<<"Success To Create Folder : "<< dir_path << endl;
            return true;
        }
        else
        {
            cout<<"Fail to Create Folder : "<< dir_path << endl;
            return false;
        }
    }
}

bool ellipse_transform::create_folder_test(string dir_path)
{
    /*
     * First of all this is a intuitive stupid way, I hope you can find some better way to do the same thing
     */

    /*
     * TO DO:
     * generate the paths to check does it exists
     * the path here should be the paths which we are read
     * which will looks like "2002/07/22/big/img_43"
     * here, we want to seperate it by "/" and put the new string to the vector<string> paths
     * so the example path will be seperate as "2002/", "2002/07/", "2002/07/22/",  "2002/07/22/big/", and "2002/07/22/big/img_43/"
     */

    vector<string> paths;



    /*
     * TO DO:
     * After we have the seperate paths, we need to check is it exist?
     * we need to create a loop to do that
     * if not exist, we save it into a new vector<string>
     * if exist, we quit the loop and the create the folder based on the new vector<string>
     *
     * Notice: Here, we need to add the root path with the vector string
     * Notice: if you want, use the element which is in the end of the vector and erase it at the same time,
     *         you could use a function called pop_back
     *         It will looks like this paths.pop_back()
     */
    boost::filesystem::path dir(dir_path);
    if(boost::filesystem::is_directory(dir_path))
    {
        cout<<"Folder : "<< dir_path << " is existed" << endl;
        return true;
    }


    /*
     * The final step is create all paths in the vector
     * This is simple, similar with the previous function
     * Notice:
     * But the most important thing is to notice where you should return true!
     * If you return it in a wrong position, the img may can not be created!
     */

    else
    {

    }
}


