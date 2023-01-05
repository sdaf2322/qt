#include "servcer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Servcer w;
    w.show();
    return a.exec();
}
