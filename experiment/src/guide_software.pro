QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    data/model/appmodel.cpp \
    data/utils/choice.cpp \
    logic/core/core.cpp \
    logic/core/heatmapcore.cpp \
    logic/core/performancecore.cpp \
    logic/utils/compatibility.cpp \
    logic/utils/subcore.cpp \
    logic/utils/taskmanager.cpp \
    main.cpp \
    ui/utils/customgraphicsview.cpp \
    ui/utils/imageviewer.cpp \
    ui/widget/choicewidget.cpp \
    ui/widget/consoledock.cpp \
    ui/widget/heatmapviewer.cpp \
    ui/widget/logdock.cpp \
    ui/widget/probewidget.cpp \
    ui/window/aboutdialog.cpp \
    ui/window/choiceguide.cpp \
    ui/window/dependencyinstaller.cpp \
    ui/window/mainpage.cpp \
    ui/window/passworddialog.cpp

FORMS += \
    ui/utils/imageviewer.ui \
    ui/widget/choicewidget.ui \
    ui/widget/probewidget.ui \
    ui/window/aboutdialog.ui \
    ui/window/choiceguide.ui \
    ui/window/dependencyinstaller.ui \
    ui/window/mainpage.ui

HEADERS += \
    data/model/appmodel.h \
    data/utils/choice.h \
    inc.h \
    logic/core/core.h \
    logic/core/heatmapcore.h \
    logic/core/performancecore.h \
    logic/utils/compatibility.h \
    logic/utils/subcore.h \
    logic/utils/taskmanager.h \
    ui/utils/customgraphicsview.h \
    ui/utils/imageviewer.h \
    ui/widget/choicewidget.h \
    ui/widget/consoledock.h \
    ui/widget/heatmapviewer.h \
    ui/widget/logdock.h \
    ui/widget/probewidget.h \
    ui/window/aboutdialog.h \
    ui/window/choiceguide.h \
    ui/window/dependencyinstaller.h \
    ui/window/mainpage.h \
    ui/window/passworddialog.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../res.qrc
