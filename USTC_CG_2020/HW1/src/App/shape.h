#pragma once

#include <QtGui>

class Shape
{
public:
	Shape();
	virtual ~Shape();
    virtual void Draw(QPainter &paint)=0;
   void set_start(QPoint s);
    void set_end(QPoint e);
	void Setshape(QPoint e, QPoint s);
    void set_line(QColor line_color,int line_width);
	void set_brush(QBrush brush);
    QPoint get_start();
    QPoint get_end();

public:
	enum Type
	{
		kDefault = 0,
		kLine=1,
		kRect=2,
        kEllipse=3,
        kPoly=4,
        kFreeHand=5,
	};
	QBrush	brush;
protected:
	QPolygonF points;
	QPoint start;
	QPoint end;
    QColor color;
	
	void Append(QPointF s);
    int width;
	
};

