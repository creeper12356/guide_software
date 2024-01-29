#include "inc.h"
#include "imageviewer.h"
class ImageDisplay : public ImageViewer
{
public:
    explicit ImageDisplay(QWidget* parent = 0);
    void loadFromFile(const QString& fileName);
};
