#include "shape.h"

Shape::Shape()
{
    start=QPoint(0,0);
    end=QPoint(0,0);
    color=Qt::black;
    width=1;
}

Shape::~Shape()
{
}

void Shape::Append(QPointF s)
{
    points << s;
}

//设定起点
void Shape::set_start(QPoint s)
{
	start = s;
}

//设定终点
void Shape::set_end(QPoint e)
{
	end = e;
}


void Shape::Setshape(QPoint s, QPoint e)
{
    start = s;
    end = e;
}


//设定颜色与宽度
void Shape::set_line(QColor line_color, int line_width)
{
    color=line_color;
    width=line_width;
}

void Shape::set_brush(QBrush shape_brush)
{
    brush = shape_brush;
}


//获取起点
QPoint Shape::get_start()
{
    return start;
}
//获取终点
QPoint Shape::get_end()
{
    return end;
}



