#ifndef PGM_HPP
#define PGM_HPP

#include <fstream>
#include <string>
#include <iostream>
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

int dir_to_pixel[4][2] =
{
     1,  0,
     1,  1,
     0,  1,
    -1,  1
};

void clamp(int &val, int max_int)
{
    if(val < 0)
        val = 0;
    else if(val > max_int)
        val = max_int;
}

void read_pgm(std::string img_path, int **&img, int &width, int &height, int &max_int)
{
    std::ifstream input_img(img_path);
    std::string line;
    for(int i = 0; i < 2; i++)
        getline(input_img, line);
    input_img >> width >> height >> max_int;

    img = new int *[height];
    for(int i = 0; i < height; i++)
        img[i] = new int [width];

    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            input_img >> img[i][j];
    
    input_img.close();
}

void write_pgm(std::string img_path, int **img, int width, int height, int max_int)
{
    std::ofstream output_img(img_path);
    output_img << "P2\n" << "# Imagen generada por Alfonso Cardenas\n";
    output_img << width << " " << height << "\n" << max_int << std::endl;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            output_img << img[i][j] << std::endl;

    output_img.close();
}

void negate_img(int **img, int width, int height, int max_int)
{
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            img[i][j] = max_int - img[i][j];
}

void spread_img_intensities(int **img, int width, int height, int max_int)
{
    int old_min_int = max_int;
    int old_max_int = 0;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            if(img[i][j] < old_min_int) old_min_int = img[i][j];
            if(img[i][j] > old_max_int) old_max_int = img[i][j];
        }
    double slope = (double)max_int/(old_max_int - old_min_int);
    std::cout << "max: " << old_max_int << "\nmin: " << old_min_int << std::endl;
    std::cout << "slope: " << slope << std::endl;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            img[i][j] = slope*(img[i][j] - old_min_int);
}

void delete_img(int **img, int width, int height)
{
    for(int i = 0; i < height; i++)
        delete[] img[i];
    delete[] img;
}

void delete_img(double **img, int width, int height)
{
    for(int i = 0; i < height; i++)
        delete[] img[i];
    delete[] img;
}

double apply_filter_5x5(int **img, int h, int w, double kernel[5][5])
{
    double sum = 0;
    for(int i = -2; i <= 2; i++)
        for(int j = -2; j <= 2; j++)
            sum += img[h+i][w+j]*kernel[i+2][j+2];
    return sum;
}

double apply_filter_3x3(int **img, int h, int w, double kernel[3][3])
{
    double sum = 0;
    for(int i = -1; i <= 1; i++)
        for(int j = -1; j <= 1; j++)
            sum += img[h+i][w+j]*kernel[i+1][j+1];
    return sum;
}

int** apply_Gaussian_filter(double sigma, int **img, int width, int height)
{
    double r;
    double s = 2. * sigma * sigma;

    double G_kernel[5][5];

    double sum = 0;

    for(int x = -2; x <= 2; x++)
    {
        for(int y = -2; y <= 2; y++)
        {
            r = x*x + y*y;
            G_kernel[x+2][y+2] = (exp(-r/s))/(M_PI*s); 
            sum += G_kernel[x+2][y+2];
        }
    }

    for(int x = -2; x <= 2; x++)
        for(int y = -2; y <= 2; y++)
            G_kernel[x+2][y+2] /= sum;

    int** new_img = new int *[height];
    for(int i = 0; i < height; i++)
        new_img[i] = new int [width];
    
    for(int i = 2; i < height - 2; i++)
        for(int j = 2; j < width - 2; j++)
            new_img[i][j] = apply_filter_5x5(img, i, j, G_kernel);
    
    for(int i = 0; i < height; i++)
    {
        new_img[i][0] = 0;
        new_img[i][1] = 0;
        new_img[i][width-1] = 0;
        new_img[i][width-2] = 0;
    }
    
    for(int i = 0; i < width; i++)
    {
        new_img[0][i] = 0;
        new_img[1][i] = 0;
        new_img[height-1][i] = 0;
        new_img[height-2][i] = 0;
    }

    return new_img;
}

int map_angle(double theta)
{
    if(theta >= 22.5 && theta <= 67.5)
        return 1;
    
    if(theta >= 67.5 && theta <= 112.5)
        return 2;
    
    if(theta >= 112.5 && theta <= 157.5)
        return 3;
    
    return 0;
}

void erase_border(int **img, int width, int height, int thickness)
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < thickness; j++)
        {
            img[i][j] = 0;
            img[i][width - j - 1] = 0;
        }
    }

    for(int i = 0; i < thickness; i++)
    {
        for(int j = 0; j < width; j++)
        {
            img[i][j] = 0;
            img[height - i - 1][j] = 0;
        }
    }
}

void intensity_gradient(int **img, int width, int height, double **&G, int **&dir)
{
    double Gx_Kernel[3][3] = {
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    };
    double Gy_Kernel[3][3] = {
         1,  2,  1,
         0,  0,  0,
        -1, -2, -1
    };
    G = new double *[height];
    dir = new int *[height];
    double Gx, Gy;

    for(int i = 0; i < height; i++)
    {
        G[i] = new double [width];
        dir[i] = new int [width];
    }
    
    for(int i = 3; i < height-3; i++)
    {
        for(int j = 3; j < width-3; j++)
        {
            Gx = apply_filter_3x3(img, i, j, Gx_Kernel);
            Gy = apply_filter_3x3(img, i, j, Gy_Kernel);
            G[i][j] = sqrt(Gx*Gx + Gy*Gy);
            dir[i][j] = map_angle(atan2(Gy, Gx));
        }
    }
}

void lower_bound_cut_off(int **img, int width, int height, double **G, int **dir)
{
    int x, y;
    for(int i = 3; i < height-3; i++)
    {
        for(int j = 3; j < width-3; j++)
        {
            x = dir_to_pixel[dir[i][j]][0];
            y = dir_to_pixel[dir[i][j]][1];

            if(G[i][j] > G[i+y][j+x] && G[i][j] > G[i-y][j-x])
                img[i][j] = G[i][j];
            else
                img[i][j] = 0;
        }
    }
}

void double_threshold(int **img, int width, int height, int max_int)
{
    int first_threshold = .02 * max_int;
    int second_threshold = .05 * max_int;
    for(int i = 4; i < height-4; i++)
    {
        for(int j = 4; j < width-4; j++)
        {
            if(img[i][j] < first_threshold)
                img[i][j] = 0;
            else if(img[i][j] < second_threshold)
                img[i][j] = 128;
            else
                img[i][j] = 255;
        }
    }
}

bool search_strong(int **img, int h, int w)
{
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++)
            if(img[h+y][w+x] == 255) return true;

    return false;
}

void hysteresis(int **img, int width, int height)
{
    for(int i = 4; i < height-4; i++)
    {
        for(int j = 4; j < width-4; j++)
        {
            if(img[i][j] == 128)
            {
                if(search_strong(img, i, j))
                    img[i][j] = 255;
                else
                    img[i][j] = 0;
            }
        }
    }
}

int** Canny_edge_detector(int **img, int width, int height)
{
    int **new_img = apply_Gaussian_filter(2, img, width, height);
    
    double **G;
    int **dir;
    intensity_gradient(new_img, width, height, G, dir);
    lower_bound_cut_off(new_img, width, height, G, dir);
    erase_border(new_img, width, height, 4);

    int max_int = 0;
    for(int i = 4; i < height - 4; i++)
        for(int j = 4; j < width - 4; j++)
            if(new_img[i][j] > max_int) max_int = new_img[i][j];
    
    double_threshold(new_img, width, height, max_int);
    hysteresis(new_img, width, height);

    delete_img(G, width, height);
    delete_img(dir, width, height);

    return new_img;
}

#endif