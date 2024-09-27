#include "Getmixgrad.h"


CGetmixgrad::CGetmixgrad(void)
{
}


CGetmixgrad::~CGetmixgrad(void)
{
}

//Get the mixed gradient

int CGetmixgrad::v(QPoint p, QPoint q, int rgb, QPoint pos)
{
    QRgb gp = source_imagewidget_->image().pixel(p);
    QRgb gq = source_imagewidget_->image().pixel(q);

    QRgb fp = target_imagewidget_->image().pixel(p - source_rect_region_->topLeft() + pos);
    QRgb fq = target_imagewidget_->image().pixel(q - source_rect_region_->topLeft() + pos);

    switch (rgb)
    {
    case 0:
        if (abs(qRed(fp)-qRed(fq)) > abs(qRed(gp)-qRed(gq)))
            return qRed(fp)-qRed(fq);
        else
            return qRed(gp)-qRed(gq);
    case 1:
        if (abs(qGreen(fp)-qGreen(fq)) > abs(qGreen(gp)-qGreen(gq)))
            return qGreen(fp)-qGreen(fq);
        else
            return qGreen(gp)-qGreen(gq);
    case 2:
        if (abs(qBlue(fp)-qBlue(fq)) > abs(qBlue(gp)-qBlue(gq)))
            return qBlue(fp)-qBlue(fq);
        else
            return qBlue(gp)-qBlue(gq);
    default:
        break;
    }
    return  0;
}
