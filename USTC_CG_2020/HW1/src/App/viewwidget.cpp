#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    draw_status_ = false;
    shape_ = NULL;
    type_ = Shape::kDefault;
    current_color = Qt::black;
    current_width = 1;
    stop_polygon = true;
    isundo = false;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::setLine()
{
    type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
    type_ = Shape::kRect;
}

void ViewWidget::setEllipse()
{
    type_ = Shape::kEllipse;
}

void ViewWidget::setPoly()
{
    type_ = Shape::kPoly;
}

void ViewWidget::setFreeHand()
{
    type_ = Shape::kFreeHand;
}
//鼠标点击，选择画不同的图形
void ViewWidget::mousePressEvent(QMouseEvent* event)
{
    isundo = false;
    start_point_ = end_point_ = event->pos();
    if (Qt::RightButton == event->button())
    {
        stop_polygon = true;
        return;
    }
    //右键代表多边形绘制完成
    if (Qt::LeftButton == event->button())
    {
        if (type_ == Shape::kPoly)
        {
            if (stop_polygon == true || shape_list_.empty())
            {
                shape_ = new Poly;
                Poly* tmp = dynamic_cast<Poly*>(shape_);
                QPoint* p = new QPoint(event->pos());
                tmp->Push(p);
                QPoint* p2 = new QPoint(event->pos() + QPoint(0, 1));
                tmp->Push(p2);
                shape_list_.push_back(shape_);
                shape_->set_line(current_color, current_width);
                stop_polygon = false;
            }
            else
            {
                if (type_ != Shape::kPoly)
                {
                    stop_polygon = true;
                    return;
                }
                QPoint* p = new QPoint(event->pos());
                dynamic_cast<Poly*>(shape_)->Push(p);
            }
            if (shape_ != NULL)
            {
                draw_status_ = true;
             
            //    shape_->Setshape(start_point_, end_point_);
              shape_->set_start(start_point_);
               shape_->set_end(end_point_);
            }
            update();
            return;
        }

        stop_polygon = true;
        //选择不同图形
        switch (type_)
        {
        case Shape::kLine:
            shape_ = new Line();
            break;
        case Shape::kDefault:
            break;
        case Shape::kRect:
            shape_ = new Rect();
            break;
        case Shape::kEllipse:
            shape_ = new Elli();
            break;
        case Shape::kFreeHand: {
            shape_ = new FreeHand(QPoint(start_point_));
            shape_list_.push_back(shape_);
            FreeHand* pfreehand = (FreeHand*)shape_;
            pfreehand->AppendPoint(start_point_);
       
            break; }
        default:
            break;
        }
        //确定起点
        if (shape_ != NULL)
        {
            draw_status_ = true;
           
         //   shape_->Setshape(start_point_, end_point_);
          shape_->set_start(start_point_);
         shape_->set_end(end_point_);
            shape_->set_line(current_color, current_width);
            shape_->set_brush(brush);

           

        }
        update();
    }

}
//鼠标移动
void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
    end_point_ = event->pos();
    if (draw_status_ && shape_ != NULL)
    {
        if (type_ == Shape::kPoly)
        {
            Poly* tmp = dynamic_cast<Poly*>(shape_list_.back());
            QPoint* p = tmp->top();
            p->setX(event->x());
            p->setY(event->y());
            update();
            return;
        }
     
   //     if (type_ != Shape::kFreeHand)
      shape_->set_end(end_point_);
      //  shape_->Setshape(start_point_, end_point_);
     
        if (type_ == Shape::kFreeHand)
        {
       //     shape_ = new Line();
       //   shape_->set_line(current_color, current_width);
      //    shape_list_.push_back(shape_);
      //    shape_->Setshape(end_point_, end_point_);
     //   shape_->set_start(end_point_);
     //    shape_->set_end(end_point_);
         FreeHand* pfreehand = (FreeHand*)shape_;
        pfreehand->AppendPoint(end_point_);
        }
        update();
    }
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    
    if (shape_ != NULL)
    {
        draw_status_ = false;
        if (type_ != Shape::kPoly)
        {
            shape_list_.push_back(shape_);
            shape_ = NULL;
        }

    }
}

//画出储存的图

void ViewWidget::paintEvent(QPaintEvent*)
{
    this->setAutoFillBackground(true);
    QPalette pal;
    if (!filename.isEmpty())
        pal.setBrush(QPalette::Window, QBrush(QPixmap(filename)));
    else
        pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    QPainter painter(this);

    for (int i = 0; i < shape_list_.size(); i++)
    {
        shape_list_[i]->Draw(painter);
    }

    if (shape_ != NULL && !isundo) {
        shape_->Draw(painter);
    }

    update();
}

//设置宽度

void ViewWidget::setColor(QColor c)
{
    current_color = c;
}

//设置画刷

/*
void ViewWidget::setWidth(int w)
{
    current_width = w % 20;
}
*/

/*
void ViewWidget::setbrushcolor(QColor bc)
{
    brush_color = bc;
}
*/

void ViewWidget::setPenWidth(int a_pen_width)
{
    current_width= a_pen_width %20 ;
}

void ViewWidget::setFilled(QBrush abrush)
{
    brush = abrush;
    if (shape_ != nullptr)
    {
        shape_->brush = brush;
        brush.setColor(brush_color);
        update();
    }
}

//设置背景

void ViewWidget::setBackground()
{
    filename = QFileDialog::getOpenFileName(this, "打开文件", QDir::currentPath());
}

//实现撤销操作，只需将shape_list队尾元素删除即可

void ViewWidget::Undo()
{
    stop_polygon = true;
    if (shape_ != nullptr)
    {
        delete shape_;
        draw_status_ = false;
    }
    else {
        if (!shape_list_.empty())
        {
            isundo = true;
            redo_shape_list.push_back(shape_list_.back());
            shape_list_.pop_back();
            shape_list_.shrink_to_fit(); //防止内存泄露

        }
    }
    repaint();
}

//实现返回操作，只需将redo_shape_list中点弹出即可

void ViewWidget::Redo()
{
    stop_polygon = true;
    
    if (!redo_shape_list.empty())
    {
        shape_list_.push_back(redo_shape_list.back());
        redo_shape_list.pop_back();
        redo_shape_list.shrink_to_fit(); //防止内存泄露
       
    }
    update();
}

//直接调用Clear函数实现清屏功能

void ViewWidget::Clear()
{
    stop_polygon = true;
    shape_ = NULL;
    current_color = Qt::black;
    current_width = 1;
    brush_color = Qt::black;
    shape_list_.clear();
    shape_list_.shrink_to_fit(); //防止内存泄露
    redo_shape_list.clear();
    redo_shape_list.shrink_to_fit();
    filename = "";
    update();
}
