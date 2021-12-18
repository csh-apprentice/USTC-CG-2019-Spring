

#include <ui_minidraw.h>
#include "ViewWidget.h"

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h>
#include <QColor>
#include <QColorDialog>

class MiniDraw : public QMainWindow {
	Q_OBJECT

public:
	MiniDraw(QWidget* parent = 0);
	~MiniDraw();

	QMenu* operation_Menu;
	QMenu* pMenu;
	QMenu* style_Menu;
	QToolBar* OperationBar;
	QToolBar* pToolBar;
	QToolBar* styleBar;

	//operation action
	QAction* Action_delete;

	//figure action
	QAction* Action_About;
	QAction* Action_Line;
	QAction* Action_Rect;
	QAction* Action_Ellipse;
	QAction* Action_Polygon;
	QAction* Action_Freehand;


	//style action
	QAction* Action_setpencolor;
	QAction* Action_setbrushcolor;

	

	void Creat_Menu();
	void Creat_ToolBar();
	void Creat_Action();

	void AboutBox();

	//methods to response style actions
	//void	ChangeBrushColor();
	//void ChangePenColor();


private:
	Ui::MiniDrawClass ui;
	ViewWidget* view_widget_;
};
