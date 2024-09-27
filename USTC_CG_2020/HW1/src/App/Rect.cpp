#include "Rect.h"

Rect::Rect()
{
}

Rect::~Rect()
{
}

//Qt自带的画矩形功能
void Rect::Draw(QPainter& painter)
{
    //设置画笔
    painter.setPen(QPen(color,width));
    painter.setBrush(brush);
    painter.drawRect(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());

}

