#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
//#include "clientitem.h"

#include <QFile>
#include <QMenu>

ClientManagerForm::ClientManagerForm(QWidget *parent) :                                                         /*ClientManagerForm 생성자*/
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);
    ui->idLineEdit->setDisabled(true);
    ui->phoneNumberLineEdit->setInputMask("000-0000-0000");                                                     // 전화번호 입력 칸 마스크 설정
    QList<int> sizes;
    sizes << 540 << 400;                                                                                        // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                                                         // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));                                             // 종료액션 누르면 항목 삭제

    menu = new QMenu;                                                                                           // 메뉴 생성
    menu->addAction(removeAction);                                                                              // 메뉴 만들어서 종료액션 추가
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);                                                 // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));          // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸

    connect(ui->searchLineEdit, SIGNAL(returnPressed()), SLOT(on_searchPushButton_clicked()));
    // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴
    searchModel = new QStandardItemModel(0, 4);                                                                 // 테이블 모델의 내용을 저장하기 위해 만든 모델
    header << "ID" << "Name" << "PhoneNumber" << "Address";
    searchModel->setHorizontalHeaderLabels(header);                                                             // 테이블의 헤더 설정
    ui->searchTableView->setModel(searchModel);
}

void ClientManagerForm::LoadData()                                                                              /*loadData 함수*/
{
    QModelIndex index;
    int i = 0;
    createConnection();                                                                                         // 데이터베이스 생성
    model = new QSqlTableModel(this, db);                                                                       // 사용할 데이터베이스 선택
    model->setTable("clientManager");                                                                           // 테이블 생성
    ui->tableView->setModel(model);                                                                             // 테이블 뷰로 출력할 데이터베이스 선택
    model->select();                                                                                            // 데이터베이스 출력
    QSqlQuery query("SELECT * FROM clientManager");                                                             // 쿼리문으로 테이블 전체 내용을 불러옴

    while(i < model->rowCount())                                                                                // 테이블 내용을 끝까지 읽음
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString name = model->data(model->index(i, 1)).toString();
        QString phoneNumber = model->data(model->index(i, 2)).toString();
        QString address = model->data(model->index(i, 3)).toString();                                           // 테이블에 들어있는 각 항목의 정보들을 받아옴
        emit clientAdded(id, name);                                                                             // 이름에 대한 정보를 시그널로 보냄
        emit Send_Client_Name(id, name);
        i++;
    }
}

ClientManagerForm::~ClientManagerForm()                                                                         /*ClientManagerForm 소멸자*/
{
    delete ui;

    model->submitAll();                                                                                         //
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();                                                                                             // 데이터베이스 종료
}

int ClientManagerForm::makeId( )                                                                                /*id 자동생성 함수*/
{
    if(model->rowCount() == 0) {
        return 100;                                                                                             // 테이블에 내용이 없으면 100부터 시작
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;                                                                                            // 그렇지않으면 테이블의 마지막 행의 id값에 1을 더해 리턴
    }
}

void ClientManagerForm::removeItem()                                                                            /*고객정보를 삭제하는 슬롯*/
{
    QModelIndex index = ui->tableView->currentIndex();                                                          // 테이블 뷰에서 선택된 현재 인덱스 값을 저장
    model->removeRow(index.row());                                                                              // 선택한 행을 삭제
    model->select();                                                                                            // 수정된 모델 다시 선택
    ui->tableView->update();                                                                                    // 테이블 뷰 갱신
}

void ClientManagerForm::showContextMenu(const QPoint &pos)                                                      /*마우스 우측버튼 누를경우 발생하는 슬롯*/
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);                                                         // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                                                                      // 마우스 커서 좌표에서 메뉴를 표시
}

void ClientManagerForm::on_searchPushButton_clicked()                                                           /*서치버튼을 누를 경우 발생하는 슬롯*/
{
    searchModel->clear();                                                                                       // searchModel 내용 비움
    searchModel->setHorizontalHeaderLabels(header);                                                             // searchModel에 표시할 헤더 설정
    ui->searchTableView->setModel(searchModel);                                                                 // 테이블 뷰로 표시할 모델 선택
    int i = ui->searchComboBox->currentIndex();                                                                 // searchTableView에 선택된 현재 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;                                                                    // 문자열이 서로 일치하는지 비교하는 플래그

    QModelIndexList indexes = model->match(model->index(0, i), Qt::EditRole, ui->searchLineEdit->text(), -1, Qt::MatchFlags(flag));     // 문자열이 일치하는 항목의 인덱스들을 가져옴

    foreach(auto index, indexes)
    {
        int id = model->data(index.siblingAtColumn(0)).toInt();
        QString name = model->data(index.siblingAtColumn(1)).toString();
        QString number = model->data(index.siblingAtColumn(2)).toString();
        QString address = model->data(index.siblingAtColumn(3)).toString();                                     // 조건이 만족하는 인덱스에 저장된 데이터를 변수에 저장
        QStringList strings;
        strings << QString::number(id) << name << number << address;                                            // 변수들을 리스트에 저장

        QList<QStandardItem *> items;
        for (int i = 0; i < 4; ++i) {
            items.append(new QStandardItem(strings.at(i)));                                                     // 리스트의 내용을 QStandardItem에 저장하고
        }
        searchModel->appendRow(items);                                                                          // 저장한 QStandardItem의 데이터를 테이블 뷰에 표시
    }
}

void ClientManagerForm::on_modifyPushButton_clicked()                                                           /*변경버튼을 누를경우 발생하는 슬롯*/
{
    QModelIndex index = ui->tableView->currentIndex();                                                          // 테이블 뷰에 선택된 현재 인덱스 값 가져옴
    int id;
    QString name, number, address;
    id = model->data(index.siblingAtColumn(0)).toInt();
    name = ui->nameLineEdit->text();
    number = ui->phoneNumberLineEdit->text();
    address = ui->addressLineEdit->text();                                                                      // 라인에딧에 저장한 고객정보들을 변수에 저장

    QSqlQuery query(db);
    query.prepare("UPDATE clientManager SET "
                  "Name=:name, PhoneNumber=:number, Address=:address "
                  "WHERE ID=:id");                                                                              // id에 맞는 행의 정보들을 라인에딧에 정의한 정보들로 수정

    query.bindValue(":name", ui->nameLineEdit->text());
    query.bindValue(":number", ui->phoneNumberLineEdit->text());
    query.bindValue(":address", ui->addressLineEdit->text());
    query.bindValue(":id", ui->idLineEdit->text());
    query.exec();                                                                                               // 수정된 값을 테이블에 삽입
    model->select();                                                                                            // 수정된 모델 선택
}

void ClientManagerForm::on_addPushButton_clicked()                                                              /*추가버튼 눌렀을 경우*/
{
    int id = makeId( );
    QString name = ui->nameLineEdit->text();
    QString number = ui->phoneNumberLineEdit->text();
    QString address = ui->addressLineEdit->text();                                                              // 라인에딧에 정의한 고객정보들을 변수에 삽입

    QSqlQuery query(db);
    query.prepare("insert into clientManager(ID, Name, PhoneNumber, Address) "
                  "values (:id, :name, :number, :address)");                                                    // 새로운 고객에 대한 정보를 테이블에 삽입
    query.bindValue(":id", QString::number(id));
    query.bindValue(":name", ui->nameLineEdit->text());
    query.bindValue(":number", ui->phoneNumberLineEdit->text());
    query.bindValue(":address", ui->addressLineEdit->text());
    query.exec();

    model->select();                                                                                            // 수정된 모델 선택
}

void ClientManagerForm::on_tableView_clicked(QModelIndex index)                                                 /*테이블 뷰를 클릭했을 경우 발생하는 슬롯*/
{
    index = ui->tableView->currentIndex();
    int id;
    QString name, phoneNumber, address;
    id = model->data(index.siblingAtColumn(0)).toInt();
    name = model->data(index.siblingAtColumn(1)).toString();
    phoneNumber = model->data(index.siblingAtColumn(2)).toString();
    address = model->data(index.siblingAtColumn(3)).toString();                                                 // 테이블 뷰에 선택된 행의 정보들을 불러와 변수에 저장

    ui->idLineEdit->setText(QString::number(id));
    ui->nameLineEdit->setText(name);
    ui->phoneNumberLineEdit->setText(phoneNumber);
    ui->addressLineEdit->setText(address);                                                                      // 불러온 정보들을 각각의 라인에딧에 표시
}

bool ClientManagerForm::createConnection()                                                                      /*데이터베이스 생성하는 함수*/
{
    db = QSqlDatabase::addDatabase("QSQLITE", "clientConnection");                                              // QSQLITE를 데이터베이스로 사용
    db.setDatabaseName("client1.data");                                                                         // 데이터베이스 이름 설정
    db.open();                                                                                                  // 데이터베이스 오픈

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS clientManager(ID INTERGER Primary Key, "
               "Name VARCHAR(20) NOT NULL, PhoneNumber VARCHAR(20), Address VARCHAR(20));");                    // clientManager라는 테이블을 만들고 컬럼 정의

    return true;
}
