#include "Freehand.h"

Freehand::Freehand()
{}


Freehand::Freehand(std::vector<QPoint> toppoints)
{
	size_points = toppoints.size();
	for (int i = 0;i < size_points;i++)
	{
		temp_points.push_back(toppoints[i]);
	}
}

Freehand::~Freehand()
{

}

void Freehand::set_width()
{
	int minx = temp_points[0].x(), maxx = temp_points[0].x();
	for (auto& temp : temp_points)
	{
		if (temp.x() > maxx)
			maxx = temp.x();
		if (temp.x() < minx)
			minx = temp.x();
	}

	width = maxx - minx;
}


void Freehand::set_heigth()
{
	int miny = temp_points[0].y(), maxy = temp_points[0].y();
	for (auto& temp : temp_points)
	{
		if (temp.y() > maxy)
			maxy = temp.y();
		if (temp.y() < miny)
			miny = temp.y();
	}

	heigth = maxy - miny;
}

void Freehand::set_center_point()
{
	int minx = temp_points[0].x(), maxx = temp_points[0].x();
	int miny = temp_points[0].y(), maxy = temp_points[0].y();
	int centerx, centery;
	for (auto& temp : temp_points)
	{
		if (temp.x() > maxx)
			maxx = temp.x();
		if (temp.x() < minx)
			minx = temp.x();
	}
	for (auto& temp : temp_points)
	{
		if (temp.y() > maxy)
			maxy = temp.y();
		if (temp.y() < miny)
			miny = temp.y();
	}
	centerx = (maxx + minx) / 2;
	centery = (maxy + miny) / 2;
	center_point = QPoint(centerx, centery);
}

void Freehand::Draw(QPainter& painter)
{
	QPoint* mine = PointArray();
	painter.setPen(s_pen_);
	painter.drawPolyline(mine, size_points);
	delete[]mine;
	if (isSelected())
		Draw_frame(painter);
}

QPoint* Freehand::PointArray()
{
	QPoint* temp = new QPoint[size_points];
	assert(temp != NULL);
	for (int i = 0;i < size_points;i++)
	{
		temp[i] = temp_points[i];
	}
	return temp;
}