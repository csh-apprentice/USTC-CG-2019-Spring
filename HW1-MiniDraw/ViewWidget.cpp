#include "ViewWidget.h"
#include "qdebug.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setFocusPolicy(Qt::TabFocus);
	draw_status_ = false;
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
	toppoints.clear();
	delete pen_color_change;
	delete brush_color_change;
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}
void ViewWidget::setEllipse()
{
	type_ = Shape::kEllipse;
}
void ViewWidget::setPolygon()
{
	type_ = Shape::kPolygon;
}
void ViewWidget::setFreehand()
{
	type_ = Shape::kFreehand;
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	deselected_all();
	if (Qt::LeftButton == event->button())
	{
		switch (type_)
		{
		case Shape::kLine:
			shape_ = new Line();
			break;
		case Shape::kDefault:
			break;

		case Shape::kRect:
			shape_ = new Rect();
			break;
		case Shape::kEllipse:
			shape_ = new MEllipse();
			break;
		case Shape::kPolygon:
			setMouseTracking(true);
		    shape_ = new MPolygon();
			
			if (toppoints.size() == 0)
			{
				toppoints.push_back(event->pos());
				
			}
			break;
		case Shape::kFreehand:
			setMouseTracking(true);
			shape_ = new Freehand();

			if (toppoints.size() == 0)
			{
				toppoints.push_back(event->pos());

			}
			break;
		}
		if (shape_ != NULL)
		{
			draw_status_ = true;
			start_point_ = end_point_ = event->pos();
			shape_->set_start(start_point_);
			shape_->set_end(end_point_);
			
			if (type_ != Shape::kDefault)
			{
				shape_->set_pen(pen_);
			}
		}
	}
	else if(Qt::RightButton==event->button()&&draw_status_==true&&type_==Shape::kPolygon)
	{
		toppoints.push_back(QPoint(event->pos()));
		shape_ = new MPolygon(toppoints);
		shape_->set_pen(pen_);
		shape_->set_brush(brush_);
		shape_->setSelected(true);
		shape_->set_center_point();
		shape_->set_width();
		shape_->set_heigth();
		qDebug() << "point size is " << toppoints.size();
		qDebug() << "center point is " << shape_->get_center_point();
		shape_list_.push_back(shape_);
		toppoints.clear();
		shape_ = new MPolygon();
		draw_status_ = false;
		setMouseTracking(false);
	}
	update();
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	switch (type_)
	{
	case Shape::kFreehand:
		end_point_ = event->pos();
		shape_->set_end(end_point_);
		toppoints.push_back(end_point_);
		break;
	default:
		if (draw_status_ && shape_ != NULL)
		{
			end_point_ = event->pos();
			shape_->set_end(end_point_);

		}
	}
}


void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (draw_status_ == true)
	{
		switch (type_)
		{
		case Shape::kPolygon:                     //if the type is Polygon, we use the vector to storage its top points' information.
		{
			toppoints.push_back(end_point_);
			
			break;
		}
		case Shape::kFreehand:
		{
			toppoints.push_back(end_point_);
			shape_ = new Freehand(toppoints);
			shape_->set_center_point();
			shape_->set_width();
			shape_->set_heigth();
			shape_->set_pen(pen_);
			brush_.setStyle(Qt::SolidPattern);
			shape_->setSelected(true);
			shape_list_.push_back(shape_);
			shape_ = NULL;
			draw_status_ = false;
			setMouseTracking(false);
			toppoints.clear();
		}
		default:
			if (shape_ != NULL)
			{
				draw_status_ = false;
				shape_->set_width();
				shape_->set_heigth();
				shape_->set_center_point();
				shape_->set_pen(pen_);
				shape_->set_brush(brush_);
				shape_->setSelected(true);
				shape_list_.push_back(shape_);
				shape_ = NULL;
			}
		}
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	std::vector<QPoint>::const_iterator p, q;

	for (int i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i]->isSelected())
		{
			shape_list_[i]->set_pen(pen_);
			shape_list_[i]->set_brush(brush_);
	     }
		shape_list_[i]->Draw(painter);
	
	}
	if (draw_status_ == true)
	{
		painter.setPen(pen_);
		brush_.setStyle(Qt::SolidPattern);
		painter.setBrush(brush_);
		shape_->set_brush(brush_);
		//qDebug() << "current brush is" << brush_;
		//system("pause");
		switch (type_)
		{
		case Shape::kPolygon:
		  p = q = toppoints.begin();
		if (toppoints.begin()!=toppoints.end())
		{
			qDebug() << "the size is " << toppoints.size();
			
			
			for (q++;q != toppoints.end();p++, q++)
			{
				painter.drawLine(*p, *q);
			}
			qDebug() << "the end point is" << end_point_;
			painter.drawLine(*p, end_point_);
			
		}
		break;
		case Shape::kFreehand:
			p = q = toppoints.begin();
			if (toppoints.begin() != toppoints.end())
			{
				//qDebug() << "the size is " << toppoints.size();
				for (q++;q != toppoints.end();p++, q++)
				{
					painter.drawLine(*p, *q);
				}
				painter.drawLine(*p, end_point_);
			}
			break;
		default:
			if (shape_ != NULL) {
				shape_->Draw(painter);
				qDebug() << "the end point is" << end_point_;
			}
		}
	}
	update();
}

void ViewWidget::setpencolor()
{
	QColor mine = pen_color_change->getColor();
	if(mine.isValid())
	pen_.setColor(mine);
	
}

void ViewWidget::setbrushcolor()
{
	QColor mine = brush_color_change->getColor();
	if (mine.isValid())
		brush_.setColor(mine);
}

void ViewWidget::delete_()
{
	auto& temp = shape_list_.begin();
	while (temp != shape_list_.end())
	{
		if ((*temp)->isSelected())
			temp=shape_list_.erase(temp);
		else
			temp++;
	}
}

void ViewWidget::deselected_all()
{
	for (auto& shape : shape_list_)
	{
		if (shape->isSelected())
			shape->setSelected(false);
	}
}