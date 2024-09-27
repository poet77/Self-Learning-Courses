#include "minidraw.h"
#include <QColorDialog>
#include <QInputDialog>

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    QActionGroup* group = new QActionGroup(this);
    setStyleSheet("padding:10px 4px;");
	view_widget_ = new ViewWidget();
    setCentralWidget(view_widget_);
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();

}

void MiniDraw::CreatePenSettings()
{
    pLabel_Width = new QLabel();
    pLabel_Width->setText(" Width ");
    pToolBar->addWidget(pLabel_Width);

    pen_width_slider_ = new QSlider();
    //pen_width_slider_->setGeometry(QRect(800, 5, 100, 15));
    pen_width_slider_->setOrientation(Qt::Horizontal);
    pen_width_slider_->setRange(1, 50);
    pToolBar->addWidget(pen_width_slider_);
    pen_width_slider_->setFixedSize(100, 15);
    connect(pen_width_slider_, &QSlider::valueChanged, this, &MiniDraw::SetPenWidth);
    pCheckBox_Filled = new QCheckBox();
    pCheckBox_Filled->setText("Filled");
    connect(pCheckBox_Filled, &QCheckBox::clicked, this, &MiniDraw::onCheckBox_FilledClicked);
    pToolBar->addWidget(pCheckBox_Filled);
    
}

void MiniDraw::SetPenWidth()
{
    view_widget_->setPenWidth(pen_width_slider_->value());
}

void MiniDraw::Setbrushcolor(QColor c)
{
    brushcolor = c;
    if (pCheckBox_Filled->isChecked())
    {
        view_widget_->setFilled(QBrush(QColor(brushcolor), Qt::SolidPattern));
    }
}

//相关动作

void MiniDraw::Creat_Action()
{
    Action_About = new QAction(QIcon("../resources/about.png"),tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);

    Action_Line = new QAction(QIcon("../resources/Line.png"),tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

    Action_Rect = new QAction(QIcon("../resources/Rect.png"),tr("&Rect"), this);
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

    Action_Ellipse = new QAction(QIcon("../resources/Elli.png"),tr("&Ellipse"), this);
    connect(Action_Ellipse, &QAction::triggered, view_widget_, &ViewWidget::setEllipse);

    Action_Poly = new QAction(QIcon("../resources/polygon.png"),tr("&Polygon"), this);
    connect(Action_Poly, &QAction::triggered, view_widget_, &ViewWidget::setPoly);

    Action_FreeHand = new QAction(QIcon("../resources/Freehand.png"),tr("&FreeHand"), this);
    connect(Action_FreeHand, &QAction::triggered, view_widget_, &ViewWidget::setFreeHand);

    Action_Color = new QAction(QIcon("../resources/color.png"),tr("&Color"), this);
    connect(Action_Color, &QAction::triggered,this,&MiniDraw::color_slot);
    connect(this,&MiniDraw::change_color,view_widget_,&ViewWidget::setColor); //将改变颜色的信号与槽函数相连

    Action_brushColor = new QAction(QIcon("../resources/brushcolor.png"), tr("&brushColor"), this);
    connect(Action_brushColor, &QAction::triggered, this, &MiniDraw::brushcolor_slot);
    connect(this, &MiniDraw::change_brushcolor, this, &MiniDraw::Setbrushcolor); //将改变颜色的信号与槽函数相连
   
 /*
    Action_Width = new QAction(QIcon("../resources/Width.png"),tr("&Width"),this);
    connect(Action_Width, &QAction::triggered,this,&MiniDraw::width_slot);
    connect(this,&MiniDraw::change_width,view_widget_,&ViewWidget::setWidth);//将改变宽度的信号与槽函数相连
*/
  
 



    Action_Open = new QAction(QIcon("../resources/open.png"),tr("&Insert Picture"),this);
    connect(Action_Open, &QAction::triggered, view_widget_, &ViewWidget::setBackground);

    Action_Undo = new QAction(QIcon("../resources/Undo.png"),tr("&Undo"),this);
    connect(Action_Undo ,&QAction::triggered, view_widget_, &ViewWidget::Undo);

    Action_Redo = new QAction(QIcon("../resources/Redo.png"),tr("&Redo"),this);
    connect(Action_Redo, &QAction::triggered, view_widget_, &ViewWidget::Redo);

    Action_Clear= new QAction(QIcon("../resources/Clear.png"),tr("&Clear"),this);
    connect(Action_Clear, &QAction::triggered, view_widget_, &ViewWidget::Clear);


 
}

void MiniDraw::onCheckBox_FilledClicked()
{
    if (pCheckBox_Filled->isChecked())
    {
        view_widget_->setFilled(QBrush(QColor(brushcolor), Qt::SolidPattern));
    }
    else
    {
        view_widget_->setFilled();
    }
}

//创建工具栏

void MiniDraw::Creat_ToolBar()
{
    pToolBar = addToolBar(tr("&Draw"));
    pToolBar->addAction(Action_Line);
    pToolBar->addAction(Action_Rect);
    pToolBar->addAction(Action_Ellipse);
    pToolBar->addAction(Action_Poly);
    pToolBar->addAction(Action_FreeHand);
    pToolBar->addAction(Action_Color);
    pToolBar->addAction(Action_brushColor);

 //   pToolBar->addAction(Action_Width);

    p2ToolBar=addToolBar(tr("&Tool"));
    p2ToolBar->addAction(Action_Open);
    p2ToolBar->addAction(Action_Undo);
    p2ToolBar->addAction(Action_Redo);
    p2ToolBar->addAction(Action_Clear);
    p2ToolBar->addAction(Action_About);

    CreatePenSettings();
}

//创建菜单

void MiniDraw::Creat_Menu()
{
    p2Menu=menuBar()->addMenu(tr("&File"));
    p2Menu->addAction(Action_Open);
    p2Menu->addAction(Action_About);

    pMenu = menuBar()->addMenu(tr("&Figure"));
    pMenu->addAction(Action_Line);
    pMenu->addAction(Action_Rect);
    pMenu->addAction(Action_Ellipse);
    pMenu->addAction(Action_Poly);
    pMenu->addAction(Action_FreeHand);

    p3Menu=menuBar()->addMenu(tr("&Edit"));
    p3Menu->addAction(Action_Undo);
    p3Menu->addAction(Action_Redo);
    p3Menu->addAction(Action_Clear);

   
}

//设置颜色，使用Qt自带的库

void MiniDraw::color_slot()
{
    QColor c=QColorDialog::getColor(Qt::black);
    emit change_color(c);
}

void MiniDraw::brushcolor_slot()
{
    QColor c = QColorDialog::getColor(Qt::black);
    emit change_brushcolor(c);
}


//设置宽度，使用Qt自带的库

/*
void MiniDraw::width_slot()
{
    int w= QInputDialog::getInt(this, QString("Set Width"), QString("Input width"), true, 1, 20, 1);
    emit change_width(w);
}
*/


//关于Minidraw的介绍

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw,a simple drawing tool,created by 77"));
}



MiniDraw::~MiniDraw()
{

}
