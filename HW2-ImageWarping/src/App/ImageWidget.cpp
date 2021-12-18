#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include <QDebug>
#include <OWarping.h>
#include <IDW_Warping.h>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	my_warp = new OWarping();
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);
	//qDebug() << "painter is " << painter.brush.;


	// Draw image
	if (ptr_image_!=NULL)
	{
		image_start_point_ = QPoint((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2);
		QRect rect = QRect((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2, ptr_image_->width(), ptr_image_->height());
		painter.drawImage(rect, *ptr_image_);
	}


	//Draw warping line
	painter.setPen(Qt::red);
	//qDebug() << "the size of line list is " << line_list.size();
	for (int i = 0;i < line_list.size();i++)
	{
		line_list[i]->Draw(painter);
		//qDebug() << "the " << i << " point is (" << line_list[i]->get_end_point().x()<<","<<line_list[i]->get_end_point().y()<<")";
	}
	if (draw_status)
	  painter.drawLine(start_point_, end_point_);
	update();
	painter.end();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	current_file_ = fileName;

	// Load file
	if (!fileName.isEmpty())
	{if(!check_ptr())
		ptr_image_ = new QImage();
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;

	set_save_false();
	update();
}

void ImageWidget::Save()
{
	if (!check_ptr())
		return;
	if(current_file_.isEmpty())
	   SaveAs();
    else
      {
	ptr_image_->save(current_file_);
       }


set_save_true();                              //set the saving status true
}

void ImageWidget::SaveAs()
{
	if (!check_ptr())
		return;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
	current_file_ = filename;

	set_save_true();                             //set the save status true
}

void ImageWidget::Invert()
{
	if (!check_ptr())
		return;
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			//std::cout << "(" << i << "," << j << ")" << std::endl;
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (!check_ptr())
		return;
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();


	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	if (!check_ptr())
		return;
	QImage old = *ptr_image_;                                      //checking the saving status
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}
	if (old != *ptr_image_)
		set_save_false();
	update();
}

void ImageWidget::Restore()
{
	if (!check_ptr())
		return;
	if (ptr_image_ != ptr_image_backup_)
		set_save_false();
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}


void ImageWidget::sethorizontal(bool type)
{
	horizontal = type;
}

void ImageWidget::setvertical(bool type)
{
	vertical = type;
}

void ImageWidget::set_mirror_type()
{
	if (!check_ptr())
		return;
	QStringList mirror_type_;
	mirror_type_ << tr("horizontally only") << tr("vertically only") << tr("both horizontally and vertically");
	bool ok;
	QString Item = QInputDialog::getItem(this, tr("Please input the mirror action's type"), tr("mirror type"), mirror_type_, 0, false, &ok);
	if (ok && !mirror_type_.isEmpty())
	{
		set_save_false();
		if (Item == "horizontally only")
			//connect(action_mirror_, &QAction::triggered, imagewidget_, &ImageWidget::Mirror(1,0));
		{
			sethorizontal(true);
			setvertical(false);
		}

		else if (Item == "vertically only")
		{
			setvertical(true);
			sethorizontal(false);
		}
		else
		{
			sethorizontal(true);
			setvertical(true);
		}
	}
	Mirror(horizontal, vertical);
}

void ImageWidget::set_save_false()
{ 
	save_status = false;
}

void ImageWidget::set_save_true()
{
	save_status = true;
}

bool ImageWidget::maybe_saved()
{
	if (save_status == true)
		return true;
	else
		return false;
}

void ImageWidget::Close()
{
	delete ptr_image_;
	ptr_image_ = NULL;
	current_file_.clear();
	line_list.clear();

}

bool ImageWidget::check_ptr()
{
	if (!ptr_image_)
		return false;
	else
		return true;
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (warp_status == false)
		return;
	if (Qt::LeftButton == event->button())
	{
		draw_status = true;
		warp_line = new Line();
		start_point_ = event->pos();
		end_point_ = event->pos();
		warp_line->set_start_point(start_point_);
		warp_line->set_end_point(end_point_);
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (warp_status && draw_status)
	{
		end_point_ = event->pos();
		warp_line->set_end_point(end_point_);

	}
	

}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (warp_status && draw_status)
	{
		end_point_ = event->pos();
		warp_line->set_end_point(end_point_);
		qDebug() << "the end point is " << end_point_-image_start_point_;
		draw_status = false;
		line_list.push_back(warp_line);
		//delete warp_line;
		warp_line = NULL;
	}
}

void ImageWidget::set_warp_status_true()
{
	warp_status = true;
}

void ImageWidget::IDW_Warping_()
{
	if (!check_ptr())
		return;

	my_warp = new IDW_Warping();
	if (warp_status == false && line_list.empty())
		return;
	QPair<QPoint, QPoint> anchor_point;
	for (auto temp : line_list)
	{
		anchor_point.first = temp->get_start_point() - image_start_point_;
		anchor_point.second = temp->get_end_point() - image_start_point_;
		my_warp->add_pair(anchor_point);
	}

	my_warp->get_size_();
	my_warp->init_IDW_();
	QImage image_tmp(*(ptr_image_));

	int width = ptr_image_->width();
	int height = ptr_image_->height();
	int num_change_ = 0;
	ptr_image_->fill(QColor(255, 255, 255));

	for (int i = 0;i < width;i++)
		for (int j = 0;j < height;j++)
		{
			QPoint old_point(i, j);
			QPoint new_point;

			new_point = my_warp->change_point_(old_point);

			if (new_point.x() >= 0 && new_point.x() < width && new_point.y() >= 0 && new_point.y() < height)
				//std::cout << "(" << i << "," << j << ")" << std::endl;
			{
				num_change_++;
				//std::cout << "one point change" << std::endl;
				QRgb color = image_tmp.pixel(old_point.x(), old_point.y());
				ptr_image_->setPixel(new_point.x(), new_point.y(), qRgb(qRed(color), qGreen(color), qBlue(color)));
			}
		}


	//std::cout << "("<<my_warp->change_point_(QPoint(128,128)).x() <<","<< my_warp->change_point_(QPoint(128,128)).y()<<")"<<std::endl;
	//my_warp->change_point_(QPoint(128, 128));
	//my_warp->get_weighed_function();
	
	//my_warp->show_point_();

	//my_warp->current_weighed_function(QPoint(0,0));
//std::cout << "done!" << std::endl;
std::cout << "number of changes is " << num_change_<<std::endl;
	update();
}

void ImageWidget::RBF_Warping_()
{
	if (!check_ptr())
		return;

	my_warp = new RBF_Warping();
	if (warp_status == false && line_list.empty())
		return;
	QPair<QPoint, QPoint> anchor_point;
	for (auto temp : line_list)
	{
		anchor_point.first = temp->get_start_point() - image_start_point_;
		anchor_point.second = temp->get_end_point() - image_start_point_;
		my_warp->add_pair(anchor_point);
	}
	QImage image_tmp(*(ptr_image_));
	my_warp->get_size_();
	my_warp->init_RBF_();
	
	int width = ptr_image_->width();
	int height = ptr_image_->height();
	ptr_image_->fill(QColor(255, 255, 255));
	int num_change_ = 0;
	for (int i = 0;i < width;i++)
		for (int j = 0;j < height;j++)
		{
			QPoint old_point(i, j);
			QPoint new_point;

			new_point = my_warp->change_point_(old_point);

			if (new_point.x() >= 0 && new_point.x() < width && new_point.y() >= 0 && new_point.y() < height)
				//std::cout << "(" << i << "," << j << ")" << std::endl;
			{
				num_change_++;
				//std::cout << "one point change" << std::endl;
				QRgb color = image_tmp.pixel(old_point.x(), old_point.y());
				ptr_image_->setPixel(new_point.x(), new_point.y(), qRgb(qRed(color), qGreen(color), qBlue(color)));
			}
		}
	std::cout << "number of changes is " << num_change_ << std::endl;
	update();

}

void ImageWidget::FillHole()
{
	if (!check_ptr())
		return;

	if (warp_status == false && line_list.empty())
		return;

	int width = ptr_image_->width();
	int height = ptr_image_->height();
	int num_change_ = 0;
	for (int i = 0;i < width;i++)
		for (int j = 0;j < height;j++)
		{
				QRgb color = ptr_image_->pixel(i,j);
				if (qRed(color)==255&&qGreen(color)==255&&qBlue(color)==255)
				{
					num_change_++;
					int red_a = 0, green_a = 0,blue_a = 0;
					int num=0;
					for (int width_start =i-1;width_start < i+2;width_start++)
						for (int height_start = j - 1;height_start < j + 2;height_start++)
						{
							if (width_start >= 0 && height_start >= 0 && width_start < width && height_start < height)
								if (width_start!= i || height_start != j)
								{
									red_a += qRed(ptr_image_->pixel(width_start,height_start));
									green_a += qGreen(ptr_image_->pixel(width_start, height_start));
									blue_a += qBlue(ptr_image_->pixel(width_start, height_start));
									num++;
								}
						}
					red_a /= num;
					green_a /= num;
					blue_a /= num;
					ptr_image_->setPixel(i, j, qRgb(red_a, green_a, blue_a));
				}

			
		}
	std::cout << "number of changes is " << num_change_ << std::endl;
	update();


}

void ImageWidget::AboutDeveloper()
{
	QMessageBox::information(NULL, "About",
		QString::fromLocal8Bit(
			"PB19000216 程诗涵\n"\
			"（PS：我ANN不知为何用不了，自己用kdtree实现了一个KNN算法填补空白）\n"
		),
		QMessageBox::Yes
	);
}

void ImageWidget::AboutInstructions()
{
	QMessageBox::information(NULL, "About",
		QString::fromLocal8Bit(
			"享受生活\n"

		),
		QMessageBox::Yes
	);
}