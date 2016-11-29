#include <iostream>
#include "MTCNN.h"

using namespace std;
using namespace cv;

int main() {

    vector<string> model_file = {
            "../model/det1.prototxt",
            "../model/det2.prototxt",
            "../model/det3.prototxt"
//            "../model/det4.prototxt"
    };

    vector<string> trained_file = {
            "../model/det1.caffemodel",
            "../model/det2.caffemodel",
            "../model/det3.caffemodel"
//            "../model/det4.caffemodel"
    };

    MTCNN mtcnn(model_file, trained_file);

    vector<Rect> rectangles;
    string img_path = "../result/trump.jpg";
    Mat img = imread(img_path);

    mtcnn.detection_TEST(img, rectangles);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

