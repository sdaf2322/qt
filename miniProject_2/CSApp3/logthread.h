#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogThread : public QThread
{
    Q_OBJECT
public:
    explicit LogThread(QObject *parent = nullptr);

private:
    void run();

    QList<QTreeWidgetItem*> itemList;                               // 트리위젯의 항목에 해당하는 정보들을 보관하는 리스트
    QString filename;                                               // 파일명

signals:
    void send(int data);                                            // 데이터를 보내는 시그널

public slots:
    void appendData(QTreeWidgetItem*);                              // 리스트에 정보를 저장하는 함수
    void saveData();                                                // 데이터를 텍스트 파일로 저장하는 함수
};

#endif // LOGTHREAD_H
