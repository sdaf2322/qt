#include "mainserver.h"
#include "ui_mainserver.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

MainServer::MainServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainServer)
{
    ui->setupUi(this);

    chatServer = new QTcpServer(this);                                                 // 채팅을 위한 TCP서버 생성
    connect(chatServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));             // 서버연결이 되면 clientConnect 슬롯 발생
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"),\
                              tr("Unable to start the server: %1.")\
                              .arg(chatServer->errorString( )));                       // ip주소와 포트번호를 통한 서버 연결에 실패할 경우 에러메세지 띄움
        close( );
        return;
    }

    fileServer = new QTcpServer(this);                                                 // 파일 전송을 위한 TCP서버 생성
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));            // 서버연결이 되면 acceptConnection 슬롯 발생
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));                       // ip주소와 포트번호를 통한 서버 연결에 실패하면 에러메세지 띄움
        close( );
        return;
    }
    qDebug("Start listening ...");
}

MainServer::~MainServer()
{
    delete ui;

    chatServer->close( );
    fileServer->close( );                                                               // 채팅서버와 파일 서버 닫음
}

void MainServer::clientConnect()
{
    QTcpSocket* clientConnection = chatServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(receiveData()));
    qDebug("new connection is established....");
}

void MainServer::receiveData()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray;
    Patient_Info type;
    char data[11][1020];

    for(int i = 0; i < 11; i++)
    {
        if(i == 0)
        {
            bytearray = clientConnection->read(BLOCK_SIZE);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정

            in.device()->seek(0);                                                                       // 처음 위치로 감
            in >> type;                                                                                 // 소켓으로 부터 타입을 읽음
            in.readRawData(data[i], 1020);
        }
        else
        {
            bytearray = clientConnection->read(BLOCK_SIZE - 4);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정
            in.readRawData(data[i], 1020);
        }
    }

    switch(type){
    case Send_Info:
        socketHash[type] = clientConnection;
        // 데이터베이스에 저장
        break;
    case Request_Info:
        socketHash[type] = clientConnection;
        // 데이터베이스에서 정보 가져옴

        QByteArray sendArray;
        sendArray.clear();
        QDataStream out(&sendArray, QIODevice::WriteOnly);                                       // 바이트어레이를 쓰기모드로 설정
        out << Get_Info;                                                                        // 보낼 타입을 chat_talk로 씀
        sendArray.append(patientName.toStdString().data());
        sendArray.append(patientBirthday.toStdString().data());
        sendArray.append(patientChartNumber.toStdString().data());
        sendArray.append(patientGender.toStdString().data());
        sendArray.append(patientFirstVisitdate.toStdString().data());
        sendArray.append(patientLastVisitdate.toStdString().data());
        sendArray.append(patientMobile.toStdString().data());
        sendArray.append(patientPhone.toStdString().data());
        sendArray.append(patientEmail.toStdString().data());
        sendArray.append(patientAddress.toStdString().data());
        sendArray.append(patientDetailAddress.toStdString().data());
        clientConnection->write(sendArray);                                                                  // 소켓으로 데이터 보냄
        qDebug() << clientConnection->peerPort();
        break;
    }
}

// 파일전송
void MainServer::acceptConnection()
{
    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
    qDebug("file connection is established....");
}

//void MainServer::readClient()
//{
//    qDebug("Receiving file ...");
//    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
//    QString filename, name;

//    if (byteReceived == 0) {
//        progressDialog->reset();
//        progressDialog->show();

//        QString ip = receivedSocket->peerAddress().toString();
//        quint16 port = receivedSocket->peerPort();
//        qDebug() << ip << " : " << port;

//        QDataStream in(receivedSocket);
//        in >> totalSize >> byteReceived >> filename >> name;
//        progressDialog->setMaximum(totalSize);

//        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
//        item->setText(0, ip);
//        item->setText(1, QString::number(port));
//        item->setText(2, QString::number(clientIDHash[name]));
//        item->setText(3, name);
//        item->setText(4, filename);
//        item->setText(5, QDateTime::currentDateTime().toString());
//        item->setToolTip(4, filename);

//        /* 컨텐츠의 길이로 QTreeWidget의 헤더의 크기를 고정 */
//        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
//            ui->messageTreeWidget->resizeColumnToContents(i);

//        ui->messageTreeWidget->addTopLevelItem(item);
//        logThread->appendData(item);

//        QFileInfo info(filename);
//        QString currentFileName = info.fileName();
//        file = new QFile(currentFileName);
//        file->open(QFile::WriteOnly);
//    } else {
//        inBlock = receivedSocket->readAll();

//        byteReceived += inBlock.size();
//        file->write(inBlock);
//        file->flush();
//    }

//    progressDialog->setValue(byteReceived);

//    if (byteReceived == totalSize) {
//        qDebug() << QString("%1 receive completed").arg(filename);

//        inBlock.clear();
//        byteReceived = 0;
//        totalSize = 0;
//        progressDialog->reset();
//        progressDialog->hide();

//        file->close();
//        delete file;
//    }
//}

//void Doctor::goOnSend(qint64 numBytes)
//{
//    byteToWrite -= numBytes;
//    outBlock = file->read(qMin(byteToWrite, numBytes));
//    fileClient->write(outBlock);

//    progressDialog->setMaximum(totalSize);
//    progressDialog->setValue(totalSize-byteToWrite);

//    if (byteToWrite == 0) {
//        qDebug("File sending completed!");
//        progressDialog->reset();
//    }
//}

//void Doctor::sendFile()
//{
//    loadSize = 0;
//    byteToWrite = 0;
//    totalSize = 0;
//    outBlock.clear();

//    QString filename = QFileDialog::getOpenFileName(this);
//    if(filename.length()) {
//        file = new QFile(filename);
//        file->open(QFile::ReadOnly);

//        qDebug() << QString("file %1 is opened").arg(filename);
//        progressDialog->setValue(0);

//        if (!isSent) {
//            fileClient->connectToHost(ui->serverAddress->text( ),
//                                      ui->serverPort->text( ).toInt( ) + 1);
//            isSent = true;
//        }

//        byteToWrite = totalSize = file->size();
//        loadSize = 1024;

//        QDataStream out(&outBlock, QIODevice::WriteOnly);
//        out << qint64(0) << qint64(0) << filename << ui->name->text();

//        totalSize += outBlock.size();
//        byteToWrite += outBlock.size();

//        out.device()->seek(0);
//        out << totalSize << qint64(outBlock.size());

//        fileClient->write(outBlock);

//        progressDialog->setMaximum(totalSize);
//        progressDialog->setValue(totalSize-byteToWrite);
//        progressDialog->show();
//    }
//    qDebug() << QString("Sending file %1").arg(filename);
//}
