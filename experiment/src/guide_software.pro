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
    aboutdialog.cpp \
    choiceguide.cpp \
    dependencyinstaller.cpp \
    mainpage.cpp \
    choicewidget.cpp \
    imagedisplay.cpp \
    core.cpp \
    main.cpp \
    consoledock.cpp \
    taskmanager.cpp \
    passworddialog.cpp \
    choice.cpp \
    appmodel.cpp \
    imageviewer.cpp

HEADERS += \
    aboutdialog.h \
    choiceguide.h \
    dependencyinstaller.h \
    mainpage.h \
    choicewidget.h \
    imagedisplay.h \
    core.h \
    inc.h \
    consoledock.h \
    taskmanager.h \
    passworddialog.h \
    choice.h \
    appmodel.h \
    imageviewer.h

FORMS += \
    aboutdialog.ui \
    choiceguide.ui \
    dependencyinstaller.ui \
    mainpage.ui \
    choicewidget.ui \
    imageviewer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../res.qrc
