#ifndef WARP_H
#define WARP_H
#include <vector>
#include <QtMath>
#include <QPoint>
#include <QColor>
#define GET_DISTANCE(p1, p2) qSqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()))
using namespace std;

class Warp
{
public:
    Warp();
    virtual ~Warp();

    void set_s(QPoint s);

    void set_p(vector<QPoint> p_);
    void set_q(vector<QPoint> q_);

 //   QColor blendColors(const std::vector<QColor>& colors);

protected:
    vector<QPoint> p_;
    vector<QPoint> q_;
    QPoint start_point;
};

#endif // WARP_H
