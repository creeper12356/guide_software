#include "imageviewer.h"

#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QToolBar>
#include <QWheelEvent>
#include <QMenu>

#include "ui_imageviewer.h"

ImageViewer::ImageViewer(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ImageViewer) {
  ui->setupUi(this);

  QStringList argumentList = QApplication::arguments();
  if (argumentList.size() > 1) {
    currentFile = argumentList.at(1);
    showImage();
  }

  //menu
  connect(ui->actionZoom_In, &QAction::triggered, this, &ImageViewer::zoomIn);
  connect(ui->actionZoom_Out, &QAction::triggered, this, &ImageViewer::zoomOut);
  connect(ui->actionFullscreen, &QAction::triggered, this,
          &ImageViewer::fullscreen);
  connect(ui->actionSaveAs, &QAction::triggered, this,&ImageViewer::saveAs);

  // Toolbar
  QToolBar* toolBar = new QToolBar(this);
  addToolBar(Qt::TopToolBarArea,toolBar);
  toolBar->addAction(ui->actionZoom_In);
  toolBar->addAction(ui->actionZoom_Out);
  toolBar->addAction(ui->actionFullscreen);

  // Menu
  menu = new QMenu(this);
  menu->addAction(ui->actionSaveAs);
}

ImageViewer::~ImageViewer() { delete ui; }

void ImageViewer::open(const QString &fileName)
{
    if(QFile::exists(fileName)){
        currentFile = fileName;
    }
    else {
        currentFile = "";
    }
    showImage();
}

void ImageViewer::showImage() {
  // Reset scale
  ui->graphicsView->resetMatrix();

  // Image is a Pixmap
  image.load(currentFile);

  scene = new QGraphicsScene(this);

  scene->addPixmap(image);
  scene->setSceneRect(image.rect());
  ui->graphicsView->setScene(scene);

  scaleImageToFitWindow();
  QFileInfo file(currentFile);
  setWindowTitle(file.fileName());
  statusBar()->showMessage(QString("%1 %2x%3px %4 kB")
                               .arg(file.fileName())
                               .arg(image.width())
                               .arg(image.height())
                               .arg(QFile(currentFile).size() / 1000.0));
}

void ImageViewer::scaleImageToFitWindow() {
  int y = ui->graphicsView->size().height() - image.height();
  int x = ui->graphicsView->size().width() - image.width();

  double scaleFactor = 1.0;

  double xsf = ui->graphicsView->size().width() / (double)image.width();
  double ysf = ui->graphicsView->size().height() / (double)image.height();

  // There are 4 cases here -
  // 1. (+x,+y) Image fits well in the slot -> Do nothing
  // 2. (-x, +y) Image extends vertically
  // 3. (+x, -y) Image extends horizontally
  // 4. (-x, -y) Image extends both directions -> This has 2 cases

  if (x < 0 && y < 0) {
    if (x < y)
      scaleFactor = xsf;
    else
      scaleFactor = ysf;
  }

  if (x < 0 && y >= 0) scaleFactor = xsf;
  if (x >= 0 && y < 0) scaleFactor = ysf;

  ui->graphicsView->scale(scaleFactor, scaleFactor);
}


void ImageViewer::zoomIn() {
  zoomin++;
  if (zoomin <= 12)
    ui->graphicsView->scale(1.2, 1.2);
  else
    zoomin--;
}

void ImageViewer::zoomOut() {
  zoomin--;
  if (zoomin >= -8)
    ui->graphicsView->scale(1 / 1.2, 1 / 1.2);
  else
      zoomin++;
}

void ImageViewer::saveAs()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,"另存为",currentFile,"*.jpg *.png");
    if(saveFileName.isEmpty()){
        return ;
    }
    image.save(saveFileName);
}

void ImageViewer::fullscreen() {
  showFullScreen();
  ui->graphicsView->showFullScreen();

  ui->graphicsView->resetMatrix();
  scaleImageToFitWindow();
}

void ImageViewer::about() {
  QMessageBox::about(this, "About Coreavor",
                     "<center><h2>Coreavor</h2></center><br>"
                     "Simple image viewer<br>"
                     "Source code at <a style=\"color: #8AB8FE\" "
                     "href='https://github.com/Mithil467/Coreavor'>GitHub</a>");
}

void ImageViewer::resizeEvent(QResizeEvent*) {
  ui->graphicsView->resetMatrix();
  scaleImageToFitWindow();
}

void ImageViewer::contextMenuEvent(QContextMenuEvent *e)
{
    if(!currentFile.isEmpty()){
        menu->exec(e->globalPos());
    }
}


void ImageViewer::mouseDoubleClickEvent(QMouseEvent*) {
  setWindowState(isFullScreen() ? Qt::WindowMaximized : Qt::WindowFullScreen);
}
