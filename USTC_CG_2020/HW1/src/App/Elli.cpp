#include "Elli.h"

Elli::Elli()
{
}

Elli::~Elli()
{
}
//Qt自带的画椭圆功能
void Elli::Draw(QPainter& painter)
{
    //设置画笔
    painter.setPen(QPen(color, width));
    painter.setBrush(brush);
    painter.drawEllipse(start.x(), start.y(),
        end.x() - start.x(), end.y() - start.y());
}

