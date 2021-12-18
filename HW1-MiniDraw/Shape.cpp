#include "Shape.h"
#include "qdebug.h"

Shape::Shape()
{
	
}

Shape::~Shape()
{
}

void Shape::set_start(QPoint s)
{
	start = s;
}

void Shape::set_end(QPoint e)
{
	end = e;
}

QPoint Shape::get_start_point()
{
	return start;
}

QPoint Shape::get_end_point()
{
	return end;
}

QPoint Shape::get_center_point()
{
	return center_point;
}

void Shape::set_center_point()
{
	center_point = (start+end)/2;
}

void Shape::Draw_frame(QPainter &framepainter)
{
	QBrush mine_brush;
	QPen mine_pen;
	mine_pen.setColor(QColor::fromRgb(100, 100, 100));
	mine_pen.setStyle(Qt::DotLine);
	
	framepainter.setPen(mine_pen);
	framepainter.setBrush(mine_brush);
	//framepainter.setPen(Qt::DotLine);
	framepainter.drawRect(center_point.x()-width/2, center_point.y()-heigth/2, width, heigth);
}

void Shape::set_pen(QPen &inputpen)
{
	s_pen_=inputpen;
}

void Shape::set_brush(QBrush& inputbrush)
{
	s_brush = inputbrush;
}

bool Shape::isSelected()
{
	return(selected);
}

void Shape::setSelected(bool status)
{
	selected = status;
}
