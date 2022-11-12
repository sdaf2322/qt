#include "chattingform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChattingForm w;
    ChattingForm w1;
    w.show();
    w1.show();
    return a.exec();
}
