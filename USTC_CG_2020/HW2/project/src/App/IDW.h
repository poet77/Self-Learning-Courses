#pragma once
#ifndef IDW_H
#define IDW_H

#include <QPoint>
#include <vector>

#include <QDebug>
#include <QImage>
#include "Warp.h"

using namespace std;

class IDW :public Warp
{
public:
    IDW();
    ~IDW();
public:
    QMap<int, double> Get_control_point_weight(QPoint input_point);
    QPoint Get_output_point(QPoint input_point);
    QImage Get_image_deal_with_IDW(QImage img,bool fillhole);

private:
    double mu_;
};
#endif
