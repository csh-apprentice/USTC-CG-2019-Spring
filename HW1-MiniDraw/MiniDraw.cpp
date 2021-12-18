#include "MiniDraw.h"
#include <QIcon>
#include <QToolBar>

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget();
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();
	//setdialog();

	setCentralWidget(view_widget_);
}

void MiniDraw::Creat_Action() {
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);

	Action_Line = new QAction(QIcon("Icons\\line.png"), tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(QIcon("Icons\\rect.png"),tr("&Rect"), this);
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

	Action_Ellipse = new QAction(QIcon("Icons\\ellipse.png"), tr("&Ellipse"), this);
	connect(Action_Ellipse, &QAction::triggered, view_widget_, &ViewWidget::setEllipse);

	Action_Polygon = new QAction(QIcon("Icons\\polygon.png"), tr("&Polygon"), this);
	connect(Action_Polygon, &QAction::triggered, view_widget_, &ViewWidget::setPolygon);

	Action_Freehand = new QAction(QIcon("Icons\\freehand.png"), tr("&Freehand"), this);
	connect(Action_Freehand, &QAction::triggered, view_widget_, &ViewWidget::setFreehand);

	Action_setpencolor = new QAction(QIcon("Icons\\pen_color.png"), tr("&Setpencolor"), this);
	connect(Action_setpencolor, &QAction::triggered, view_widget_, &ViewWidget::setpencolor);

	Action_setbrushcolor = new QAction(QIcon("Icons\\brush_color.png"), tr("&Setbrushcolor"), this);
	connect(Action_setbrushcolor, &QAction::triggered, view_widget_, &ViewWidget::setbrushcolor);

	Action_delete = new QAction(QIcon("Icons\\delete.png"), tr("&delete"), this);
	connect(Action_delete, &QAction::triggered, view_widget_, &ViewWidget::delete_);
}

void MiniDraw::Creat_ToolBar() {
	OperationBar = addToolBar(tr("&Operation"));
	OperationBar->addAction(Action_delete);
	
	pToolBar = addToolBar(tr("&Figure"));
	pToolBar->addAction(Action_About);
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Polygon);
	pToolBar->addAction(Action_Freehand);
	
	styleBar = addToolBar(tr("&Style"));
	styleBar->addAction(Action_setpencolor);
	styleBar->addAction(Action_setbrushcolor);
}

void MiniDraw::Creat_Menu() {
	operation_Menu = menuBar()->addMenu(tr("&Operation"));
	operation_Menu->addAction(Action_delete);
	
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));
	pMenu->addAction(Action_About);
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Polygon);
	pMenu->addAction(Action_Freehand);

	style_Menu = menuBar()->addMenu(tr("&Style"));
	style_Menu->addAction(Action_setpencolor);
	style_Menu->addAction(Action_setbrushcolor);

}




void MiniDraw::AboutBox() {
	QMessageBox::about(this, tr("About"), tr("MiniDraw"));
}



MiniDraw::~MiniDraw() {}