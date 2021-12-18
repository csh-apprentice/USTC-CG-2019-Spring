#include "Ellipse.h"

MEllipse::MEllipse()
{
}

MEllipse::~MEllipse()
{
}

void MEllipse::set_width()
{
	width = fabs(start.x() - end.x()) ;
}

void MEllipse::set_heigth()
{
	heigth = fabs(start.y() - end.y()) ;
}

void MEllipse::Draw(QPainter& painter)
{
	painter.setPen(s_pen_);
	s_brush.setStyle(Qt::SolidPattern);
	painter.setBrush(s_brush);
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
	if (isSelected())
		Draw_frame(painter);
}
