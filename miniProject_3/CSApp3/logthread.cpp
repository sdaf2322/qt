#include "logthread.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

LogThread::LogThread(QObject *parent)
    : QThread{parent}
{
    QString format = "yyyyMMdd_hhmmss";
    filename = QString("log_%1.txt").arg(QDateTime::currentDateTime().toString(format));        // 파일명, 현재 날짜와 시간을 파일명으로 설정
}

void LogThread::run()
{
    Q_FOREVER {
        saveData();
        sleep(60);                                                                              // 1분마다 저장
    }
}

void LogThread::appendData(QTreeWidgetItem* item)                                               /*로그로 저장할 데이터를 받아오는 함수*/
{
    itemList.append(item);                                                                      // 로그로 저장할 데이터를 리스트에 삽입
}

void LogThread::saveData()                                                                      /*로그정보를 텍스트파일로 저장하는 함수*/
{
    if(itemList.count() > 0) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                                 // 파일이 쓰기모드나 텍스트 파일이 아니라면 리턴
            return;

        QTextStream out(&file);
        foreach(auto item, itemList) {                                                          // 리스트에 저장된 정보들을 텍스트 파일에 씀
            out << item->text(0) << ", ";
            out << item->text(1) << ", ";
            out << item->text(2) << ", ";
            out << item->text(3) << ", ";
            out << item->text(4) << ", ";
            out << item->text(5) << "\n";
        }
        file.close();
    }
}
