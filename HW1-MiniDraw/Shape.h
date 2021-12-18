#pragma once

#include <QtGui>
#include <assert.h>
#include <QPen>
#include <cmath>
#include <QPainter>

class Shape
{
public:
	//initialization and deconstruction
	Shape();
	virtual ~Shape();

	//figure function
	virtual void Draw(QPainter& paint) = 0;
	void set_start(QPoint s);
	void set_end(QPoint e);
	QPoint get_start_point();
	QPoint get_end_point();
	

	//frame function
	QPoint get_center_point();                            //return the center point of the figure
	virtual void set_width()=0;                           //set the width of the figure's frame
	virtual void set_heigth()=0;                          //set the heigth of the figure's frame
	virtual void set_center_point();
	
	void Draw_frame(QPainter &framepainter);

	//style function
	void set_pen(QPen &inputpen);
	void set_brush(QBrush& inputbrush);

	//set_function
	bool	isSelected();							//return whether the figure is selected
	void	setSelected(bool status);				//reset the status of the figure being selected

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllipse = 3,
		kPolygon = 4,
		kFreehand=5
	};

protected:
	//figure
	QPoint start;
	QPoint end;

	//frame
	QPoint center_point;                  //center point of the frame
	int width;                            //width of the frame
	int heigth;                                //heigth of the frame

	//style
	QPen s_pen_=QPen();
	QBrush s_brush = QBrush();

	//set
	bool selected=false;
};




