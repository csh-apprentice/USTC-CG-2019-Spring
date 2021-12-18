#include "mainwindow.h"
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);
//std::cout << "initiate done!" << std::endl;
	setGeometry(300, 150, 800, 450);

	imagewidget_ = new ImageWidget();
	setCentralWidget(imagewidget_);

	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
	
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *e)
{

}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{
	
}

void MainWindow::CreateActions()
{
	action_new_ = new QAction(QIcon(":/MainWindow/Resources/images/new.jpg"), tr("&New"), this);
	action_new_->setShortcut(QKeySequence::New);
	action_new_->setStatusTip(tr("Create a new file"));
	// connect ...

	action_open_ = new QAction(QIcon(":/MainWindow/Resources/images/open.jpg"), tr("&Open..."), this);
	action_open_->setShortcuts(QKeySequence::Open);
	action_open_->setStatusTip(tr("Open an existing file"));
	connect(action_open_, &QAction::triggered, imagewidget_, &ImageWidget::Open);

	action_save_ = new QAction(QIcon(":/MainWindow/Resources/images/save.jpg"), tr("&Save"), this);
	action_save_->setShortcuts(QKeySequence::Save);
	action_save_->setStatusTip(tr("Save the document to disk"));
	connect(action_save_, &QAction::triggered, imagewidget_, &ImageWidget::Save);

	action_saveas_ = new QAction(tr("Save &As..."), this);
	action_saveas_->setShortcuts(QKeySequence::SaveAs);
	action_saveas_->setStatusTip(tr("Save the document under a new name"));
	connect(action_saveas_, &QAction::triggered, imagewidget_, &ImageWidget::SaveAs);

	action_close_ = new QAction(tr("Close"), this);
	action_close_->setShortcuts(QKeySequence::Close);
	action_close_->setStatusTip(tr("Close the cuurent file"));
	connect(action_close_, &QAction::triggered, imagewidget_, &ImageWidget::Close);

	action_invert_ = new QAction(tr("Inverse"), this);
	action_invert_->setStatusTip(tr("Invert all pixel value in the image"));
	connect(action_invert_, &QAction::triggered, imagewidget_, &ImageWidget::Invert);

	action_mirror_ = new QAction(tr("Mirror"), this);
	action_mirror_->setStatusTip(tr("Mirror image vertically or horizontally"));
	connect(action_mirror_, &QAction::triggered, imagewidget_, &ImageWidget::set_mirror_type);


	action_gray_ = new QAction(tr("Grayscale"), this);
	action_gray_->setStatusTip(tr("Gray-scale map"));
	connect(action_gray_, &QAction::triggered, imagewidget_, &ImageWidget::TurnGray);

	action_restore_ = new QAction(tr("Restore"), this);
	action_restore_->setStatusTip(tr("Show origin image"));
	connect(action_restore_, &QAction::triggered, imagewidget_, &ImageWidget::Restore);

	action_draw_ = new QAction(tr("draw"), this);
	action_draw_->setStatusTip(tr("set anchored points"));
	connect(action_draw_, &QAction::triggered, imagewidget_, &ImageWidget::set_warp_status_true);

	action_IDW_warp_ = new QAction(tr("IDW"), this);
	action_IDW_warp_->setStatusTip(tr("IDW warp of the given image"));
	connect(action_IDW_warp_, &QAction::triggered, imagewidget_, &ImageWidget::IDW_Warping_);

	action_RBF_warp_ = new QAction(tr("RBF"), this);
	action_RBF_warp_->setStatusTip(tr("RBF warp of the given image"));
	connect(action_RBF_warp_, &QAction::triggered, imagewidget_, &ImageWidget::RBF_Warping_);

	action_Fill_hole_ = new QAction(tr("FillHole"), this);
	action_Fill_hole_->setStatusTip(tr("Fill the hole of the white area"));
	connect(action_Fill_hole_, &QAction::triggered, imagewidget_, &ImageWidget::FillHole);

	about_developer_action_ = new QAction(tr("About developer"), this);
	about_developer_action_->setStatusTip(tr("About developer"));
	connect(about_developer_action_, &QAction::triggered, imagewidget_, &ImageWidget::AboutDeveloper);

	about_instructions_action_ = new QAction(tr("About Instructions"), this);
	about_instructions_action_->setStatusTip(tr("About Instructions"));
	connect(about_instructions_action_, &QAction::triggered, imagewidget_, &ImageWidget::AboutInstructions);

}

void MainWindow::CreateMenus()
{
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
	menu_file_->addAction(action_new_);
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);
	menu_file_->addAction(action_close_);

	menu_edit_ = menuBar()->addMenu(tr("&Edit"));
	menu_edit_->setStatusTip(tr("Edit menu"));
	menu_edit_->addAction(action_invert_);
	menu_edit_->addAction(action_mirror_);
	menu_edit_->addAction(action_gray_);
	menu_edit_->addAction(action_restore_);

	menu_warp_=menuBar()->addMenu(tr("&Warp"));
	menu_warp_->setStatusTip(tr("Warp the Image"));
	menu_warp_->addAction(action_draw_);
	menu_warp_->addAction(action_IDW_warp_);
	menu_warp_->addAction(action_RBF_warp_);
	menu_warp_->addAction(action_Fill_hole_);

	menu_about_ = menuBar()->addMenu(tr("&About"));
    menu_about_->setStatusTip(tr("About")); 
	menu_about_->addAction(about_developer_action_);
	menu_about_->addAction(about_instructions_action_);

}

void MainWindow::CreateToolBars()
{
	toolbar_file_ = addToolBar(tr("File"));
	toolbar_file_->addAction(action_new_);
	toolbar_file_->addAction(action_open_);
	toolbar_file_->addAction(action_save_);
	toolbar_file_->addAction(action_close_);

	// Add separator in toolbar 
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_invert_);
	toolbar_file_->addAction(action_mirror_);
	toolbar_file_->addAction(action_gray_);
	toolbar_file_->addAction(action_restore_);

	// Add warping application in toolbar
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_draw_);
	toolbar_file_->addAction(action_IDW_warp_);
	toolbar_file_->addAction(action_RBF_warp_);
	toolbar_file_->addAction(action_Fill_hole_);
}

void MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
