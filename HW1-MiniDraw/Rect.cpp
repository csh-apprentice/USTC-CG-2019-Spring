#include "Rect.h"
#include <qdebug.h>

Rect::Rect()
{
}

Rect::~Rect()
{
}

void Rect::set_width()
{
	width= fabs(start.x() - end.x()) ;
}

void Rect::set_heigth()
{
	heigth = fabs(start.y() - end.y());
}

void Rect::Draw(QPainter& painter)
{    
	
	painter.setPen(s_pen_);
	s_brush.setStyle(Qt::SolidPattern);
	painter.setBrush(s_brush);
	painter.drawRect(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
	if (isSelected())
		Draw_frame(painter);
}
