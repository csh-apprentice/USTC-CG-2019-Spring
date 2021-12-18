#pragma once
#include <QWidget>
#include <Line.h>
#include <vector>
#include <OWarping.h>
#include <IDW_Warping.h>
#include <RBF_Warping.h>
#include <iosfwd>


QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);

public:
	//About mirror type
	void sethorizontal(bool type);
	void setvertical(bool type);

	//about save status
	void set_save_false();
	void set_save_true();
	bool maybe_saved();                                         //returing the status whether the image were saved if any changes occured
	bool check_ptr();

public:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public:
	void set_warp_status_true();


public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file
	void Close();

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

	//about mirror type
	void set_mirror_type();

	//about warping
	void IDW_Warping_();
	void RBF_Warping_();

	//about Fillhole
	void FillHole();

	//About
	void AboutInstructions();
	void AboutDeveloper();

private:
	QImage		*ptr_image_;				                    // image 
	QImage		*ptr_image_backup_;
	bool horizontal=false;
	bool vertical=false;
	bool save_status=false;
	QString current_file_;                                      //current file name
	QPoint image_start_point_;                                  //image start point

	bool warp_status=false;                                     //check if the user is exectuate the warping 
	QPoint start_point_;                                        //the starting point of warping
	QPoint end_point_;                                          //the end point of warping
	Line* warp_line;
	std::vector<Line*> line_list;
	bool draw_status=false;
	OWarping* my_warp;
};

