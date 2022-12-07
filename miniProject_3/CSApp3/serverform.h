#ifndef SERVERFORM_H
#define SERVERFORM_H

#include <QWidget>
#include <QList>
#include <QHash>

class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogThread;

namespace Ui {
class ServerForm;
}

typedef enum {
    Chat_Login,             // 로그인(서버 접속)
    Chat_In,                // 채팅방 입장
    Chat_Talk,              // 채팅
    Chat_Out,               // 채팅방 퇴장
    Chat_LogOut,            // 로그 아웃
    Chat_Invite,            // 초대
    Chat_KickOut,           // 강퇴
} Chat_Status;

class ServerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ServerForm(QWidget *parent = nullptr);
    ~ServerForm();

private:
    const int BLOCK_SIZE = 1024;
    const int PORT_NUMBER = 8000;

    Ui::ServerForm *ui;
    QTcpServer *chatServer;
    QTcpServer *fileServer;
    QHash<quint16, QString> clientNameHash;                                     // 이름을 저장하는 해쉬
    QHash<QString, QTcpSocket*> clientSocketHash;                               // 소켓을 저장하는 해쉬
    QHash<QString, int> clientIDHash;                                           // 고객아이디를 저장하는 해쉬
    QMenu* menu;
    QFile* file;
    QProgressDialog* progressDialog;
    qint64 totalSize;
    qint64 byteReceived;
    QByteArray inBlock;
    LogThread* logThread;

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
};
#endif // SERVERFORM_H
