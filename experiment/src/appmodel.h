#ifndef APPMODEL_H
#define APPMODEL_H
#include "inc.h"
class AppModel : public QObject
{
    Q_OBJECT
public:
    explicit AppModel(QObject *parent = 0);

signals:

public slots:
};

#endif // APPMODEL_H
