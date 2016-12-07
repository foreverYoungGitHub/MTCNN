#include <iostream>
#include "compare.h"

int main() {

    vector<string> result= {
                    "/Users/Young/projects/MTCNN/result/FDDB_evaluation/FDDB-folds/fileList.txt",
                    "/Users/Young/projects/MTCNN/result/FDDB_evaluation/FDDB-folds/ellipseList.txt",
                    "/Users/Young/projects/MTCNN/result/FDDB_evaluation/FDDB-folds/MTCNN.txt"
            };

    Compare program;

    program.run(result);

    return 0;
}