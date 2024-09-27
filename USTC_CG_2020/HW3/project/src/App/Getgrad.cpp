#include "Getgrad.h"


CGetgrad::CGetgrad(void)
{
}


CGetgrad::~CGetgrad(void)
{
}

//Obtains the internal gradient of the original image

int CGetgrad::v(QPoint p, QPoint q, int rgb, QPoint pos)
{
	switch (rgb)
	{
	case 0:
        return qRed(source_imagewidget_->image().pixel(p)) - qRed(source_imagewidget_->image().pixel(q));
	case 1:
        return qGreen(source_imagewidget_->image().pixel(p)) - qGreen(source_imagewidget_->image().pixel(q));
	case 2:
        return qBlue(source_imagewidget_->image().pixel(p)) - qBlue(source_imagewidget_->image().pixel(q));
	default:
		break;
	}
	return 0;
}
