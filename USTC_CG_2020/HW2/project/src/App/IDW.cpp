#include "IDW.h"
#include <iostream>

IDW::IDW()
{
    mu_ = 2.0;

}
IDW::~IDW()
{

}


// Computed weight function
QMap<int, double> IDW::Get_control_point_weight(QPoint input_point)
{
    QMap<int, double> point_correspond_control_point_weight_map;
    double sum_weight_ = 0;
    for (int i = 0; i < p_.size(); i++)
    {
        double temp_ = 0;
        temp_ = 1 /GET_DISTANCE(p_.at(i), input_point);
        temp_ = qPow(temp_, mu_);
        sum_weight_ = sum_weight_ + temp_;
    }
    for (int i = 0; i < p_.size(); i++)
    {
        double temp_ = 0;
        temp_ = 1 /GET_DISTANCE(p_.at(i), input_point);
        temp_ = qPow(temp_, mu_);
        temp_ = temp_ / sum_weight_;
        point_correspond_control_point_weight_map.insert(i, temp_);
    }
    return point_correspond_control_point_weight_map;
}

// Calculate output point
QPoint IDW::Get_output_point(QPoint input_point)
{
    double offset_x_ = 0;
    double offset_y_ = 0;
    double x = 0, y = 0;
    QMap<int, double> tem_map_ = Get_control_point_weight(input_point);
    for (int i = 0; i < p_.size(); i++)
    {
        offset_x_ = q_.at(i).x() - p_.at(i).x();
        offset_y_ = q_.at(i).y() - p_.at(i).y();
        x += offset_x_ * tem_map_.value(i);
        y += offset_y_ * tem_map_.value(i);
    }
    input_point.rx() = input_point.x() + x;
    input_point.ry() = input_point.y() + y;
    return input_point;
}

QImage IDW::Get_image_deal_with_IDW(QImage img,bool fillhole)
{
    QImage output_image = QImage(img.width(), img.height(), QImage::Format_ARGB32);
    QPoint output_point;
    // Determine if the point is filled
    int** filled;
    filled = new int* [img.height()];
    for (int j = 0; j < img.height(); j++)
        filled[j] = new int[img.width()];

    for (int i = 0; i < img.height(); i++)
        for (int j = 0; j < img.width(); j++)
            filled[i][j] = 0;

    for (int i = 0; i < img.width(); i++)
    {
        for (int j = 0; j < img.height(); j++)
        {
            output_point = this->Get_output_point(QPoint(i, j));
            if (output_point.x() > 0 && output_point.y() > 0 && output_point.x() < img.width() && output_point.y() < img.height())
            {
                filled[output_point.y()][output_point.x()] = 1;
                output_image.setPixelColor(output_point.x(), output_point.y(), img.pixelColor(QPoint(i, j)));
            }
        }
    }
    //  Fill it with the color of the surrounding points
    if (fillhole)
    {
        for (int i = 1; i < img.height() - 1; i++)
        {
            for (int j = 1; j < img.width() - 1; j++)
            {
                if (filled[i][j] == 0)
                {
                    /*
                    QColor temp11 = output_image.pixelColor(QPoint(j - 1, i - 1));
                    QColor temp12 = output_image.pixelColor(QPoint(j , i - 1));
                    QColor temp13 = output_image.pixelColor(QPoint(j + 1, i - 1));
                    QColor temp21 = output_image.pixelColor(QPoint(j , i - 1));
                    QColor temp22 = output_image.pixelColor(QPoint(j, i ));
                    QColor temp23 = output_image.pixelColor(QPoint(j, i + 1));
                    QColor temp31 = output_image.pixelColor(QPoint(j + 1, i - 1));
                    QColor temp32 = output_image.pixelColor(QPoint(j + 1, i));
                    QColor temp33 = output_image.pixelColor(QPoint(j + 1, i + 1));
                    std::vector<QColor> colors;
                    colors.push_back(temp11);
                    colors.push_back(temp12);
                    colors.push_back(temp13);
                    colors.push_back(temp21);
                    colors.push_back(temp22);
                    colors.push_back(temp23);
                    colors.push_back(temp31);
                    colors.push_back(temp32);
                    colors.push_back(temp33);
                    QColor blendedColor = blendColors(colors);
                    output_image.setPixelColor(j, i, blendedColor);
                    */

                    if (filled[i + 1][j] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j, i + 1)));
                    else if (filled[i - 1][j] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j, i - 1)));
                    else if (filled[i][j + 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j + 1, i)));
                    else if (filled[i][j - 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j - 1, i)));
                    else if (filled[i + 1][j + 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j + 1, i + 1)));
                    else if (filled[i - 1][j - 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j - 1, i - 1)));
                    else if (filled[i + 1][j - 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j - 1, i + 1)));
                    else if (filled[i - 1][j + 1] != 0)
                        output_image.setPixelColor(j, i, output_image.pixelColor(QPoint(j + 1, i - 1)));
                }
            }
        }
    }
    for (int j = 0; j < img.height(); j++)
    delete[] filled[j];
    delete[] filled;

    return output_image;
}
