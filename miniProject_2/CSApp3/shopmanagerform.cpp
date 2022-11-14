#include "shopmanagerform.h"
#include "ui_shopmanagerform.h"
//#include "shopitem.h"

#include <QFile>
#include <QMenu>


ShopManagerForm::ShopManagerForm(QWidget *parent, ClientManagerForm* cm, ProductManagerForm* pm) :
    QWidget(parent), cm(cm), pm(pm),
    ui(new Ui::ShopManagerForm)
{
    ui->setupUi(this);
    ui->idLineEdit->setDisabled(true);
    ui->productIDLineEdit->setDisabled(true);
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

    ui->shopDateEdit->setDate(QDate::currentDate());
    ui->shopDateEdit->setMinimumDate(QDate(1900, 1, 1));
    ui->shopDateEdit->setMaximumDate(QDate(2100, 12, 31));

    searchModel = new QStandardItemModel(0, 9);
    header << "ID" << "Name" << "ProductName" << "Option" << "Quantity" << "ShopDate" << "ClientID" << "ProductID" << "TotalPrice";
    searchModel->setHorizontalHeaderLabels(header);
    ui->searchTableView->setModel(searchModel);

    productModel = new QStandardItemModel(0, 5);
    p_header << "ID" << "ProductName" << "Price" << "ProductType" << "Option";
    productModel->setHorizontalHeaderLabels(p_header);
    ui->productTableView->setModel(productModel);

    searchProductModel = new QStandardItemModel(0, 5);
    searchProductModel->setHorizontalHeaderLabels(p_header);
    ui->searchProductTableView->setModel(searchProductModel);
}

void ShopManagerForm::LoadData()
{
    createConnection();
    model = new QSqlTableModel(this, db);
    model->setTable("shopManager");
    ui->tableView->setModel(model);
    model->select();
    QSqlQuery query("SELECT * FROM shopManager");
}


ShopManagerForm::~ShopManagerForm()     // 파일 저장
{
    delete ui;

    model->submitAll();
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();
}

void ShopManagerForm::get_Client_Name(int id, QString name)
{
    client_name.insert(id, name);
}

void ShopManagerForm::get_ProductName(int id, QString productName)
{
    product_name.insert(id, productName);
}

void ShopManagerForm::get_Product_Price(int id, QString price)
{
    product_price.insert(id, price);
}

void ShopManagerForm::get_Product_Info(int id, QString productName, QString price, QString productType, QString option)
{
    productModel->setHorizontalHeaderLabels(p_header);
    ui->productTableView->setModel(productModel);
    QStringList strings;
    strings << QString::number(id) << productName << price << productType << option;

    QList<QStandardItem *> items;
    for (int i = 0; i < 5; ++i) {
        items.append(new QStandardItem(strings.at(i)));
    }
    productModel->appendRow(items);
}

void ShopManagerForm::productModel_clear()
{
    productModel->clear();
    emit clear_productTreeWidget();
}

int ShopManagerForm::makeId( )            // id 자동생성
{
    if(model->rowCount() == 0) {
        return 100;
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;
    }
    qDebug("%d", __LINE__);
}

void ShopManagerForm::removeItem()        // 아이템 제거
{
    QModelIndex index = ui->tableView->currentIndex();
    model->removeRow(index.row());
    model->select();
    ui->tableView->update();
    qDebug("%d", __LINE__);
}

void ShopManagerForm::removeOption()
{
    ui->optionComboBox->clear();
}

void ShopManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);         // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                       // 마우스 커서 좌표에서 메뉴를 여는 것 같음
}

void ShopManagerForm::on_searchPushButton_clicked()
{
    searchModel->clear();                               // searchTreeWidget 창 비우기
    searchModel->setHorizontalHeaderLabels(header);

    int i = ui->searchComboBox->currentIndex();                   // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;

    QModelIndexList indexes = model->match(model->index(0, i), Qt::EditRole, ui->searchLineEdit->text(), -1, Qt::MatchFlags(flag));

    foreach(auto index, indexes)
    {
        int id = model->data(index.siblingAtColumn(0)).toInt();
        QString name = model->data(index.siblingAtColumn(1)).toString();
        QString option = model->data(index.siblingAtColumn(2)).toString();
        QString productName = model->data(index.siblingAtColumn(3)).toString();
        QString quantity = model->data(index.siblingAtColumn(4)).toString();
        QString shopDate = model->data(index.siblingAtColumn(5)).toString();
        int client_id = model->data(index.siblingAtColumn(6)).toInt();
        int product_id = model->data(index.siblingAtColumn(7)).toInt();
        QString totalPrice = model->data(index.siblingAtColumn(8)).toString();
        QStringList strings;
        strings << QString::number(id) << name << option << productName << quantity << shopDate << QString::number(client_id)
                << QString::number(product_id) << totalPrice;

        QList<QStandardItem *> items;
        for (int i = 0; i < 9; ++i) {
            items.append(new QStandardItem(strings.at(i)));
        }
        searchModel->appendRow(items);

    }
    qDebug("%d", __LINE__);
}

void ShopManagerForm::on_addPushButton_clicked()                  // 커넥트 없어도 자동으로 연결됨. qt문법
{
    int id = makeId( );
    int client_id = ui->clientIDLineEdit->text().toInt();
    int product_id = ui->productIDLineEdit->text().toInt();
    QString name = client_name.find(client_id).value();
    qDebug("%d", __LINE__);
    QString productName = product_name.find(product_id).value();
    QString option = ui->optionComboBox->currentText();
    QString quantity = ui->quantitySpinBox->text();
    QString shopDate = ui->shopDateEdit->text();
    QString price = product_price.find(product_id).value();
    QString totalPrice = QString::number(price.toInt() * quantity.toInt());
    QSqlQuery query(db);

    query.prepare("insert into shopManager(ID, Name, ProductName, Option, Quantity, ShopDate, ClientID, ProductID, TotalPrice) "
                  "values (:id, :name, :productName, :option, :quantity, :shopDate, :client_id, :product_id, :totalPrice)");
    query.bindValue(":id", QString::number(id));
    query.bindValue(":name", name);
    query.bindValue(":productName", productName);
    query.bindValue(":option", option);
    query.bindValue(":quantity", quantity);
    query.bindValue(":shopDate", shopDate);
    query.bindValue(":client_id", QString::number(client_id));
    query.bindValue(":product_id", QString::number(product_id));
    query.bindValue(":totalPrice", totalPrice);
    qDebug("%d", __LINE__);

    query.exec();
    qDebug("%d", __LINE__);

    model->select();
}

void ShopManagerForm::on_tableView_clicked(QModelIndex index)
{
    index = ui->tableView->currentIndex();
    int id;
    int client_id, product_id;
    QString option, quantity, shopDate;

    id = model->data(index.siblingAtColumn(0)).toInt();
    client_id = model->data(index.siblingAtColumn(6)).toInt();
    product_id = model->data(index.siblingAtColumn(7)).toInt();
    option = model->data(index.siblingAtColumn(3)).toString();
    quantity = model->data(index.siblingAtColumn(4)).toString();
    shopDate = model->data(index.siblingAtColumn(5)).toString();

    ui->idLineEdit->setText(QString::number(id));
    ui->clientIDLineEdit->setText(QString::number(client_id));
    ui->productIDLineEdit->setText(QString::number(product_id));
    ui->optionComboBox->clear();
    ui->optionComboBox->addItem(option);
    ui->quantitySpinBox->setValue(quantity.toInt());
    ui->shopDateEdit->setDate(QDate::fromString(shopDate));
}

void ShopManagerForm::on_productTableView_clicked(QModelIndex index)
{
    int id;
    QString option;

    list.clear();
    ui->optionComboBox->clear();

    id = productModel->data(index.siblingAtColumn(0)).toInt();
    option = productModel->data(index.siblingAtColumn(4)).toString();

    ui->productIDLineEdit->setText(QString::number(id));
    list = option.split("/");
    foreach(auto i, list)
    {
        ui->optionComboBox->addItem(i);
    }
    ui->productIDLineEdit->setText(QString::number(id));
}

void ShopManagerForm::on_searchProductPushButton_clicked()
{
    searchProductModel->clear();
    searchProductModel->setHorizontalHeaderLabels(p_header);                              // searchTreeWidget 창 비우기

    int i = ui->searchProductComboBox->currentIndex();                         // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;

    QModelIndexList indexes = productModel->match(productModel->index(0, i), Qt::EditRole, ui->searchProductLineEdit->text(), -1, Qt::MatchFlags(flag));

    foreach(auto index, indexes)
    {
        int id = productModel->data(index.siblingAtColumn(0)).toInt();
        QString productName = productModel->data(index.siblingAtColumn(1)).toString();
        QString price = productModel->data(index.siblingAtColumn(2)).toString();
        QString productType = productModel->data(index.siblingAtColumn(3)).toString();
        QString option = productModel->data(index.siblingAtColumn(4)).toString();
        QStringList strings;
        strings << QString::number(id) << productName << price << productType << option;

        QList<QStandardItem *> items;
        for (int i = 0; i < 5; ++i) {
            items.append(new QStandardItem(strings.at(i)));
        }
        searchProductModel->appendRow(items);
    }
}

void ShopManagerForm::on_searchProductTableView_clicked(QModelIndex index)
{
    int id;
    QString option;

    list.clear();
    ui->optionComboBox->clear();

    id = searchProductModel->data(index.siblingAtColumn(0)).toInt();
    option = searchProductModel->data(index.siblingAtColumn(4)).toString();

    ui->productIDLineEdit->setText(QString::number(id));
    list = option.split("/");
    foreach(auto i, list)
    {
        ui->optionComboBox->addItem(i);
    }
    ui->productIDLineEdit->setText(QString::number(id));
}

bool ShopManagerForm::createConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "shopConnection");
    db.setDatabaseName("shop1.data");
    db.open();

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS shopManager(ID INTERGER Primary Key, "
               "Name VARCHAR(20) NOT NULL, ProductName VARCHAR(20), Option VARCHAR(20), "
               "Quantity VARCHAR(20), ShopDate VARCHAR(20), ClientID INTERGER, ProductID INTERGER, TotalPrice VARCHAR(20));");

    return true;
}

