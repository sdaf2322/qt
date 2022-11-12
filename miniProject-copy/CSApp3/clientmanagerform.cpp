#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
#include "clientitem.h"

#include <QFile>
#include <QMenu>

ClientManagerForm::ClientManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);
    ui->idLineEdit->setDisabled(true);
    ui->phoneNumberLineEdit->setInputMask("000-0000-0000");
    QList<int> sizes;
    sizes << 540 << 400;                    // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                     // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));         // 삭제 누르면 항목 삭제

    menu = new QMenu;                                                       // 마우스 우측 버튼 눌렀을 때 메뉴 뜸
    menu->addAction(removeAction);                                          // 메뉴 만들어서 추가
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);            // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
    // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸
    connect(ui->searchLineEdit, SIGNAL(returnPressed()), SLOT(on_searchPushButton_clicked()));
    // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴
}

void ClientManagerForm::LoadData()
{
    QModelIndex index;
    int i = 0;
    createConnection();
    model = new QSqlTableModel(this, db);
    model->setTable("clientManager");
    ui->tableView->setModel(model);
    model->select();
    QSqlQuery query("SELECT * FROM clientManager");

    while(i < model->rowCount())
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString name = model->data(model->index(i, 1)).toString();
        QString phoneNumber = model->data(model->index(i, 2)).toString();
        QString address = model->data(model->index(i, 3)).toString();
        emit clientAdded(id, name);
        emit Send_Client_Name(id, name);
        i++;
    }
}

ClientManagerForm::~ClientManagerForm()     // 파일 저장
{
    delete ui;

    model->submitAll();
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();
}

int ClientManagerForm::makeId( )            // id 자동생성
{
    if(model->rowCount() == 0) {
        return 100;
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;
    }
    qDebug("%d", __LINE__);
}

void ClientManagerForm::removeItem()        // 아이템 제거
{
    QModelIndex index = ui->tableView->currentIndex();
    model->removeRow(index.row());
    model->select();
    ui->tableView->update();
    qDebug("%d", __LINE__);
}

void ClientManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);         // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                       // 마우스 커서 좌표에서 메뉴를 여는 것 같음
    qDebug("%d", __LINE__);
}

void ClientManagerForm::on_searchPushButton_clicked()
{
    ui->searchTreeWidget->clear();                                // searchTreeWidget 창 비우기

    int i = ui->searchComboBox->currentIndex();                   // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    // i가 0이 아니면 검색의 대소문자 구분하거나 검색어가 항목에 포함, 0이면 검색의 대소문자 구분???

    QModelIndexList indexes = model->match(model->index(0, i), Qt::EditRole, ui->searchLineEdit->text(), -1, Qt::MatchFlags(flag));
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->searchTreeWidget);;

    foreach(auto i, indexes)
    {
        QModelIndex index = static_cast<QModelIndex>(i);
        int id = model->data(index.siblingAtColumn(0)).toInt();
        QString name = model->data(index.siblingAtColumn(1)).toString();
        QString number = model->data(index.siblingAtColumn(2)).toString();
        QString address = model->data(index.siblingAtColumn(3)).toString();

        item->setText(0, QString::number(id));
        item->setText(1, name);
        item->setText(2, number);
        item->setText(3, address);

        ui->searchTreeWidget->addTopLevelItem(item);
    }
    qDebug("%d", __LINE__);
}

void ClientManagerForm::on_modifyPushButton_clicked()               // 커넥트 없어도 자동으로 연결됨. qt문법
{
    QModelIndex index = ui->tableView->currentIndex();
    int id;
    QString name, number, address;                              // 이름, 전화번호, 주소
    id = model->data(index.siblingAtColumn(0)).toInt();
    name = ui->nameLineEdit->text();
    number = ui->phoneNumberLineEdit->text();
    address = ui->addressLineEdit->text();

    QSqlQuery query(db);
    query.prepare("UPDATE clientManager SET "
                  "Name=:name, PhoneNumber=:number, Address=:address "
                  "WHERE ID=:id");

    query.bindValue(":name", ui->nameLineEdit->text());
    query.bindValue(":number", ui->phoneNumberLineEdit->text());
    query.bindValue(":address", ui->addressLineEdit->text());
    query.bindValue(":id", ui->idLineEdit->text());
    query.exec();
    model->select();
}

void ClientManagerForm::on_addPushButton_clicked()                  // 커넥트 없어도 자동으로 연결됨. qt문법
{
    qDebug() << "hi";
    int id = makeId( );
    QString name = ui->nameLineEdit->text();
    QString number = ui->phoneNumberLineEdit->text();
    QString address = ui->addressLineEdit->text();

    QSqlQuery query(db);
    query.prepare("insert into clientManager(ID, Name, PhoneNumber, Address) "
                  "values (:id, :name, :number, :address)");
    query.bindValue(":id", QString::number(id));
    query.bindValue(":name", ui->nameLineEdit->text());
    query.bindValue(":number", ui->phoneNumberLineEdit->text());
    query.bindValue(":address", ui->addressLineEdit->text());
    query.exec();

    model->select();
}

void ClientManagerForm::on_tableView_clicked(QModelIndex index)
{
    index = ui->tableView->currentIndex();
    int id;
    QString name, phoneNumber, address;
    id = model->data(index.siblingAtColumn(0)).toInt();
    name = model->data(index.siblingAtColumn(1)).toString();
    phoneNumber = model->data(index.siblingAtColumn(2)).toString();
    address = model->data(index.siblingAtColumn(3)).toString();

    ui->idLineEdit->setText(QString::number(id));
    ui->nameLineEdit->setText(name);
    ui->phoneNumberLineEdit->setText(phoneNumber);
    ui->addressLineEdit->setText(address);
}

bool ClientManagerForm::createConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "clientConnection");
    db.setDatabaseName("client1.data");
    db.open();

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS clientManager(ID INTERGER Primary Key, "
               "Name VARCHAR(20) NOT NULL, PhoneNumber VARCHAR(20), Address VARCHAR(20));");

    return true;
}
