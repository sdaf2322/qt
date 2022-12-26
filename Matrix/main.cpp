#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutCreateWindow("OpenGL");
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
