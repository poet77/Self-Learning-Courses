#include "Line.h"

Line::Line()
{
}

Line::~Line()
{
}
//利用Qt自带的画直线功能
void Line::Draw(QPainter& painter)
{
    //设置画笔
    painter.setPen(QPen(color,width));
    painter.setBrush(brush);
    painter.drawLine(start, end);
}
