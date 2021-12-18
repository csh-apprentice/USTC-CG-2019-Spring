#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	
	shape_ = NULL;
	type_ = Shape::kNone;
	figure_type_ = Shape::kNone;
	//draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;

	//point_start_ = QPoint(0, 0);
	//point_end_ = QPoint(0, 0);

	source_window_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
	return image_mat_.rows;
}

void ImageWidget::set_draw_status_to_rec_choose()
{
	
	type_ = Shape::kRec;
	figure_type_ = Shape::kRec;
}

void ImageWidget::set_draw_status_to_polygon_choose()
{
	figure_type_ = Shape::kPolygon;
	type_ = Shape::kPolygon;
}

void ImageWidget::set_draw_status_to_paste()
{
	type_ =Shape:: kPaste;
}

void ImageWidget::set_draw_status_to_polygon_paste()
{
	type_ = Shape::kPolygonPaste;
}

void ImageWidget::set_draw_status_to_possion()
{
	type_ = Shape::kPossion_Paste;
}
cv::Mat ImageWidget::image()
{
	return image_mat_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	std::vector<QPoint>::const_iterator p, q;
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QImage image_show = QImage((unsigned char*)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_show.width(), image_show.height());
	painter.drawImage(rect, image_show);


	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	if (is_choosing_)
	{
		switch (type_)
		{
		case Shape::kRec:

			painter.drawRect(start_.x(), start_.y(),
				end_.x() - start_.x(), end_.y() - start_.y());
			break;
		case Shape::kPolygon:
		{
			p = q = toppoints.begin();
		if (toppoints.begin() != toppoints.end())
		{
			//qDebug() << "the size is " << toppoints.size();


			for (q++;q != toppoints.end();p++, q++)
			{
				painter.drawLine(*p, *q);
			}
			//qDebug() << "the end point is" << end_;
			painter.drawLine(*p, end_);

		}
		break;
		}

		default:
			break;
		}
	}
	else
		if (shape_ != nullptr)
		{
			shape_->Draw(painter);
			//std::cout << "shape_'s lowbound is " << shape_->getlowbound() << std::endl;
			//std::cout << "shape_'s highbound is " << shape_->gethighbound() << std::endl;
			//std::cout << "shape_;s height is " << shape_->get_height_() << std::endl;
		}
		
	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (type_)
		{
		case Shape::kRec:
			is_choosing_ = true;
			//shape_ = new KRect();
			//shape_->point_start_ = shape_->point_end_ = mouseevent->pos();
			start_ = end_ = mouseevent->pos();
			break;
		case Shape::kPolygon:
		{
			start_ = end_ = mouseevent->pos();
			is_choosing_ = true;
			setMouseTracking(true);
			//shape_ = new Polygon();

			if (toppoints.size() == 0)
			{
				toppoints.push_back(mouseevent->pos());

			}
			break;
		}
		case Shape::kPaste:
		{  
		
		is_pasting_ = true;

		// Start point in object image
		xpos = mouseevent->pos().rx();
		ypos = mouseevent->pos().ry();

		// Start point in source image
		xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
		ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

		// Width and Height of rectangle region
		int w = source_window_->imagewidget_->shape_->point_end_.rx()
			- source_window_->imagewidget_->shape_->point_start_.rx() + 1;
		int h = source_window_->imagewidget_->shape_->point_end_.ry()
			- source_window_->imagewidget_->shape_->point_start_.ry() + 1;

		// Paste
		if ((xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
		{
			// Restore image
		//	*(image_) = *(image_backup_);

			// Paste
			for (int i = 0; i < w; i++)
			{
				for (int j = 0; j < h; j++)
				{
					image_mat_.at<cv::Vec3b>(ypos + j, xpos + i) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>(ysourcepos + j, xsourcepos + i);
				}
			}
		}
		break;
		}
		 
		   
		case Shape::kPolygonPaste:
		{is_pasting_ = true;

		// Start point in object image
		xpos = mouseevent->pos().rx();
		ypos = mouseevent->pos().ry();

		// Start point in source image
		xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
		ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

		int change_x = xpos - xsourcepos;
		int change_y = ypos - ysourcepos;

		int lowbound = source_window_->imagewidget_->shape_->getlowbound();
		int highbound = source_window_->imagewidget_->shape_->gethighbound();
		int xmin = source_window_->imagewidget_->shape_->get_x_min_();
		int xmax = source_window_->imagewidget_->shape_->get_x_max_();

		if (lowbound + change_y >= 0 && highbound + change_y < image_mat_.rows && xmin + change_x>0 && xmax + change_x < image_mat_.cols)
		{
			assert(source_window_->imagewidget_->my_polygon_.num == source_window_->imagewidget_->my_polygon_.contain_point_list_.size());
			std::cout<<"my_polygon_num is " << source_window_->imagewidget_->my_polygon_.num << std::endl;
			for (int i = 0;i < source_window_->imagewidget_->my_polygon_.num;i++)
			{
				//std::cout << "polygon paste happens!" << std::endl;
				int current_x_ = source_window_->imagewidget_->my_polygon_.contain_point_list_[i].second.x();
				int current_y_ = source_window_->imagewidget_->my_polygon_.contain_point_list_[i].second.y();
				image_mat_.at<cv::Vec3b>( current_y_ + change_y,current_x_ + change_x) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>( current_y_,current_x_);
			}
		}

		break;
		}


		case Shape::kPossion_Paste:
		{
			is_pasting_ = true;

			// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();

			// Start point in source image
			xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
			ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->shape_->point_end_.rx()
				- source_window_->imagewidget_->shape_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->shape_->point_end_.ry()
				- source_window_->imagewidget_->shape_->point_start_.ry() + 1;

			//mix possion pasting whenmoving the mouse
			edit_ = new Possionmix();
			edit_->set_big_matrix(xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, Shape::kRec, source_window_->imagewidget_->image_mat_);
			//Possion Paste
			if ((xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image
			//	*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_mat_.at<cv::Vec3b>(ypos + j, xpos + i) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>(ysourcepos + j, xsourcepos + i);
					}
				}
				
				//std::cout << "right here init!" << std::endl;
				edit_->set_vector_V(xsourcepos, ysourcepos, xpos, ypos, source_window_->imagewidget_->shape_, Shape::kRec, source_window_->imagewidget_->image_mat_, image_mat_backup_);
				//std::cout << "right here a!" << std::endl;				
				edit_->set_all_matrix_(xsourcepos, ysourcepos,xpos, ypos, source_window_->imagewidget_->shape_, Shape::kRec, image_mat_);
				//std::cout << "right here b!" << std::endl;
				edit_->Possion_change(xpos, ypos, xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, Shape::kRec, image_mat_);
			}
			break;
		}

	
		break;

		default:
			break;
		}
		update();
	}
	else if (Qt::RightButton == mouseevent->button() && is_choosing_ == true && type_ == Shape::kPolygon)
	{
	toppoints.push_back(QPoint(mouseevent->pos()));
	shape_ = new KPolygon(toppoints);
	shape_->point_start_ = shape_->point_end_ = start_;
	my_polygon_=KPolygon(toppoints);
	toppoints.clear();
	is_choosing_ = false;
	setMouseTracking(false);
	type_ = Shape::kNone;
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (type_)
	{
	case Shape::kRec:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			//shape_->point_end_ = mouseevent->pos();
			end_ = mouseevent->pos();
		}
		break;

	case Shape::kPolygon:
		end_ = mouseevent->pos();
		break;
	case Shape::kPaste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();

			// Start point in source image
			xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
			ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->shape_->point_end_.rx()
				- source_window_->imagewidget_->shape_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->shape_->point_end_.ry()
				- source_window_->imagewidget_->shape_->point_start_.ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image 
				image_mat_ = image_edit_.clone();
				//Restore();
				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_mat_.at<cv::Vec3b>(ypos + j, xpos + i) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>(ysourcepos + j, xsourcepos + i);
					}
				}
			}
			break;
		}
	case Shape::kPolygonPaste:
	{is_pasting_ = true;

	// Start point in object image
	xpos = mouseevent->pos().rx();
	ypos = mouseevent->pos().ry();

	// Start point in source image
	xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
	ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

	int change_x = xpos - xsourcepos;
	int change_y = ypos - ysourcepos;

	int lowbound = source_window_->imagewidget_->shape_->getlowbound();
	int highbound = source_window_->imagewidget_->shape_->gethighbound();
	int xmin = source_window_->imagewidget_->shape_->get_x_min_();
	int xmax = source_window_->imagewidget_->shape_->get_x_max_();

	if (lowbound + change_y >= 0 && highbound + change_y < image_mat_.rows && xmin + change_x>0 && xmax + change_x < image_mat_.cols)
	{
		KPolygon* temp = (KPolygon*)(source_window_->imagewidget_->shape_);
		// Restore image 
		image_mat_ = image_edit_.clone();
		for (int i = 0;i < temp->num;i++)
		{
			int current_x_ = temp->contain_point_list_[i].second.x();
			int current_y_ = temp->contain_point_list_[i].second.y();
			image_mat_.at<cv::Vec3b>(current_y_ + change_y,current_x_ + change_x) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>(current_y_,current_x_ );
		}
	}

	break;
	}

		

	case Shape::kPossion_Paste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();

			// Start point in source image
			xsourcepos = source_window_->imagewidget_->shape_->point_start_.rx();
			ysourcepos = source_window_->imagewidget_->shape_->point_start_.ry();

			//edit_->set_big_matrix(xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, Shape::kRec, source_window_->imagewidget_->image_mat_);
			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->shape_->point_end_.rx()
				- source_window_->imagewidget_->shape_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->shape_->point_end_.ry()
				- source_window_->imagewidget_->shape_->point_start_.ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image 
				image_mat_ = image_edit_.clone();

				// Possion Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_mat_.at<cv::Vec3b>(ypos + j, xpos + i) = source_window_->imagewidget_->image_mat_.at <cv::Vec3b>(ysourcepos + j, xsourcepos + i);
					}
				}
				
				//std::cout << "right here init!" << std::endl;
				edit_->set_vector_V(xsourcepos, ysourcepos, xpos, ypos, source_window_->imagewidget_->shape_, Shape::kRec, source_window_->imagewidget_->image_mat_, image_mat_backup_);
				//std::cout << "right here a!" << std::endl;				
				edit_->set_all_matrix_(xsourcepos, ysourcepos,xpos, ypos, source_window_->imagewidget_->shape_, Shape::kRec, image_mat_);
				//std::cout << "right here b!" << std::endl;
				edit_->Possion_change(xpos, ypos, xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, Shape::kRec, image_mat_);
			}
		}
		break;

	default:
		break;
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (type_)
	{
	case Shape::kRec:
		if (is_choosing_)
		{
			std::cout << "the start point is (" << start_.x() << "," << start_.y() << ")" << std::endl;
			std::cout << "the end point is (" << end_.x() << "," << end_.y() << ")" << std::endl;
			end_ = mouseevent->pos();
			shape_ = new KRect(start_,end_);
			//shape_->point_start_ = start_;
			//shape_->point_end_ = mouseevent->pos();	
			is_choosing_ = false;
			type_ = Shape::kNone;
		}
		break;
	case Shape::kPolygon:                     //if the type is Polygon, we use the vector to storage its top points' information.
	{   end_ = mouseevent->pos();
		toppoints.push_back(end_);

		break;
	}
	case Shape::kPaste:
		if (is_pasting_)
		{// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();
			image_edit_ = image_mat_.clone();
			is_pasting_ = false;
			type_ = Shape::kNone;
		}
		break;
	case Shape::kPolygonPaste:
		if (is_pasting_)
		{// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();
			image_edit_ = image_mat_.clone();
			is_pasting_ = false;
			type_ = Shape::kNone;
		}
		break;
	case Shape::kPossion_Paste:
		if (is_pasting_)
		{// Start point in object image
			xpos = mouseevent->pos().rx();
			ypos = mouseevent->pos().ry();
			image_edit_ = image_mat_.clone();
			is_pasting_ = false;
			type_ = Shape::kNone;
		}
		break;

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		//only for bmp?
		image_mat_ = cv::imread(filename.toLatin1().data());
		cv::cvtColor(image_mat_, image_mat_, cv::COLOR_BGR2RGB);
		image_mat_backup_ = image_mat_.clone();
		image_edit_ = image_mat_.clone();
		//image_->load(filename);
		//*(image_backup_) = *(image_);
	}
	cout << "image size: " << image_mat_.rows << ' ' << image_mat_.cols << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	cv::Mat image_save;
	cv::cvtColor(image_mat_, image_save, cv::COLOR_BGR2RGB);
	cv::imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255 - (*iter)[0];
		(*iter)[1] = 255 - (*iter)[1];
		(*iter)[2] = 255 - (*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (image_mat_.empty())
		return;
	int width = image_mat_.cols;
	int height = image_mat_.rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, width - 1 - i);
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(j, width - 1 - i);
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, i);
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		int itmp = ((*iter)[0] + (*iter)[1] + (*iter)[2]) / 3;
		(*iter)[0] = itmp;
		(*iter)[1] = itmp;
		(*iter)[2] = itmp;
	}

	update();
}

void ImageWidget::Restore()
{
	image_mat_ = image_mat_backup_.clone();
	update();
}

void ImageWidget::PossionEdiitng_normal_()
{
	if (source_window_->imagewidget_->image_mat_.empty())
		return;
	edit_ = new Possion();
	std::cout << "right here init!" << std::endl;
	edit_->set_vector_V(xsourcepos, ysourcepos,xpos,ypos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, source_window_->imagewidget_->image_mat_,image_mat_);
	std::cout << "right here a!" << std::endl;
	edit_->set_big_matrix(xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, source_window_->imagewidget_->image_mat_);
	std::cout << "right here b!" << std::endl;
	edit_->set_all_matrix_(xsourcepos, ysourcepos,xpos,ypos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, image_mat_);
	std::cout << "right here c!" << std::endl;
	edit_->Possion_change(xpos, ypos, xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, image_mat_);
	image_edit_ = image_mat_.clone();
	update();
}

void ImageWidget::PossionEdiitng_mix_()
{
	if (source_window_->imagewidget_->image_mat_.empty())
		return;
	edit_ = new Possionmix();
	std::cout << "right here init!" << std::endl;
	edit_->set_vector_V(xsourcepos, ysourcepos, xpos, ypos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, source_window_->imagewidget_->image_mat_, image_mat_backup_);
	std::cout << "right here a!" << std::endl;
	edit_->set_big_matrix(xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, source_window_->imagewidget_->image_mat_);

	edit_->set_all_matrix_(xsourcepos, ysourcepos,xpos, ypos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, image_mat_);
	std::cout << "right here b!" << std::endl;
	edit_->Possion_change(xpos, ypos, xsourcepos, ysourcepos, source_window_->imagewidget_->shape_, source_window_->imagewidget_->figure_type_, image_mat_);
	update();
}