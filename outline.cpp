#include <iostream>
#include "pgm.hpp"

using namespace std;

string line;

int main()
{
    int **img;
    int width, height, max_int;
    read_pgm("Images/130.pgm", img, width, height, max_int);

    int **new_img = Canny_edge_detector(img, width, height);

    write_pgm("output_img.pgm", new_img, width, height, max_int);
    delete_img(img, width, height);
    delete_img(new_img, width, height);
}