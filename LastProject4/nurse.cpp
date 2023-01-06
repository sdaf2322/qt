#include "nurse.h"
//#include "logthread.h"
#include "ui_nurse.h"

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
#include <QProgressDialog>

#define BLOCK_SIZE      1024

Nurse::Nurse(QWidget *parent)
    : QWidget(parent), isSent(false),
    ui(new Ui::Nurse) {
    ui->setupUi(this);

//------------------------------------------------------------ 데이터 소켓 생성 --------------------------------------------------------------//
    clientSocket = new QTcpSocket(this);                                                        // 채팅을 위한 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << clientSocket->errorString( ); });                                 // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                        // 연결 종료시 disconnect 슬롯

//------------------------------------------------------------ 시그널  --------------------------------------------------------------//
    connect(ui->sendButton, SIGNAL(clicked( )), SLOT(sendData( )));
    connect(ui->Patient_Gender_Male_pushButton, SIGNAL(clicked()), SLOT(Patlent_Gender_Male_pushButton_clicked()));
    connect(ui->Patient_Gender_Female_pushButton, SIGNAL(clicked()), SLOT(Patlent_Gender_Female_pushButton_clicked()));
    connect(ui->Patient_Gender_None_pushButton, SIGNAL(clicked()), SLOT(Patlent_Gender_None_pushButton_clicked()));
}

//----------------------------------------------------------------- 생성자 끝 ---------------------------------------------------------------//

Nurse::~Nurse( )                                                                  /*ChattingForm 소멸자*/
{
    clientSocket->close( );                                                                     // 소켓 닫음
//    QSettings settings("ChatClient", "Chat Client");
//    settings.setValue("ChatClient/ID", ui->name->text());
}


void Nurse::closeEvent(QCloseEvent*)
{
    clientSocket->disconnectFromHost();                                                         // 소켓 연결 종료
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();                                                    // 소켓이 연결되지 않은 상태면 잠시동안 기다림
}

void Nurse::receiveData( )                                                               // 채팅자 끼리 채팅 주고 받기
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));                           //
    //if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray;                                                                 // 소켓으로 부터 읽은 데이터를 바이트어레이에 저장
    Patient_Info type;
    char data[11][1020];

    for(int i = 0; i < 11; i++)
    {
        if(i == 0)
        {
            bytearray = clientSocket->read(BLOCK_SIZE);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정

            in.device()->seek(0);                                                                       // 처음 위치로 감
            in >> type;                                                                                 // 소켓으로 부터 타입을 읽음
            in.readRawData(data[i], 1020);
        }
        else
        {
            bytearray = clientSocket->read(BLOCK_SIZE - 4);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정
            in.readRawData(data[i], 1020);
        }
    }

    if(type == Get_Info)
    {
        ui->Patient_Name_lineEdit->setText(data[0]);
        ui->Patient_Birthdat_lineEdit->setText(data[1]);
        ui->Patient_ChartNumber_lineEdit->setText(data[2]);

        ui->Patient_Birthdat_lineEdit->setText(data[3]);                         // --------------------   성별 버튼

        ui->Patient_FirstVist_dateEdit->setDisplayFormat(data[4]);
        ui->Patient_LastVist_dateEdit->setDisplayFormat(data[5]);
        ui->Patient_Mobile_lineEdit->setText(data[6]);
        ui->Patient_Phone_lineEdit->setText(data[7]);
        ui->Patient_Email_lineEdit->setText(data[8]);
        ui->Patient_Address_lineEdit->setText(data[9]);
        ui->Patient_DetailAddress_lineEdit->setText(data[10]);
    }
}

//---------------------------------- 데이터 전송 ----------------------------------------------------------------------------//
void Nurse::sendProtocol(Patient_Info type, QByteArray* bytearray, int size)                         /*sendProtocol 함수*/
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);                                          // 바이트어레이를 쓰기전용으로 설정
    out.device()->seek(0);                                                                      // 처음 위치로 감
    out << type;                                                                                // 타입 씀
//    out << byteArray[0];
//    out << byteArray[1];
    for(int i = 0; i < 11; i++)
    {
        out.writeRawData(bytearray[i].data(), size);                                                               // 데이터 씀
    }
    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());                                                 // 데이터 보내기 위해 소켓 잠시 기다림
}

void Nurse::sendData()                                                                   /*sendData 슬롯*/
{
    QString str[11];
    //QTextStream str1;
    str[0] = ui->Patient_Name_lineEdit ->text();
    str[1] = ui->Patient_Birthdat_lineEdit->text();
    str[2] = ui->Patient_ChartNumber_lineEdit->text();
    str[3] = Gender;
    str[4] = ui->Patient_FirstVist_dateEdit->text();
    str[5] = ui->Patient_LastVist_dateEdit->text();
    str[6] = ui->Patient_Mobile_lineEdit->text();
    str[7] = ui->Patient_Phone_lineEdit->text();
    str[8] = ui->Patient_Email_lineEdit ->text();
    str[9] = ui->Patient_Address_lineEdit->text();
    str[10] = ui->Patient_DetailAddress_lineEdit->text();

    //str1 << str[0] << str[1] << str[2] << str[3] << str[4] << str[5] << str[6] << str[7] << str[8] << str[9] << str[10];
    if(str[0].length() || str[1].length() || str[2].length() || str[3].length() || str[4].length() || str[5].length() || str[6].length() || str[7].length()
            || str[8].length() || str[9].length() || str[10].length()) {                                                                          // 입력한 메세지가 있을 경우
        QByteArray bytearray[11];
        for(int i = 0; i < 11; i++)
        {
            bytearray[i] = str[i].toUtf8();
        }
        sendProtocol(Send_Info, bytearray);
    }
}

void Nurse::Patient_Gender_Male_pushButton_clicked()
{
    Gender = ui->Patient_Gender_Male_pushButton->text();
}
void Nurse::Patient_Gender_Female_pushButton_clicked()
{
    Gender = ui->Patient_Gender_Female_pushButton->text();
}
void Nurse::Patient_Gender_None_pushButton_clicked()
{
    Gender = ui->Patient_Gender_None_pushButton->text();
}


//void Nurse::connectButton_clicked()                                                      /*connectButton 누를 경우 발생하는 슬롯*/
//{
//    if(ui->connectButton->text() == tr("Log In")) {                                             // connectButton이 Log In으로 써 있을 경우
//        clientSocket->connectToHost(ui->serverAddress->text( ),                                 // 소켓은 해당 ip주소와 포트번호를 가지는 서버와 연결
//                                    ui->serverPort->text( ).toInt( ));
//        clientSocket->waitForConnected();                                                       // 연결을 위해 잠시 기다림
//        sendProtocol(Chat_Login, ui->name->text().toStdString().data());                        // 타입과 이름 서버로 전송

//        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
//        item->setText(0, ui->name->text());
//        item->setText(1, ui->connectButton->text());
//        item->setText(2, QDateTime::currentDateTime().toString());
//        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

//        ui->connectButton->setText(tr("Chat in"));                                              // connectButton의 글자를 Chat In으로 바꿈
//        ui->name->setReadOnly(true);                                                            // 이름 라인에딧은 읽기전용으만 설정
//    } else if(ui->connectButton->text() == tr("Chat in"))  {                                    // connectButton의 글자가 Chat In일경우
//        sendProtocol(Chat_In, ui->name->text().toStdString().data());                           // 타입과 이름 서버로 전송

//        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
//        item->setText(0, ui->name->text());
//        item->setText(1, ui->connectButton->text());
//        item->setText(2, QDateTime::currentDateTime().toString());
//        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

//        ui->connectButton->setText(tr("Chat Out"));                                             // connectButton의 글자를 Chat Out으로 설정
//        ui->inputLine->setEnabled(true);                                                        // 채팅 입력칸 활성화
//        ui->sentButton->setEnabled(true);                                                       // sendButton 활성화
//    } else if(ui->connectButton->text() == tr("Chat Out"))  {                                   // connectButton의 글자가 Chat Out일 경우
//        sendProtocol(Chat_Out, ui->name->text().toStdString().data());                          // 타입과 이름 서버로 전송

//        QTreeWidgetItem* item = new QTreeWidgetItem(ui->logTreeWidget);                         // 로그를 표시할 위젯 생성
//        item->setText(0, ui->name->text());
//        item->setText(1, ui->connectButton->text());
//        item->setText(2, QDateTime::currentDateTime().toString());
//        logThread->appendData(item);                                                            // 이름, 로그인 상태, 현재 시간을 로그로 저장

//        ui->connectButton->setText(tr("Chat in"));                                              // connectButtonn의 글자를 Chat In으로 바꿈
//        ui->inputLine->setDisabled(true);                                                       // 채팅 입력칸 비활성화
//        ui->sentButton->setDisabled(true);                                                      // sentButton 비활성화
//    }
//}
