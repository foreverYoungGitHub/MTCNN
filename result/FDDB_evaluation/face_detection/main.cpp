#include <iostream>
#include "fddb_detection.h"
//#include "ellipse_transform.h"
int main() {

    std::vector<std::string> model_file = {
            "../../../model/det1.prototxt",
            "../../../model/det2.prototxt",
            "../../../model/det3.prototxt"
    };

    std::vector<std::string> trained_file = {
            "../../../model/det1.caffemodel",
            "../../../model/det2.caffemodel",
            "../../../model/det3.caffemodel"
    };

    MTCNN * cascadeCNN;
    cascadeCNN = new MTCNN(model_file, trained_file);

    string dataset_path = "/Users/Young/projects/FDDB/";

    fddb_detection detection(dataset_path, cascadeCNN);

    detection.run();
}

//int main()
//{
//    vector<string> path = {
//            "/home/xileli/Documents/program/CascadeCNN/result/FDDB_evaluation/FDDB-folds/fileList.txt",
//            "/home/xileli/Documents/program/CascadeCNN/result/FDDB_evaluation/FDDB-folds/ellipseList.txt",
//            "/home/xileli/Documents/program/CascadeCNN/result/FDDB_evaluation/FDDB-folds/CascadeCNN.txt"
//    };
//    ellipse_transform ellipse;
//    ellipse.run(path);
//}