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
    ui->name->setPlaceholderText("Input Client Name");

    //serverAddress = new QLineEdit(this);
    ui->serverAddress->setText("127.0.0.1");
    ui->serverAddress->setPlaceholderText("Input IP Number");
    //serverAddress->setInputMask("999.999.999.999;_");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    ui->serverAddress->setPlaceholderText("Server IP Address");
    ui->serverAddress->setValidator(&validator);

    //ui->serverPort = new QLineEdit(this);
    ui->serverPort->setText(QString::number(PORT_NUMBER));
    ui->serverPort->setInputMask("00000;_");
    ui->serverPort->setPlaceholderText("Server Port No");

    ui->message->setReadOnly(true);

    ui->inputLine->setPlaceholderText("Input Chatting");
    connect(ui->inputLine, SIGNAL(returnPressed( )), SLOT(sendData( )));                        // 엔터 치면 센드데이터 슬롯
    connect(ui->inputLine, SIGNAL(returnPressed( )), ui->inputLine, SLOT(clear( )));            // 엔터 치면 인풋라인 클리어
    connect(ui->sentButton, SIGNAL(clicked( )), SLOT(sendData( )));
    connect(ui->sentButton, SIGNAL(clicked( )), ui->inputLine, SLOT(clear( ))); 
    ui->inputLine->setDisabled(true);
    ui->sentButton->setDisabled(true);

    connect(ui->connectButton, SIGNAL(clicked()), SLOT(connectButton_clicked()));

    connect(ui->fileButton, SIGNAL(clicked( )), SLOT(sendFile( )));
    ui->fileButton->setDisabled(true);

    connect(ui->quitButton, SIGNAL(clicked( )), qApp, SLOT(quit( )));

    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << clientSocket->errorString( ); });                                     // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                              // 읽을 준비가 되면 리시브 데이터 슬롯
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                            // 연결 종료시 디스커넥트 슬롯

    /* 파일 전송을 위한 소켓 */
    fileClient = new QTcpSocket(this);
    connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    setWindowTitle(tr("Chat Client"));

    logThread = new LogThread(this);
    logThread->start();                                                                         // 로그시작

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));                // 세이브 버튼 누르면 저장
}

ChattingForm::~ChattingForm( )
{
    clientSocket->close( );
    QSettings settings("ChatClient", "Chat Client");
    settings.setValue("ChatClient/ID", ui->name->text());
    logThread->terminate();
}

/* 창이 닫힐 때 서버에 연결 접속 메시지를 보내고 종료 */
void ChattingForm::closeEvent(QCloseEvent*)
{
    sendProtocol(Chat_LogOut, ui->name->text().toStdString().data());           // 타입과 이름 전송
    clientSocket->disconnectFromHost();
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();
}

void ChattingForm::connectButton_clicked()
{
    if(ui->connectButton->text() == tr("Log In")) {
        clientSocket->connectToHost(ui->serverAddress->text( ),
                                    ui->serverPort->text( ).toInt( ));
        clientSocket->waitForConnected();
        sendProtocol(Chat_Login, ui->name->text().toStdString().data());    // 타입과 이름 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);

        ui->connectButton->setText(tr("Chat in"));
        ui->name->setReadOnly(true);
    } else if(ui->connectButton->text() == tr("Chat in"))  {
        sendProtocol(Chat_In, ui->name->text().toStdString().data());       // 타입과 이름 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);

        ui->connectButton->setText(tr("Chat Out"));
        ui->inputLine->setEnabled(true);
        ui->sentButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
    } else if(ui->connectButton->text() == tr("Chat Out"))  {
        sendProtocol(Chat_Out, ui->name->text().toStdString().data());      // 타입과 이름 전송

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);
        item->setText(0, ui->name->text());
        item->setText(1, ui->connectButton->text());
        item->setText(2, QDateTime::currentDateTime().toString());
        logThread->appendData(item);

        ui->connectButton->setText(tr("Chat in"));
        ui->inputLine->setDisabled(true);
        ui->sentButton->setDisabled(true);
        ui->fileButton->setDisabled(true);
    }
}

/* 데이터를 받을 때 */
void ChattingForm::receiveData( )
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));
    if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);

    Chat_Status type;       // 채팅의 목적
    char data[1020];        // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);                                                 // 데이터 읽음

    switch(type) {
    case Chat_Talk:
        ui->message->append(QString(data));
        ui->inputLine->setEnabled(true);
        ui->sentButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        break;
    case Chat_KickOut:      // 강퇴면
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Kick out from Server"));
        ui->inputLine->setDisabled(true);       // 버튼의 상태 변경
        ui->sentButton->setDisabled(true);
        ui->fileButton->setDisabled(true);
        ui->name->setReadOnly(false);           // 메시지 입력 불가
        break;
    case Chat_Invite:       // 초대면
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Invited from Server"));
        ui->inputLine->setEnabled(true);
        ui->sentButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        ui->name->setReadOnly(true);            // 메시지 입력 가능
        break;
    };
}

/* 연결이 끊어졌을 때 : 상태 변경 */
void ChattingForm::disconnect( )
{
    QMessageBox::critical(this, tr("Chatting Client"), \
                          tr("Disconnect from Server"));
    ui->inputLine->setEnabled(false);
    ui->name->setReadOnly(false);
    ui->sentButton->setEnabled(false);
    ui->connectButton->setText(tr("Log in"));
}

/* 프로토콜을 생성해서 서버로 전송 */
void ChattingForm::sendProtocol(Chat_Status type, char* data, int size)
{
    QByteArray dataArray;           // 소켓으로 보낼 데이터를 채우고
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(dataArray);     // 서버로 전송
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

/* 메시지 보내기 */
void ChattingForm::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length()) {
        QByteArray bytearray;              // 인풋라인의 메세지가 들어가 있음
        bytearray = str.toUtf8( );
        /* 화면에 표시 : 앞에 '나'라고 추가 */
        ui->message->append("<font color=red>나</font> : " + str);
        sendProtocol(Chat_Talk, bytearray.data());
    }
}

/* 파일 전송시 여러번 나눠서 전송 */
void ChattingForm::goOnSend(qint64 numBytes) // Start sending file content
{
    byteToWrite -= numBytes; // Remaining data size
    outBlock = file->read(qMin(byteToWrite, numBytes));
    fileClient->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);

    if (byteToWrite == 0) { // Send completed
        qDebug("File sending completed!");
        progressDialog->reset();
    }
}

/* 파일 보내기 */
void ChattingForm::sendFile() // Open the file and get the file name (including path)
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0); // Not sent for the first time

        if (!isSent) { // Only the first time it is sent, it happens when the connection generates the signal connect
            fileClient->connectToHost(ui->serverAddress->text( ),
                                      ui->serverPort->text( ).toInt( ) + 1);
            isSent = true;
        }

        // When sending for the first time, connectToHost initiates the connect signal to call send, and you need to call send after the second time

        byteToWrite = totalSize = file->size(); // The size of the remaining data
        loadSize = 1024; // The size of data sent each time

        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out << qint64(0) << qint64(0) << filename << ui->name->text();

        totalSize += outBlock.size();
        byteToWrite += outBlock.size();

        out.device()->seek(0);
        out << totalSize << qint64(outBlock.size());

        fileClient->write(outBlock); // Send the read file to the socket

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
    qDebug() << QString("Sending file %1").arg(filename);
}
