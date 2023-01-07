#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(mainserver, SIGNAL(From_DB(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)),
            mainserver, SLOT(sendData(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)));

}



