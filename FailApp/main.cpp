#include "widget.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Widget w;
    w.setAge(0);
    QTest::qExec(&w);

    return a.exec();
}
