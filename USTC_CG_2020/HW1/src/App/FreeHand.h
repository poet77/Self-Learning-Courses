#pragma once

#include "shape.h"

class FreeHand:public Shape
{
public:
    FreeHand(QPointF s);
    ~FreeHand();
    void AppendPoint(QPointF s) { points << s; };

    void Draw(QPainter& painter);
};
