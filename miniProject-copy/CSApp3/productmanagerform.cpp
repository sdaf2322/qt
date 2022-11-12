#include "productmanagerform.h"
#include "ui_productmanagerform.h"
#include "productitem.h"

#include <QFile>
#include <QMenu>

ProductManagerForm::ProductManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManagerForm)
{
    ui->setupUi(this);
    ui->idLineEdit->setDisabled(true);
    ui->optionLineEdit->setPlaceholderText("Please use the '/' to separate options.");

    QList<int> sizes;
    sizes << 540 << 400;                    // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                     // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));         // 삭제 누르면 항목 삭제

    menu = new QMenu;                                                       // 마우스 우측 버튼 눌렀을 때 메뉴 뜸
    menu->addAction(removeAction);                                          // 메뉴 만들어서 추가
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);            // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
    // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴

}

void ProductManagerForm::LoadData()
{
    QModelIndex index;
    int i = 0;
    createConnection();
    model = new QSqlTableModel(this, db);
    model->setTable("productManager");
    ui->tableView->setModel(model);
    model->select();
    QSqlQuery query("SELECT * FROM productManager");

    while(i < model->rowCount())
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString productName = model->data(model->index(i, 1)).toString();
        QString price = model->data(model->index(i, 2)).toString();
        QString productType = model->data(model->index(i, 3)).toString();
        QString option = model->data(model->index(i, 4)).toString();
        emit Add_Product_Info(id, productName, price, productType, option);
        emit Send_Product_Price(id, price);
        emit Send_ProductName(id, productName);
        i++;
    }
}

ProductManagerForm::~ProductManagerForm()     // 파일 저장
{
    delete ui;

    model->submitAll();
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();
}

int ProductManagerForm::makeId( )            // id 자동생성
{
    if(model->rowCount() == 0) {
        return 100;
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;
    }
    qDebug("%d", __LINE__);
}

void ProductManagerForm::removeItem()        // 아이템 제거
{
    QModelIndex index = ui->tableView->currentIndex();
    model->removeRow(index.row());
    model->select();
    ui->tableView->update();
    qDebug("%d", __LINE__);
    ui->idLineEdit->clear();
    emit remove_item();
    emit Remove_productTreeWidget_Item();
}

void ProductManagerForm::signal_emit()
{
    QString productName, price, productType, option;
    int id;
    int i = 0;

    while(i < model->rowCount())
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString productName = model->data(model->index(i, 1)).toString();
        QString price = model->data(model->index(i, 2)).toString();
        QString productType = model->data(model->index(i, 3)).toString();
        QString option = model->data(model->index(i, 4)).toString();
        emit Send_Product_Info(id, productName, price, productType, option);
        i++;
    }
}

void ProductManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);         // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                       // 마우스 커서 좌표에서 메뉴를 여는 것 같음
}

void ProductManagerForm::on_searchPushButton_clicked()
{
    ui->searchTreeWidget->clear();                                // searchTreeWidget 창 비우기

    int i = ui->searchComboBox->currentIndex();                   // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    // i가 0이 아니면 검색의 대소문자 구분하거나 검색어가 항목에 포함, 0이면 검색의 대소문자 구분???

    QModelIndexList indexes = model->match(model->index(0, i), Qt::EditRole, ui->searchLineEdit->text(), -1, Qt::MatchFlags(flag));
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->searchTreeWidget);

    foreach(auto i, indexes)
    {
        QModelIndex index = static_cast<QModelIndex>(i);
        int id = model->data(index.siblingAtColumn(0)).toInt();
        QString productName = model->data(index.siblingAtColumn(1)).toString();
        QString price = model->data(index.siblingAtColumn(2)).toString();
        QString productType = model->data(index.siblingAtColumn(3)).toString();
        QString option = model->data(index.siblingAtColumn(4)).toString();

        item->setText(0, QString::number(id));
        item->setText(1, productName);
        item->setText(2, price);
        item->setText(3, productType);
        item->setText(4, option);

        ui->searchTreeWidget->addTopLevelItem(item);
    }
}

void ProductManagerForm::on_modifyPushButton_clicked()               // 커넥트 없어도 자동으로 연결됨. qt문법
{
    QModelIndex index = ui->tableView->currentIndex();
    int id;
    QString productName, price, productType, option;                              // 이름, 전화번호, 주소
    id = model->data(index.siblingAtColumn(0)).toInt();
    productName = ui->productNameLineEdit->text();
    price = ui->priceLineEdit->text();
    productType = ui->productTypeLineEdit->text();
    option = ui->optionLineEdit->text();
    QSqlQuery query(db);
    query.prepare("UPDATE productManager SET "
                  "ProductName=:productName, Price=:price, ProductType=:productType, Option=:option "
                  "WHERE ID=:id");
    query.bindValue(":productName", productName);
    query.bindValue(":price", price);
    query.bindValue(":productType", productType);
    query.bindValue(":option", option);
    query.bindValue(":id", id);
    query.exec();
    model->select();
    emit remove_item();
}

void ProductManagerForm::on_addPushButton_clicked()                  // 커넥트 없어도 자동으로 연결됨. qt문법
{
    qDebug() << "hi";
    int id = makeId( );
    QString productName = ui->productNameLineEdit->text();
    QString price = ui->priceLineEdit->text();
    QString productType = ui->productTypeLineEdit->text();
    QString option = ui->optionLineEdit->text();
    QSqlQuery query(db);
    query.prepare("insert into productManager(ID, ProductName, Price, ProductType, Option) "
                  "values (:id, :productName, :price, :productType, :option)");
    query.bindValue(":id", id);
    query.bindValue(":productName", productName);
    query.bindValue(":price", price);
    query.bindValue(":productType", productType);
    query.bindValue(":option", option);
    query.exec();

    model->select();
    emit Add_Product_Info(id, productName, price, productType, option);
    emit Send_Product_Price(id, price);
    emit Send_ProductName(id, productName);
}

void ProductManagerForm::on_tableView_clicked(QModelIndex index)    // 커넥트 없어도 자동으로 연결됨. qt문법
{
    index = ui->tableView->currentIndex();
    int id;
    QString productName, price, productType, option;
    id = model->data(index.siblingAtColumn(0)).toInt();
    productName = model->data(index.siblingAtColumn(1)).toString();
    price = model->data(index.siblingAtColumn(2)).toString();
    productType = model->data(index.siblingAtColumn(3)).toString();
    option = model->data(index.siblingAtColumn(4)).toString();

    ui->idLineEdit->setText(QString::number(id));
    ui->productNameLineEdit->setText(productName);
    ui->priceLineEdit->setText(price);
    ui->productTypeLineEdit->setText(productType);
    ui->optionLineEdit->setText(option);
}

bool ProductManagerForm::createConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "productConnection");
    db.setDatabaseName("product1.data");
    db.open();

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS productManager(ID INTERGER Primary Key, "
               "ProductName VARCHAR(20) NOT NULL, Price VARCHAR(20), ProductType VARCHAR(20), Option VARCHAR(20));");

    return true;
}
