//#include "doctor.h"
////#include "logthread.h"
//#include "ui_doctor.h"

//#include <QTextEdit>
//#include <QLineEdit>
//#include <QPushButton>
//#include <QBoxLayout>
//#include <QRegularExpression>
//#include <QRegularExpressionValidator>
//#include <QDataStream>
//#include <QTcpSocket>
//#include <QApplication>
//#include <QThread>
//#include <QMessageBox>
//#include <QSettings>
//#include <QFileDialog>
//#include <QFile>
//#include <QFileInfo>
//#include <QProgressDialog>

//#define BLOCK_SIZE      1024

//Doctor::Doctor(QWidget *parent)
//    : QWidget(parent), isSent(false),
//    ui(new Ui::Doctor){
//    ui->setupUi(this);

//    QSettings settings("ChatClient", "Chat Client");
//    ui->name->setText(settings.value("ChatClient/ID").toString());
//    ui->name->setPlaceholderText("Input Client Name");                                          // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열

//    ui->serverAddress->setText("127.0.0.1");                                                    // IP주소
//    ui->serverAddress->setPlaceholderText("Input IP Number");                                   // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열
//    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."                          // IP주소를 입력받는 마스크, 정규 표현식
//                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
//                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
//                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
//    QRegularExpressionValidator validator(re);                                                  // 정규 표현식 저장
//    ui->serverAddress->setPlaceholderText("Server IP Address");                                 // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열
//    ui->serverAddress->setValidator(&validator);                                                // 서버어드레스에 정규 표현식 설정

//    ui->serverPort->setText(QString::number(PORT_NUMBER));                                      // 서버포트에 포트번호 표시
//    ui->serverPort->setInputMask("00000;_");                                                    // 마스크 설정
//    ui->serverPort->setPlaceholderText("Server Port No");                                       // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열

//    connect(ui->connectButton, SIGNAL(clicked()), SLOT(connectButton_clicked()));               // connectButton 누르면 connectButton_clicked 슬롯 발생

//    connect(ui->fileButton, SIGNAL(clicked( )), SLOT(sendFile( )));                             // fileButton 누르면 sendFile 슬롯 발생
//    ui->fileButton->setDisabled(true);                                                          // fileButton 비활성화

//    connect(ui->quitButton, SIGNAL(clicked( )), qApp, SLOT(quit( )));                           // 종료버튼 누르면 프로그램 종료


////------------------------------------------------------------------- 파일 시그널 ---------------------------------------------------------------//
//    fileClient = new QTcpSocket(this);                                                          // 파일전송을 위한 소켓 생성
//    connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));                  // 파일소켓에 데이터를 쓸 때 goOnSend 슬롯 발생

//    progressDialog = new QProgressDialog(0);                                                    // progressDialog 생성 후 0으로 초기화
//    progressDialog->setAutoClose(true);                                                         // 파일전송이 끝나면 progressDialog 자동종료
//    progressDialog->reset();                                                                    // progressDialog 리셋

//    setWindowTitle(tr("Chat Client"));                                                          // 윈도우 타이틀 설정




//    logThread = new LogThread(this);                                                            // 로그스레드 생성
//    logThread->start();                                                                         // 로그스레드 시작

//    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));                // 세이브 버튼 누르면 saveData 슬롯 발생
//}

//Doctor::~Doctor( )                                                                  /*ChattingForm 소멸자*/
//{
//    QSettings settings("ChatClient", "Chat Client");
//    settings.setValue("ChatClient/ID", ui->name->text());                                       //
//    logThread->terminate();                                                                     // 로그스레드 종료
//}

//void Doctor::connectButton_clicked()                                                      /*connectButton 누를 경우 발생하는 슬롯*/
//{

//}

//void Doctor::disconnect( )                                                                /*disconnect 슬롯*/
//{
//    QMessageBox::critical(this, tr("Chatting Client"),                                          // 메세지박스로 경고메세지 띄움
//                          tr("Disconnect from Server"));
//    ui->inputLine->setEnabled(false);                                                           // 채팅 입력 칸 비활성화
//    ui->name->setReadOnly(false);                                                               // 네임 라인에딧 읽기전용으로 설정 해제
//    ui->sentButton->setEnabled(false);                                                          // sentButton 비활성화
//    ui->connectButton->setText(tr("Log in"));                                                   // connectButton Log in으로 글자 바꿈
//}

//void Doctor::goOnSend(qint64 numBytes)                                                    /*goOnSend 슬롯*/
//{
//    byteToWrite -= numBytes;                                                                    // 남은 파일 용량으로 보낼 용량을 계속해서 뺌
//    outBlock = file->read(qMin(byteToWrite, numBytes));                                         // 보낼 용량만큼 파일을 읽어서 outBlock 변수에 저장
//    fileClient->write(outBlock);                                                                // 파싱한 파일을 소켓으로 보냄

//    progressDialog->setMaximum(totalSize);                                                      // progressDialog의 맥시멈 값을 totalSize로 설정
//    progressDialog->setValue(totalSize-byteToWrite);                                            // progressDialog의 값은 totalSize에서 남은 데이터 수를 뺌

//    if (byteToWrite == 0) {                                                                     // 파일전송이 끝난 경우
//        qDebug("File sending completed!");
//        progressDialog->reset();                                                                // progressDialog 리셋
//    }
//}

//void Doctor::sendFile()                                                                   /*sendFile 슬롯*/
//{
//    loadSize = 0;
//    byteToWrite = 0;
//    totalSize = 0;
//    outBlock.clear();                                                                           // 남은 파일 용량, 파일 전체 용량, 보낼 파일 0으로 초기화

//    QString filename = QFileDialog::getOpenFileName(this);
//    if(filename.length()) {
//        file = new QFile(filename);                                                             // 파일생성
//        file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

//        qDebug() << QString("file %1 is opened").arg(filename);
//        progressDialog->setValue(0);                                                            // progressDialog 0으로 세팅

//        if (!isSent) {                                                                          // 파일을 처음 보내는 경우
//            fileClient->connectToHost(ui->serverAddress->text( ),                               // 소켓을 연결하고 isSent를 false에서 true로 바꿈
//                                      ui->serverPort->text( ).toInt( ) + 1);
//            isSent = true;
//        }

//        byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량
//        loadSize = 1024;

//        QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
//        out << qint64(0) << qint64(0) << filename << ui->name->text();                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명, 이름 넣음

//        totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
//        byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함

//        out.device()->seek(0);                                                                  // 처음 위치로 감
//        out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀

//        fileClient->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄

//        progressDialog->setMaximum(totalSize);                                                  // progressDialog 최대값 totalSize로 설정
//        progressDialog->setValue(totalSize-byteToWrite);                                        // progressDialog 값 totalSize에서 남은 용량을 뺀 값으로 설정
//        progressDialog->show();                                                                 // progressDialog 표시
//    }
//    qDebug() << QString("Sending file %1").arg(filename);
//}
