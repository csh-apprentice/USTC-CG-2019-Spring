#include "MPolygon.h"
#include "qdebug.h"

MPolygon::MPolygon()
{}


MPolygon::MPolygon(std::vector<QPoint> toppoints)
{
	size_points = toppoints.size();
	for(int i=0;i<size_points;i++)
	{
		top_points.push_back(toppoints[i]);
	}
}

MPolygon::~MPolygon()
{
	//top_points.clear();
	
}

void MPolygon::set_width()
{
	int minx=top_points[0].x(),maxx=top_points[0].x();
	for (auto& temp: top_points)
	{
		if (temp.x() > maxx)
			maxx = temp.x();
		if (temp.x() < minx)
			minx = temp.x();
	}
	width = maxx - minx;
}

void MPolygon::set_heigth()
{
	int miny = top_points[0].y(), maxy = top_points[0].y();
	for (auto& temp : top_points)
	{
		if (temp.y() > maxy)
			maxy = temp.y();
		if (temp.y() < miny)
			miny = temp.y();
	}

	heigth = maxy - miny;
}

void MPolygon::set_center_point() 
{
	int minx = top_points[0].x(), maxx = top_points[0].x();
	int miny = top_points[0].y(), maxy = top_points[0].y();
	int centerx, centery;
	for (auto& temp : top_points)
	{
		if (temp.x() > maxx)
			maxx = temp.x();
		if (temp.x() < minx)
			minx = temp.x();
	}
	for (auto& temp : top_points)
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


void MPolygon::Draw(QPainter& painter)
{
	QPoint* mine = PointArray();
	painter.setPen(s_pen_);
	s_brush.setStyle(Qt::SolidPattern);
	painter.setBrush(s_brush);
	painter.drawPolygon(mine, size_points);
	if (isSelected())
		Draw_frame(painter);
	delete[]mine;
	//qDebug() << "Polygon drawing status= " << isSelected();
	
}

QPoint* MPolygon::PointArray()
{
	QPoint *temp = new QPoint[size_points];
	assert(temp != NULL);
	for(int i=0;i<size_points;i++)
	{
		temp[i] = top_points[i];
	}
	return temp;
}