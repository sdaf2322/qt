#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DOMReader w;
    w.show();
    return a.exec();
}
