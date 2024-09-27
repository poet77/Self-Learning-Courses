#pragma once
#include <vector>
#include <QPoint>
#include <Eigen/Dense>
#include <QImage>
#include "Warp.h"
using namespace std;
using namespace Eigen;

class RBF :public Warp
{
public:
    RBF();
    RBF(vector<QPoint>& p_points, vector<QPoint>& q_points, QPoint start, int u);
    ~RBF();

public:
    QPoint Get_output_point(QPoint p);


    void cal_distance_martix();
    void calculate_ri();
    void calculate_ai();

    QImage Get_image_deal_with_RBF(QImage img,bool fillhole);


private:
    double u_;
    int size_;
    double* d_;
    double* r_;
    VectorXd Ax_;
    VectorXd Ay_;
};

