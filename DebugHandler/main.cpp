#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>

void myDebugMsg(QtMsgType type, const QMessageLogContext &context,
                const QString &msg)
{
    switch(type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg.toStdString().c_str());
        abort();
        break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myDebugMsg);
    QCoreApplication a(argc, argv);
    qDebug("hbghjvhj");
    qWarning("dfsf");
    qFatal("saaa");
    //qInfo("");
    //qInfo();
    //qInfo();
    return a.exec();
}
