#ifndef MINIDRAW_H
#define MINIDRAW_H

#include <ui_minidraw.h>
#include <viewwidget.h>

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h>
#include <QtWidgets/QSpinBox>
#include <QLabel>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QDial>
#include <QtWidgets/QLabel>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QCheckBox>
#include <QSlider>

class MiniDraw : public QMainWindow
{
	Q_OBJECT

public:
	MiniDraw(QWidget *parent = 0);
	~MiniDraw();

    QMenu *pMenu,*p2Menu,*p3Menu;
    QToolBar *pToolBar,*p2ToolBar;
	QAction *Action_About;
	QAction *Action_Line;
	QAction *Action_Rect;
    QAction *Action_Ellipse;
    QAction *Action_Poly;
    QAction *Action_FreeHand;
    QAction *Action_Color;
    QAction* Action_brushColor;
    QAction *Action_Width;
    QAction *Action_Open;
    QAction *Action_Undo;
    QAction *Action_Redo;
    QAction *Action_Clear;
    QLabel* pLabel_Width;
    QCheckBox* pCheckBox_Filled;



	void Creat_Menu();
	void Creat_ToolBar();
    void Creat_Action();
	void AboutBox();
    void color_slot();
    void brushcolor_slot();
    void SetPenWidth();
  //  void width_slot();
    void CreatePenSettings();
    void onCheckBox_FilledClicked();
    void Setbrushcolor(QColor c);
   // void Setcolor();

signals:
    void change_color(QColor);
    void change_brushcolor(QColor);
    void change_width(int);


private:
	Ui::MiniDrawClass ui;
	ViewWidget*	view_widget_;



private:
    QLabel *width;
    QSlider* pen_width_slider_;
    QColor brushcolor;
};

#endif // MINIDRAW_H
