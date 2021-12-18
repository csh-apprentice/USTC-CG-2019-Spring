

#include <ui_viewwidget.h>

#include "Shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellipse.h"
#include "MPolygon.h"
#include "Freehand.h"

#include <qevent.h>
#include <qpainter.h>
#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <QPen>
#include <QBrush>

#include <vector>



class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidgetClass ui;

private:
	//using to storage the information of a certain figure
	bool draw_status_;
	QPoint start_point_;
	QPoint end_point_;
	Shape::Type type_;
	Shape* shape_;
	std::vector<Shape*> shape_list_;
	std::vector<QPoint> toppoints;                 //storage the top points information when drawing Polygon or freehand  
	QPen pen_;
	QBrush brush_=QBrush(QColor::fromRgb(255, 255, 255, 0));

	//dialogs
	QColorDialog* pen_color_change= new QColorDialog(QColor::fromRgb(0, 0, 0, 255), this);
	QColorDialog* brush_color_change = new QColorDialog(QColor::fromRgb(255,255, 255, 0), this);


	
public:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public:
	void paintEvent(QPaintEvent*);
	void deselected_all();								//deselect all the figure
signals:
public slots:
	//menthods to response operation action
	void delete_();                   //delete the selected figure

	//methods for initializing applications
	void setLine();
	void setRect();
	void setEllipse();
	void setPolygon();
	void setFreehand();
	
	
	//methods to response changing style
	void setpencolor();
	void setbrushcolor();

};

