#ifndef MAINSERVER_H
#define MAINSERVER_H

#include <QWidget>
#include <QList>
#include <QHash>
#include <QMenu>

class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogThread;

namespace Ui {
class MainServer;
}

typedef enum {
    Send_Info,
    Get_Info,
    Request_Info,
} Patient_Info;

class MainServer : public QWidget
{
    Q_OBJECT

public:
    explicit MainServer(QWidget *parent = nullptr);
    ~MainServer();

    void sendProtocol(Patient_Info, QByteArray*, int = 1020);
    void sendData(QString, QString, QString, QString, QString, QString, QString, QString, QString, QString, QString);


private:
    const int BLOCK_SIZE = 1024;
    const int PORT_NUMBER = 8000;

    Ui::MainServer *ui;
    QTcpServer *chatServer;
    QTcpServer *fileServer;
    QHash<Patient_Info, QTcpSocket*> socketHash;

private slots:
    void acceptConnection();                /* 파일 서버 */
    void readClient();

    void clientConnect( );                  /* 채팅 서버 */
    void receiveData( );
    void removeClient( );
    void addClient(int, QString);
    void inviteClient();
    void kickOut();
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_deletePushButton_clicked();

signals:
    void From_DB(QString, QString, QString, QString, QString, QString, QString, QString, QString, QString, QString);
};
#endif // MAINSERVER_H
