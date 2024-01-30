#pragma once

#include <QMainWindow>
#include <QGraphicsScene>

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

protected:
  Ui::ImageViewer* ui;
  QString currentFile;
  QGraphicsScene* scene;
  QPixmap image;
  int zoomin{0};
  QMenu* menu;

public:
  void open(const QString& fileName);
protected slots:
  void zoomIn();
  void zoomOut();
  void saveAs();
  void showImage();
  void fitWindow();
  void scaleImageToFitWindow();
protected:
  void mouseDoubleClickEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);
  void contextMenuEvent(QContextMenuEvent* e);

};
