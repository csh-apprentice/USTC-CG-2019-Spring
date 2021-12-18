#pragma once
#include <QWidget>
#include <shape.h>
#include <rec.h>
#include <Polygon.h>
#include <OPossion.h>
#include <Possion.h>
#include <Possion_mix_.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

//enum DrawStatus
//{
//	kRec, 
//	kPaste, 
//	kNone
//};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_rec_choose();
	void set_draw_status_to_polygon_choose();
	void set_draw_status_to_paste();
	void set_draw_status_to_polygon_paste();
	void set_draw_status_to_possion();
	cv::Mat image();
	void set_source_window(ChildWindow* childwindow);

	

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
	// File IO
	void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

	//Posssion Editing
	void PossionEdiitng_normal_();                                  //the simpliest possion editing
	void PossionEdiitng_mix_();                                  //the simpliest possion editing
public:
	QPoint						start_;					// Left top point of rectangle region
	QPoint						end_;						// Right bottom point of rectangle region
	Shape*                       shape_;
	Shape::Type                  type_;
	Shape::Type                     figure_type_;              //the choosed figure type in the source window
	KPolygon                  my_polygon_;
	OPossion*                   edit_;

private:
	cv::Mat						image_mat_;						// image
	cv::Mat						image_mat_backup_;
	cv::Mat                     image_edit_;

	//editing point

	std::vector<QPoint> toppoints;                 //storage the top points information when drawing Polygon or freehand

	// Start point in object image
	int xpos;
	int ypos;

	// Start point in source image
	int xsourcepos ;
	int ysourcepos;


	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	//DrawStatus					draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_pasting_;

	
};

