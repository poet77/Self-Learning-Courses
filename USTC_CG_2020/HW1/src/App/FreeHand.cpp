#include "FreeHand.h"

FreeHand::FreeHand(QPointF s)
{
    points.push_back(s);
}

FreeHand::~FreeHand()
{
}
//每次鼠标移动画一条很短的直线，连起来就是随手画
void FreeHand::Draw(QPainter &painter)
{
    painter.setPen(QPen(color,width));
    painter.setBrush(brush);
   // painter.drawLine(start,end);
    painter.drawPolyline(points);
}

