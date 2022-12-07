#include "chattingform.h"
#include "logthread.h"
#include "ui_chattingform.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

#define BLOCK_SIZE      1024

ChattingForm::ChattingForm(QWidget *parent)
    : QWidget(parent), isSent(false),
    ui(new Ui::ChattingForm){
    ui->setupUi(this);

    // 연결한 서버 정보 입력을 위한 위젯들
    //name = new QLineEdit(this);

    QSettings settings("ChatClient", "Chat Client");
    ui->name->setText(settings.value("ChatClient/ID").toString());
    ui->name->setPlaceholderText("Input Client Name");                                          // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열

    ui->serverAddress->setText("127.0.0.1");                                                    // IP주소
    ui->serverAddress->setPlaceholderText("Input IP Number");                                   // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."                          // IP주소를 입력받는 마스크, 정규 표현식
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);                                                  // 정규 표현식 저장
    ui->serverAddress->setPlaceholderText("Server IP Address");                                 // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열
    ui->serverAddress->setValidator(&validator);                                                // 서버어드레스에 정규 표현식 설정

    ui->serverPort->setText(QString::number(PORT_NUMBER));                                      // 서버포트에 포트번호 표시
    ui->serverPort->setInputMask("00000;_");                                                    // 마스크 설정
    ui->serverPort->setPlaceholderText("Server Port No");                                       // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열

    ui->message->setReadOnly(true);                                                             // 채팅창은 읽기전용으로만 설정

    ui->inputLine->setPlaceholderText("Input Chatting");                                        // 라인에딧에 아무런 텍스트가 없을경우 표시되는 문자열
    connect(ui->inputLine, SIGNAL(returnPressed( )), SLOT(sendData( )));                        // 엔터 키 누르면 sendData 슬롯 발생
    connect(ui->inputLine, SIGNAL(returnPressed( )), ui->inputLine, SLOT(clear( )));            // 엔터 키 누르면 채팅 입력칸 비움
    connect(ui->sentButton, SIGNAL(clicked( )), SLOT(sendData( )));                             // sentButton 누르면 sendData 슬롯 발생
    connect(ui->sentButton, SIGNAL(clicked( )), ui->inputLine, SLOT(clear( )));                 // sentButton 누르면 채팅 입력칸 비움
    ui->inputLine->setDisabled(true);                                                           // 채팅 입력칸 비활성화
    ui->sentButton->setDisabled(true);                                                          // sentButton 비활성화

    connect(ui->connectButton, SIGNAL(clicked()), SLOT(connectButton_clicked()));               // connectButton 누르면 connectButton_clicked 슬롯 발생

    connect(ui->fileButton, SIGNAL(clicked( )), SLOT(sendFile( )));                             // fileButton 누르면 sendFile 슬롯 발생
    ui->fileButton->setDisabled(true);                                                          // fileButton 비활성화

    connect(ui->quitButton, SIGNAL(clicked( )), qApp, SLOT(quit( )));                           // 종료버튼 누르면 프로그램 종료

    clientSocket = new QTcpSocket(this);                                                        // 채팅을 위한 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << clientSocket->errorString( ); });                                 // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                        // 연결 종료시 disconnect 슬롯

    fileClient = new QTcpSocket(this);                                                          // 파일전송을 위한 소켓 생성
    connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));                  // 파일소켓에 데이터를 쓸 때 goOnSend 슬롯 발생

    progressDialog = new QProgressDialog(0);                                                    // progressDialog 생성 후 0으로 초기화
    progressDialog->setAutoClose(true);                                                         // 파일전송이 끝나면 progressDialog 자동종료
    progressDialog->reset();                                                                    // progressDialog 리셋

    setWindowTitle(tr("Chat Client"));                                                          // 윈도우 타이틀 설정

    logThread = new LogThread(this);                                                            // 로그스레드 생성
    logThread->start();                                                                         // 로그스레드 시작

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));                // 세이브 버튼 누르면 saveData 슬롯 발생
}

ChattingForm::~ChattingForm( )                                                                  /*ChattingForm 소멸자*/
{
    clientSocket->close( );                                                                     // 소켓 닫음
    QSettings settings("ChatClient", "Chat Client");
    settings.setValue("ChatClient/ID", ui->name->text());                                       //
    logThread->terminate();                                                                     // 로그스레드 종료
}


void ChattingForm::closeEvent(QCloseEvent*)
{
    sendProtocol(Chat_LogOut, ui->name->text().toStdString().data());                           // 타입과 이름 전송
    clientSocket->disconnectFromHost();                                                         // 소켓 연결 종료
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();                                                    // 소켓이 연결되지 않은 상태면 잠시동안 기다림
}

void ChattingForm::connectButton_clicked()                                                      /*connectButton 누를 경우 발생하는 슬롯*/
{
    if(ui->connectButton->text() == tr("Log In")) {                                             // connectButton이 Log In으로 써 있을 경우
        clientSocket->connectToHost(ui->serverAddress->text( ),                                 // 소켓은 해당 ip주소와 포트번호를 가지는 서버와 연결
                                    ui->serverPort->text( ).toInt( ));
        clientSocket->waitForConnected();                                                       // 연결을 위해 잠시 기다림
        sendProtocol(Chat_Login, ui->name->text().toStdString().data());                        // 타입과 이름 서버로 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

        ui->connectButton->setText(tr("Chat in"));                                              // connectButton의 글자를 Chat In으로 바꿈
        ui->name->setReadOnly(true);                                                            // 이름 라인에딧은 읽기전용으만 설정
    } else if(ui->connectButton->text() == tr("Chat in"))  {                                    // connectButton의 글자가 Chat In일경우
        sendProtocol(Chat_In, ui->name->text().toStdString().data());                           // 타입과 이름 서버로 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

        ui->connectButton->setText(tr("Chat Out"));                                             // connectButton의 글자를 Chat Out으로 설정
        ui->inputLine->setEnabled(true);                                                        // 채팅 입력칸 활성화
        ui->sentButton->setEnabled(true);                                                       // sendButton 활성화
        ui->fileButton->setEnabled(true);                                                       // fileButton 활성화
    } else if(ui->connectButton->text() == tr("Chat Out"))  {                                   // connectButton의 글자가 Chat Out일 경우
        sendProtocol(Chat_Out, ui->name->text().toStdString().data());                          // 타입과 이름 서버로 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

        ui->connectButton->setText(tr("Chat in"));                                              // connectButtonn의 글자를 Chat In으로 바꿈
        ui->inputLine->setDisabled(true);                                                       // 채팅 입력칸 비활성화
        ui->sentButton->setDisabled(true);                                                      // sentButton 비활성화
        ui->fileButton->setDisabled(true);                                                      // fileButton 비활성화
    }
}

void ChattingForm::receiveData( )                                                               /*recieveData 슬롯*/
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));                           //
    if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);                                      // 소켓으로 부터 읽은 데이터를 바이트어레이에 저장

    Chat_Status type;
    char data[1020];
    memset(data, 0, 1020);                                                                      // 데이터를 저장할 배열을 0으로 초기화

    QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정
    in.device()->seek(0);                                                                       // 처음 위치로 감
    in >> type;                                                                                 // 소켓으로 부터 타입을 읽음
    in.readRawData(data, 1020);                                                                 // 소켓으로 부터 데이터 읽음

    switch(type) {
    case Chat_Talk:                                                                             // 타입이 Chat_Talk이면
        ui->message->append(QString(data));                                                     // 채팅창에 메세지 띄움
        ui->inputLine->setEnabled(true);                                                        // 채팅 입력 칸 활성화
        ui->sentButton->setEnabled(true);                                                       // sentButton 활성화
        ui->fileButton->setEnabled(true);                                                       // fileButton 활성화
        break;
    case Chat_KickOut:                                                                          // 타입이 Chat_KickOut이면
        QMessageBox::critical(this, tr("Chatting Client"),                                      // 메세지 박스로 경고메세지 띄움
                              tr("Kick out from Server"));
        ui->inputLine->setDisabled(true);                                                       // 채팅 입력 칸 비활성화
        ui->sentButton->setDisabled(true);                                                      // sentButton 비활성화
        ui->fileButton->setDisabled(true);                                                      // fileBUtton 비활성화
        ui->name->setReadOnly(false);                                                           // 네임 라인에딧 읽기전용으로 설정 해제
        break;
    case Chat_Invite:                                                                           // 타입이 Chat_Invite일 경우
        QMessageBox::critical(this, tr("Chatting Client"),                                      // 메세지 박스로 경고메세지 띄움
                              tr("Invited from Server"));
        ui->inputLine->setEnabled(true);                                                        // 채팅 입력 칸 활성화
        ui->sentButton->setEnabled(true);                                                       // sentButton 활성화
        ui->fileButton->setEnabled(true);                                                       // fileButton 활성화
        ui->name->setReadOnly(true);                                                            // 네임 라인에딧 읽기전용으로 설정
        break;
    };
}

void ChattingForm::disconnect( )                                                                /*disconnect 슬롯*/
{
    QMessageBox::critical(this, tr("Chatting Client"),                                          // 메세지박스로 경고메세지 띄움
                          tr("Disconnect from Server"));
    ui->inputLine->setEnabled(false);                                                           // 채팅 입력 칸 비활성화
    ui->name->setReadOnly(false);                                                               // 네임 라인에딧 읽기전용으로 설정 해제
    ui->sentButton->setEnabled(false);                                                          // sentButton 비활성화
    ui->connectButton->setText(tr("Log in"));                                                   // connectButton Log in으로 글자 바꿈
}

void ChattingForm::sendProtocol(Chat_Status type, char* data, int size)                         /*sendProtocol 함수*/
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);                                          // 바이트어레이를 쓰기전용으로 설정
    out.device()->seek(0);                                                                      // 처음 위치로 감
    out << type;                                                                                // 타입 씀
    out.writeRawData(data, size);                                                               // 데이터 씀
    clientSocket->write(dataArray);                                                             // 서버로 보냄
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());                                                 // 데이터 보내기 위해 소켓 잠시 기다림
}

void ChattingForm::sendData()                                                                   /*sendData 슬롯*/
{
    QString str = ui->inputLine->text();
    if(str.length()) {                                                                          // 입력한 메세지가 있을 경우
        QByteArray bytearray;
        bytearray = str.toUtf8( );                                                              // 채팅 내용, QString을 char로 바꿈
        ui->message->append("<font color=red>나</font> : " + str);                              // 메세지 창에 표시
        sendProtocol(Chat_Talk, bytearray.data());                                              // sendProtocol 함수를 호출하여 타입과 채팅내용 보냄
    }
}

void ChattingForm::goOnSend(qint64 numBytes)                                                    /*goOnSend 슬롯*/
{
    byteToWrite -= numBytes;                                                                    // 남은 파일 용량으로 보낼 용량을 계속해서 뺌
    outBlock = file->read(qMin(byteToWrite, numBytes));                                         // 보낼 용량만큼 파일을 읽어서 outBlock 변수에 저장
    fileClient->write(outBlock);                                                                // 파싱한 파일을 소켓으로 보냄

    progressDialog->setMaximum(totalSize);                                                      // progressDialog의 맥시멈 값을 totalSize로 설정
    progressDialog->setValue(totalSize-byteToWrite);                                            // progressDialog의 값은 totalSize에서 남은 데이터 수를 뺌

    if (byteToWrite == 0) {                                                                     // 파일전송이 끝난 경우
        qDebug("File sending completed!");
        progressDialog->reset();                                                                // progressDialog 리셋
    }
}

void ChattingForm::sendFile()                                                                   /*sendFile 슬롯*/
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();                                                                           // 남은 파일 용량, 파일 전체 용량, 보낼 파일 0으로 초기화

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);                                                             // 파일생성
        file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0);                                                            // progressDialog 0으로 세팅

        if (!isSent) {                                                                          // 파일을 처음 보내는 경우
            fileClient->connectToHost(ui->serverAddress->text( ),                               // 소켓을 연결하고 isSent를 false에서 true로 바꿈
                                      ui->serverPort->text( ).toInt( ) + 1);
            isSent = true;
        }

        byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량
        loadSize = 1024;

        QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
        out << qint64(0) << qint64(0) << filename << ui->name->text();                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명, 이름 넣음

        totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
        byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함

        out.device()->seek(0);                                                                  // 처음 위치로 감
        out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀

        fileClient->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄

        progressDialog->setMaximum(totalSize);                                                  // progressDialog 최대값 totalSize로 설정
        progressDialog->setValue(totalSize-byteToWrite);                                        // progressDialog 값 totalSize에서 남은 용량을 뺀 값으로 설정
        progressDialog->show();                                                                 // progressDialog 표시
    }
    qDebug() << QString("Sending file %1").arg(filename);
}
