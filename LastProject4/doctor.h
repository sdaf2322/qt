//#ifndef DOCTOR_H
//#define DOCTOR_H

//#include <QWidget>
//#include <QDataStream>

//namespace Ui {
//class Doctor;
//}

//class QTextEdit;
//class QLineEdit;
//class QTcpSocket;
//class QPushButton;
//class QFile;
//class QProgressDialog;
//class LogThread;

//typedef enum {
//    Chat_Login,                                                     // 로그인(서버 접속)
//    Chat_In,                                                        // 채팅방 입장
//    Chat_Talk,                                                      // 채팅
//    Chat_Out,                                                       // 채팅방 퇴장
//    Chat_LogOut,                                                    // 로그 아웃(서버 단절)
//    Chat_Invite,                                                    // 초대
//    Chat_KickOut,                                                   // 강퇴
//} Chat_Status;

//class Doctor : public QWidget
//{
//    Q_OBJECT

//public:
//    const int PORT_NUMBER = 8000;                                   // 포트번호
//    Doctor(QWidget *parent = nullptr);
//    ~Doctor();

//private slots:
//    void receiveData( );                                            // 서버에서 데이터가 올 때
//    void sendData( );                                               // 서버로 데이터를 보낼 때
//    void disconnect( );                                             // 서버와의 연결종료
//    void sendProtocol(Chat_Status, char*, int = 1020);              // 데이터 전송
//    void sendFile();                                                // 파일전송
//    void goOnSend(qint64);                                          // 파일전송 과정
//    void connectButton_clicked();                                   // 로그인 버튼 누를경우

//private:
//    void closeEvent(QCloseEvent*) override;
//    QTcpSocket *clientSocket;                                       // 클라이언트용 소켓
//    QTcpSocket *fileClient;
//    QProgressDialog* progressDialog;                                // 파일 진행 확인
//    QFile* file;                                                    // 서버로 보내는 파일
//    qint64 loadSize;                                                // 파일의 크기
//    qint64 byteToWrite;                                             // 보내는 파일의 크기
//    qint64 totalSize;                                               // 전체 파일의 크기
//    QByteArray outBlock;                                            // 전송을 위한 데이터
//    bool isSent;                                                    // 파일 서버에 접속되었는지 확인
//    LogThread* logThread;

//    Ui::Doctor *ui;


//};
//#endif // DOCTOR_H
