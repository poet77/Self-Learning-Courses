#include "Warp.h"

Warp::Warp()
{

}

Warp::~Warp()
{

}

void Warp::set_s(QPoint s)
{
    this->start_point = s;
}

void Warp::set_q(vector<QPoint> q_)
{
    for (int i = 0; i < q_.size(); i++)
    {
        QPoint temp;
        temp = q_[i];
        this->q_.push_back(QPoint(temp.x() - start_point.x(), temp.y() - start_point.y()));
    }

}

void Warp::set_p(vector<QPoint> p_)
{
    for (int i = 0; i < p_.size(); i++)
    {
        QPoint temp;
        temp = p_[i];
        this->p_.push_back(QPoint(temp.x() - start_point.x(), temp.y() - start_point.y()));
    }

}
/*
QColor Warp::blendColors(const std::vector<QColor>& colors) {
    int redSum = 0;
    int greenSum = 0;
    int blueSum = 0;

    for (const QColor& color : colors) {
        redSum += color.red();
        greenSum += color.green();
        blueSum += color.blue();
    }

    int size = colors.size();
    int redAverage = redSum / size;
    int greenAverage = greenSum / size;
    int blueAverage = blueSum / size;

    return QColor(redAverage, greenAverage, blueAverage);
}
*/