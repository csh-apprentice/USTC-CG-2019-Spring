#include "Line.h"
#include "qdebug.h"

Line::Line()
{
}

Line::~Line()
{
}

void Line::set_width()
{
	width = fabs(start.x() - end.x()) ;
}

void Line::set_heigth()
{
	heigth = fabs(start.y() - end.y()) ;
}

void Line::Draw(QPainter& painter)
{
	painter.setPen(s_pen_);
	painter.drawLine(start, end);
	qDebug() << "line drawing status= " << isSelected();
	if (isSelected())
		Draw_frame(painter);
}