QT -= gui

TARGET = SaveURLS
TEMPLATE = lib

DEFINES += SAVEURLS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += saveurls.cpp

HEADERS += SaveURLS_global.h \
            saveurls.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
