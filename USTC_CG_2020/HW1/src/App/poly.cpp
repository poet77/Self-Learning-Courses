#include "Poly.h"

Poly::Poly()
{
}

Poly::~Poly()
{
}


//用一个动态数组来储存多边形的顶点，如果图形选择多边形，鼠标点击则将顶点加入
void Poly::Draw(QPainter& painter)
{
    QPolygon polygon(points_.size());
    for(int i = 0; i < points_.size(); ++i)
    {
        polygon.setPoint(i, *points_[i]);
    }
    //设置画笔
    painter.setPen(QPen(color,width));
    painter.setBrush(brush);
    painter.drawPolygon(polygon);
}

void Poly::Push(QPoint *p)
{
    points_.push_back(p);
}

QPoint *Poly::top()
{
    return points_.back();
}

