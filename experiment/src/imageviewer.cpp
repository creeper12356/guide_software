#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QToolBar>
#include <QMenu>

ImageViewer::ImageViewer(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ImageViewer) {
  ui->setupUi(this);

  //初始化菜单
  connect(ui->actionZoom_In, &QAction::triggered, this, &ImageViewer::zoomIn);
  connect(ui->actionZoom_Out, &QAction::triggered, this, &ImageViewer::zoomOut);
  connect(ui->actionFit, &QAction::triggered, this,
          &ImageViewer::fitWindow);
  connect(ui->actionSaveAs, &QAction::triggered, this,&ImageViewer::saveAs);

  mMenu = new QMenu(this);
  mMenu->addAction(ui->actionSaveAs);

  //初始化工具栏
  mToolBar = new QToolBar(this);
  addToolBar(Qt::TopToolBarArea,mToolBar);
  mToolBar->addAction(ui->actionZoom_In);
  mToolBar->addAction(ui->actionZoom_Out);
  mToolBar->addAction(ui->actionFit);
}

ImageViewer::~ImageViewer()
{
    delete ui;
}

const QString &ImageViewer::currentFile() const
{
    return mCurrentFile;
}

const QGraphicsScene *ImageViewer::scene() const
{
    return mScene;
}

const QPixmap &ImageViewer::image() const
{
    return mImage;
}

QMenu *ImageViewer::menu()
{
    return mMenu;
}

QToolBar *ImageViewer::toolBar()
{
    return mToolBar;
}

void ImageViewer::open(const QString &fileName)
{
    if(QFile::exists(fileName)){
        mCurrentFile = fileName;
    }
    else {
        mCurrentFile = "";
    }
    showImage();
}

void ImageViewer::showImage() {
  // Reset scale
  ui->graphicsView->resetMatrix();

  // Image is a Pixmap
  mImage.load(mCurrentFile);

  mScene = new QGraphicsScene(this);

  mScene->addPixmap(mImage);
  mScene->setSceneRect(mImage.rect());
  ui->graphicsView->setScene(mScene);

  scaleImageToFitWindow();
  QFileInfo file(mCurrentFile);
  setWindowTitle(file.fileName());
  statusBar()->showMessage(QString("%1 %2x%3px %4 kB")
                               .arg(file.fileName())
                               .arg(mImage.width())
                               .arg(mImage.height())
                               .arg(QFile(mCurrentFile).size() / 1000.0));
}

void ImageViewer::scaleImageToFitWindow() {
  int y = ui->graphicsView->size().height() - mImage.height();
  int x = ui->graphicsView->size().width() - mImage.width();

  double scaleFactor = 1.0;

  double xsf = ui->graphicsView->size().width() / (double)mImage.width();
  double ysf = ui->graphicsView->size().height() / (double)mImage.height();

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
  mZoomin++;
  if (mZoomin <= 12)
    ui->graphicsView->scale(1.2, 1.2);
  else
    mZoomin--;
}

void ImageViewer::zoomOut() {
  mZoomin--;
  if (mZoomin >= -8)
    ui->graphicsView->scale(1 / 1.2, 1 / 1.2);
  else
      mZoomin++;
}

void ImageViewer::saveAs()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,"另存为",mCurrentFile,"*.png");
    if(saveFileName.isEmpty()){
        return ;
    }
    mImage.save(saveFileName);
}

void ImageViewer::fitWindow() {
  showFullScreen();
  ui->graphicsView->showFullScreen();

  ui->graphicsView->resetMatrix();
  scaleImageToFitWindow();
}

void ImageViewer::resizeEvent(QResizeEvent*) {
  ui->graphicsView->resetMatrix();
  scaleImageToFitWindow();
}

void ImageViewer::contextMenuEvent(QContextMenuEvent *e)
{
    if(!mCurrentFile.isEmpty()){
        mMenu->exec(e->globalPos());
    }
}


void ImageViewer::mouseDoubleClickEvent(QMouseEvent*) {
  setWindowState(isFullScreen() ? Qt::WindowMaximized : Qt::WindowFullScreen);
}
