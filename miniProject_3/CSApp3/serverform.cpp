#include "serverform.h"
#include "ui_serverform.h"
#include "logthread.h"

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

ServerForm::ServerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 540 << 200;                                                                        // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    chatServer = new QTcpServer(this);                                                          // 채팅을 위한 TCP 서버 생성
    connect(chatServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));                      // 연결이 되면 clientConnect 슬롯 발생
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString( )));                                // ip주소와 포트번호가 다르다면 에러메세지 띄우고 리턴
        close( );
        return;
    }

    fileServer = new QTcpServer(this);                                                          // 파일 전송을 위한 TCP 서버 생성
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));                     // 연결이 되면 acceptConnection 슬롯 발생
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));                                // ip주소와 포트번호+1과 다르다면 에러메세지 띄우고 리턴
        close( );
        return;
    }

    qDebug("Start listening ...");

    QAction* inviteAction = new QAction(tr("&Invite"));                                         // 초대액션 생성
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), this, SLOT(inviteClient()));                           // 초대액션 누를경우 inviteClinet 슬롯 발생

    QAction* removeAction = new QAction(tr("&Kick out"));                                       // 강퇴액션 생성
    connect(removeAction, SIGNAL(triggered()), this, SLOT(kickOut()));                                // 강퇴액션 누를경우 kickOut 슬롯 발생

    menu = new QMenu;                                                                           // 메뉴 생성
    menu->addAction(inviteAction);
    menu->addAction(removeAction);                                                              // 메뉴에 초대, 강퇴 액션 추가
    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);                          // 마우스 정책설정

    progressDialog = new QProgressDialog(0);                                                    // 파일전송시 사용되는 다이얼 생성
    progressDialog->setAutoClose(true);                                                         // 파일전송이 끝나면 자동으로 종료되도록 설정
    progressDialog->reset();                                                                    // 파일전송이 끝나면 리셋

    logThread = new LogThread(this);                                                            // 로그스레드 생성
    logThread->start();                                                                         // 로그시작

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));                // 세이브 버튼 누르면 저장

    qDebug() << tr("The server is running on port %1.").arg(chatServer->serverPort( ));
}

ServerForm::~ServerForm()                                                                       /*ServerForm 소멸자*/
{
    delete ui;

    logThread->terminate();                                                                     // 로그 종료
    chatServer->close( );                                                                       // 채팅서버 종료
    fileServer->close( );                                                                       // 파일서버 종료
}

void ServerForm::clientConnect( )                                                               /*clientConnect 슬롯*/
{
    QTcpSocket *clientConnection = chatServer->nextPendingConnection( );                        // 서버로부터 소켓 받음
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(receiveData( )));                      // 소켓에서 읽을 준비가 되면 receiveData 슬롯 발생
    connect(clientConnection, SIGNAL(disconnected( )), SLOT(removeClient()));                   // 소켓에서 연결이 끊기면 removeClient 슬롯 발생
    qDebug("new connection is established...");
}

void ServerForm::receiveData( )                                                                 /*receiveData 슬롯*/
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));                       // 소켓 받아옴
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);                                  // 소켓으로부터 받은 데이터를 바이트어레이에 저장

    Chat_Status type;                                                                           // 타입
    char data[1020];                                                                            // 데이터
    memset(data, 0, 1020);                                                                      // 1020만큼 데이터 0으로 초기화

    QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 데이터를 읽기전용으로 설정
    in.device()->seek(0);                                                                       // 처음 위치로 감
    in >> type;                                                                                 // 타입 입력
    in.readRawData(data, 1020);                                                                 // 데이터 읽음

    QString ip = clientConnection->peerAddress().toString();                                    // 현재 ip주소
    quint16 port = clientConnection->peerPort();                                                // 현재 포트번호
    QString name = QString::fromStdString(data);                                                // 받은 데이터를 변수에 저장

    qDebug() << ip << " : " << type;

    switch(type) {
    case Chat_Login:                                                                                    /*타입이 Chat_Login이면*/
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {            // clientTreeWidget에서 데이터(이름)와 일치하는 항목을 찾음
            if(item->text(0) != "-") {                                                                  // 항목의 상태가 '-'이 아니면 '-'로 바꿈
                item->setText(0, "-");
            }
            clientSocketHash[name] = clientConnection;                                                  // 해쉬에 소켓 저장
        }
        break;
    case Chat_In:                                                                                       /*타입이 Chat_In이면*/
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {            // clientTreeWidget에서 데이터(이름)와 일치하는 항목을 찾음
            if(item->text(0) != "O") {                                                                  // 항목의 상태가 'O'가 아니면 'O'로 바꿈
                item->setText(0, "O");
            }
            clientNameHash[port] = name;                                                                // 해쉬에 데이터(이름) 저장
            if(clientSocketHash.contains(name))
                clientSocketHash[name] = clientConnection;
        }
        break;
    case Chat_Talk: {                                                                                   /*타입이 Chat_Talk이면*/
        foreach(QTcpSocket *sock, clientSocketHash.values()) {                                          // 소켓해쉬에 저장된 소켓들을 불러옴
            qDebug() << sock->peerPort();
            if(clientNameHash.contains(sock->peerPort()) && port != sock->peerPort()) {
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);                                       // 바이트어레이를 쓰기모드로 설정
                out << Chat_Talk;                                                                        // 보낼 타입을 chat_talk로 씀
                sendArray.append("<font color=lightsteelblue>");
                sendArray.append(clientNameHash[port].toStdString().data());                             // 바이트어레이에 이름 추가
                sendArray.append("</font> : ");
                sendArray.append(name.toStdString().data());                                             // 바이트어레이에 채팅내용 추가
                sock->write(sendArray);                                                                  // 소켓으로 데이터 보냄
                qDebug() << sock->peerPort();
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);                                                                            // messageTreeWidget에 ip주소 표시
        item->setText(1, QString::number(port));                                                         // messageTreeWidget에 포트번호 표시
        item->setText(2, QString::number(clientIDHash[clientNameHash[port]]));                           // messageTreeWidget에 아이디 표시
        item->setText(3, clientNameHash[port]);                                                          // messageTreeWidget에 이름 표시
        item->setText(4, QString(data));                                                                 // messageTreeWidget에 채팅내용 표시
        item->setText(5, QDateTime::currentDateTime().toString());                                       // messageTreeWidget에 현재시간 표시
        item->setToolTip(4, QString(data));                                                              // 툴팁에 채팅내용 표시
        ui->messageTreeWidget->addTopLevelItem(item);                                                    // messageTreeWidget에 올림

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);                                            // 데이터의 크기에 따라 열 크기 조절

        logThread->appendData(item);                                                                     // 로그로 저장할 데이터 넣음
    }
        break;
    case Chat_Out:                                                                                       /*타입이 Chat_Out이면*/
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {                // clientTreeWidget에서 이름과 일치하는 항목 찾음
            if(item->text(0) != "-") {
                item->setText(0, "-");                                                                   // 상태가 '-'이 아니면 '-'로 바꿈
            }
            clientNameHash.remove(port);                                                                 // 네임해쉬에서 제거
        }
        break;
    case Chat_LogOut:                                                                                    /*타입이 Chat_LogOut이면*/
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {                // clientTreeWidget에서 이름과 일치하는 항목 찾음
            if(item->text(0) != "X") {
                item->setText(0, "X");                                                                   // 상태가 'X'가 아니라면 'X'로 바꿈
            }
            clientSocketHash.remove(name);
        }
        break;
    }
}

void ServerForm::removeClient()                                                                          /*removeClient 슬롯*/
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));                                // 소켓 받아옴
    if(clientConnection != nullptr) {
        QString name = clientNameHash[clientConnection->peerPort()];
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {                // clientTreeWidget에서 이름과 일치하는 항목 찾음
            item->setText(0, "X");                                                                       // 상태 'X'로 바꿈
        }
        clientSocketHash.remove(name);                                                                   // 소켓해쉬에서 소켓 제거
        clientConnection->deleteLater();                                                                 // 소켓 제거
    }
}

void ServerForm::addClient(int id, QString name)                                                         /*addClient 슬롯*/
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientTreeWidget);
    item->setText(0, "X");                                                                               // clientTreeWidget에 상태 'X'로 설정
    item->setText(1, name);                                                                              // clientTreeWidget에 이름 추가
    ui->clientTreeWidget->addTopLevelItem(item);                                                         // clientTreeWidget에 추가
    clientIDHash[name] = id;                                                                             // id해쉬에 id 추가
    ui->clientTreeWidget->resizeColumnToContents(0);                                                     // 데이터 길이에 따라 열 크기 조절
}

void ServerForm::on_deletePushButton_clicked()                                                           /*삭제버튼 누를 경우*/
{
    ui->messageTreeWidget->clear();                                                                      // messageTreeWidget 비움
}

void ServerForm::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)                       /*마우스 우측키 눌렀을 경우*/
{
    foreach(QAction *action, menu->actions()) {
        if(action->objectName() == "Invite")
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) != "O");                     // 상태가 'O'가 아니면 초대액션 활성화
        else
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == "O");                     // 상태가 'O'이면 강퇴액션 활성화
    }
    QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);                                           // 전역좌표로 설정
    menu->exec(globalPos);                                                                               // 메뉴창 띄움
}

void ServerForm::kickOut()                                                                               /*강퇴 슬롯*/
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);                                                   // 바이트어레이 쓰기모드로 설정
    out << Chat_KickOut;                                                                                 // 보낼 타입 Chat_KickOut으로 설정
    out.writeRawData("", 1020);                                                                          // 보낼 데이터 공백으로 채움

    QString name = ui->clientTreeWidget->currentItem()->text(1);                                         // 이름
    QTcpSocket* sock = clientSocketHash[name];                                                           // 이름에 해당하는 소켓
    sock->write(sendArray);                                                                              // 소켓으로 데이터 보냄

    ui->clientTreeWidget->currentItem()->setText(0, "-");
}

void ServerForm::inviteClient()                                                                          /*초대 슬롯*/
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);                                                   // 바이트어레이 쓰기모드로 설정
    out << Chat_Invite;                                                                                  // 보낼 타입 Chat_Invite로 설정
    out.writeRawData("", 1020);                                                                          // 보낼 데이터 공백으로 채움

    QString name = ui->clientTreeWidget->currentItem()->text(1);                                         // 이름
    QTcpSocket* sock = clientSocketHash[name];                                                           // 이름에 해당하는 소켓
    sock->write(sendArray);                                                                              // 소켓으로 데이터 보냄

    ui->clientTreeWidget->currentItem()->setText(0, "O");
}

void ServerForm::acceptConnection()                                                                      /*파일전송을 위한 슬롯*/
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();                                    // 파일서버와 연결된 소켓 받아옴
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));                              // 읽을 준비가 되면 readClient 슬롯 발생
}

void ServerForm::readClient()                                                                            /*readClient 슬롯*/
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));                                  // 소켓 받아옴
    QString filename, name;

    if (byteReceived == 0) {                                                                             // 파일전송 시작할 경우
        progressDialog->reset();
        progressDialog->show();                                                                          // progress다이얼로그 띄움

        QString ip = receivedSocket->peerAddress().toString();                                           // 현재 ip주소
        quint16 port = receivedSocket->peerPort();                                                       // 현재 포트번호
        qDebug() << ip << " : " << port;

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;                                             // 파일 전체크기, 받은 데이터, 파일명, 이름 받음
        progressDialog->setMaximum(totalSize);                                                           // 다이얼로그의 전체 크기를 파일 전체 크기로 설정

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);                                                                            // messageTreeWidget에 ip주소 표시
        item->setText(1, QString::number(port));                                                         // messageTreeWidget에 포트번호 표시
        item->setText(2, QString::number(clientIDHash[name]));                                           // messageTreeWidget에 id 표시
        item->setText(3, name);                                                                          // messageTreeWidget에 이름 표시
        item->setText(4, filename);                                                                      // messageTreeWidget에 파일명 표시
        item->setText(5, QDateTime::currentDateTime().toString());                                       // messageTreeWidget에 현재시간 표시
        item->setToolTip(4, filename);                                                                   // 툴팁에 파일명 표시

        /* 컨텐츠의 길이로 QTreeWidget의 헤더의 크기를 고정 */
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);                                            // 데이터 내용에 맞게 열 크기 조절

        ui->messageTreeWidget->addTopLevelItem(item);                                                    // messageTreeWidget에 추가
        logThread->appendData(item);                                                                     // 로그로 저장할 데이터 삽입

        QFileInfo info(filename);                                                                        // 파일정보
        QString currentFileName = info.fileName();                                                       // 파일이름
        file = new QFile(currentFileName);                                                               // 파일 생성
        file->open(QFile::WriteOnly);                                                                    // 파일 쓰기모드로 설정
    } else {
        inBlock = receivedSocket->readAll();                                                             // 소켓으로부터 모두 읽음

        byteReceived += inBlock.size();                                                                  // 지금까지 받은 데이터 크기에 방금 받은 데이터 크기 더함
        file->write(inBlock);                                                                            // 받은 데이터를 파일에 저장
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {                                                                     // 파일 전송을 완료한 경우
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();                                                                                 // 받은 데이터 저장소 비움
        byteReceived = 0;                                                                                // 누적 받은 데이터 크기 0으로 설정
        totalSize = 0;                                                                                   // 전체크기 0으로 설정
        progressDialog->reset();                                                                         // 다이얼로그 리셋
        progressDialog->hide();                                                                          // 다이얼로그 숨김

        file->close();                                                                                   // 파일 닫음
        delete file;
    }
}
