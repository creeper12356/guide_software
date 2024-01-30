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
  QMenu* menu;

 protected slots:
  void open(const QString& fileName);
  void zoomIn();
  void zoomOut();
  void saveAs();
  void showImage();
  void about();
  void fullscreen();
  void scaleImageToFitWindow();
protected:
  void mouseDoubleClickEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);
  void contextMenuEvent(QContextMenuEvent* e);

};
