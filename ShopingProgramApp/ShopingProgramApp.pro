QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientitem.cpp \
    clientmanagerform.cpp \
    main.cpp \
    mainwindow.cpp \
    productitem.cpp \
    productmanagerform.cpp \
    shopitem.cpp \
    shopmanagerform.cpp

HEADERS += \
    clientitem.h \
    clientmanagerform.h \
    mainwindow.h \
    productitem.h \
    productmanagerform.h \
    shopitem.h \
    shopmanagerform.h

FORMS += \
    clientmanagerform.ui \
    mainwindow.ui \
    productmanagerform.ui \
    shopmanagerform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
