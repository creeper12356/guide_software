#pragma once

#include <QMainWindow>

#include "ui_imageviewer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageViewer;
}
QT_END_NAMESPACE

class ImageViewer : public QMainWindow {
  Q_OBJECT

 public:
  ImageViewer(QWidget* parent = nullptr);
  ~ImageViewer();

 private:
  Ui::ImageViewer* ui;
  QString currentFile;
  QGraphicsScene* scene;
  QPixmap image;
  int zoomin{0};

 private slots:
  void open();
  void zoomIn();
  void zoomOut();
  void properties();
  void openContainingFolder();
  void exit();
  void showImage();
  void previousImage();
  void nextImage();
  void about();
  void trash();
  void fullscreen();
  void rotate();
  void save();
  void saveAs(QString);
  void scaleImageToFitWindow();
  void mouseDoubleClickEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);
  void rename();
};
