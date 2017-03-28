#include <iostream>
#include "celeba_crop.h"

int main() {
    //positive example
    celeba_crop test;
    test.run();

    //negtive example
    // std::fstream file("/Users/Young/Documents/dataset/negative_face/train_non_face/filename.txt");
    // std::ofstream file_out;
    // file_out.open("/Users/Young/Documents/dataset/negative_face/train_non_face/bounding_box.txt");

    // while (!file.eof()) {
    //     string line;
    //     getline(file, line);
    //     if (line == "")
    //         continue;
    //     file_out << "0 0 0 0" << std::endl;
    // }

    return 0;
}