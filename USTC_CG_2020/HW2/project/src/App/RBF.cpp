#include "RBF.h"
#include<iostream>
using namespace std;

RBF::RBF()
{
}

RBF::RBF(vector<QPoint>& p_, vector<QPoint>& q_, QPoint start, int u = -1)
{
    set_s(start);

    set_p(p_);
    set_q(q_);
    u_ = u;
    size_ = p_.size();
    d_ = new double[size_ * size_];
    r_ = new double[size_];

    cal_distance_martix();
    calculate_ri();
    calculate_ai();
}

RBF::~RBF()
{
    delete d_;
    delete r_;
}

//Computed distance matrix
void RBF::cal_distance_martix()
{
    for (size_t i = 0; i < size_; i++)
    {
        d_[i * size_ + i] = 0;
        for (size_t j = i + 1; j < size_; j++)
        {
            d_[i * size_ + j] = d_[j * size_ + i] = GET_DISTANCE(p_[i], p_[j]);
        }
    }
}

//Calculated correlation coefficient(min distance)
void RBF::calculate_ri()
{
    double min;

    for (size_t i = 0; i < size_; i++)
    {
        min = INT_MAX;
        for (size_t j = 0; j < size_; j++)
        {
            if (min > d_[i * size_ + j] && i != j)
                min = d_[i * size_ + j];
        }
        r_[i] = min;
    }
}

// solve equations by Eigen
void RBF::calculate_ai()
{
    Matrix<double, Dynamic, Dynamic> K(size_, size_);
    VectorXd Bx(size_);
    VectorXd By(size_);
    for (size_t i = 0; i < size_; i++)
    {
        Bx(i) = q_[i].x() - p_[i].x();
        By(i) = q_[i].y() - p_[i].y();
        for (size_t j = 0; j < size_; j++)
        {
            K(i, j) = pow(d_[j * size_ + i]* d_[j * size_ + i] + r_[j]* r_[j], u_/2);
        }
    }
    Ax_ = K.colPivHouseholderQr().solve(Bx);
    Ay_ = K.colPivHouseholderQr().solve(By);
}

QPoint RBF::Get_output_point(QPoint p)
{
    double x = p.x(), y = p.y(), t;
    for (int i = 0; i < size_; i++)
    {
        t = pow(GET_DISTANCE(p, p_[i])* GET_DISTANCE(p, p_[i]) + r_[i]* r_[i], u_/2);
        x += Ax_(i) * t;
        y += Ay_(i) * t;
    }
    return QPoint(x, y);
}


QImage RBF::Get_image_deal_with_RBF(QImage img,bool fillhole)
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
                   QColor temp11 = ouput_image_.pixelColor(QPoint(j - 1, i - 1));
                   QColor temp12 = ouput_image_.pixelColor(QPoint(j , i - 1));
                   QColor temp13 = ouput_image_.pixelColor(QPoint(j + 1, i - 1));
                   QColor temp21 = ouput_image_.pixelColor(QPoint(j , i - 1));

                   QColor temp23 = ouput_image_.pixelColor(QPoint(j, i + 1));
                   QColor temp31 = ouput_image_.pixelColor(QPoint(j + 1, i - 1));
                   QColor temp32 = ouput_image_.pixelColor(QPoint(j + 1, i));
                   QColor temp33 = ouput_image_.pixelColor(QPoint(j + 1, i + 1));

                   std::vector<QColor> colors;

                   colors.push_back(temp11);
                   colors.push_back(temp12);
                   colors.push_back(temp13);
                   colors.push_back(temp21);
                   colors.push_back(temp23);
                   colors.push_back(temp31);
                   colors.push_back(temp32);
                   colors.push_back(temp33);

                   QColor blendedColor = blendColors(colors);

                   ouput_image_.setPixelColor(j, i, blendedColor);
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