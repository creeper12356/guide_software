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
  ImageViewer(QWidget* parent = 0);
  ~ImageViewer();

protected:

  const QString& currentFile() const;
  const QGraphicsScene* scene() const;
  const QPixmap& image() const;
  QMenu* menu();

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

private:
  Ui::ImageViewer* ui;
  QString mCurrentFile;
  QGraphicsScene* mScene;
  QPixmap mImage;
  int mZoomin{0};
  QMenu* mMenu;
  QToolBar* mToolBar;
};
