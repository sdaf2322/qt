#include "shopmanagerform.h"
#include "ui_shopmanagerform.h"

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
    sizes << 540 << 400;                                                                                    // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                                                     // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));                                         // 종료액션 누르면 항목 삭제

    menu = new QMenu;                                                                                       // 마우스 우측 버튼 눌렀을 때 메뉴
    menu->addAction(removeAction);                                                                          // 메뉴 만들어서 추가
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);                                             // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));      // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸

    // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴
    connect(ui->searchLineEdit, SIGNAL(returnPressed()), SLOT(on_searchPushButton_clicked()));

    ui->shopDateEdit->setDate(QDate::currentDate());                                                        // 날짜 다이얼로그의 날짜를 오늘 날짜로 설정
    ui->shopDateEdit->setMinimumDate(QDate(1900, 1, 1));                                                    // 날짜 다이얼로그의 입력가능한 최소 날짜
    ui->shopDateEdit->setMaximumDate(QDate(2100, 12, 31));                                                  // 날짜 다이얼로그의 입력가능한 최대 날짜

    searchModel = new QStandardItemModel(0, 9);                                                             // 테이블 모델의 값을 저장하기 위한 모델
    header << "ID" << "Name" << "ProductName" << "Option" << "Quantity" << "ShopDate" << "ClientID" << "ProductID" << "TotalPrice";     // 헤더로 들어갈 값
    searchModel->setHorizontalHeaderLabels(header);                                                         // 서치보델의 헤더 설정
    ui->searchTableView->setModel(searchModel);                                                             // 서치 테이블 뷰에서 서치모델의 정보를 보여줌

    productModel = new QStandardItemModel(0, 5);                                                            // 제품정보를 저장할 productModel 생성
    p_header << "ID" << "ProductName" << "Price" << "ProductType" << "Option";                              // 헤더로 들어갈 값
    productModel->setHorizontalHeaderLabels(p_header);                                                      // productModel의 헤더 설정
    ui->productTableView->setModel(productModel);                                                           // productTableView에서 productModel의 정보를 보여줌

    searchProductModel = new QStandardItemModel(0, 5);                                                      // 제품정보를 검색할 searchProductModel
    searchProductModel->setHorizontalHeaderLabels(p_header);                                                // searchProductModel의 헤더 설정
    ui->searchProductTableView->setModel(searchProductModel);                                               // searchProductTableView에서 searchProductModel의 정보 보여줌
}

void ShopManagerForm::LoadData()                                                                            /*프로그램 실행시 데이터를 불러오는 함수*/
{
    createConnection();                                                                                     // 데이터베이스 생성
    model = new QSqlTableModel(this, db);                                                                   // 모델에 저장할 데이터베이스 설정
    model->setTable("shopManager");                                                                         // 모델에 저장할 테이블 정보 설정
    ui->tableView->setModel(model);                                                                         // 테이블 뷰에서 모델의 정보 보여줌
    model->select();                                                                                        // 모델 선택
    QSqlQuery query("SELECT * FROM shopManager");                                                           // 쿼리문으로 테이블의 모든 정보 가져옴
}


ShopManagerForm::~ShopManagerForm()                                                                         /*ShopManagerForm의 소멸자*/
{
    delete ui;

    model->submitAll();
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();                                                                                         // 데이터베이스 종료
}

void ShopManagerForm::get_Client_Name(int id, QString name)                                                 /*고객명의 정보를 받는 슬롯*/
{
    client_name.insert(id, name);                                                                           // 해쉬에 고객명과 아이디를 저장
}

void ShopManagerForm::get_ProductName(int id, QString productName)                                          /*제품명의 정보를 받는 슬롯*/
{
    product_name.insert(id, productName);                                                                   // 해쉬에 제품명과 아이디 저장
}

void ShopManagerForm::get_Product_Price(int id, QString price)                                              /*제품가격의 정보를 저장하는 슬롯*/
{
    product_price.insert(id, price);                                                                        // 해쉬에 제품가격과 아이디를 저장
}

void ShopManagerForm::get_Product_Info(int id, QString productName, QString price, QString productType, QString option)     /*제품정보를 받는 슬롯*/
{
    productModel->setHorizontalHeaderLabels(p_header);                                                      // productModel의 헤더 설정
    ui->productTableView->setModel(productModel);                                                           // productTableView에서 productModel의 정보 보여줌
    QStringList strings;
    strings << QString::number(id) << productName << price << productType << option;                        // stringlist에 제품정보 저장

    QList<QStandardItem *> items;
    for (int i = 0; i < 5; ++i) {
        items.append(new QStandardItem(strings.at(i)));                                                     // QStandardItem 타입의 리스트에 stringList 저장
    }
    productModel->appendRow(items);                                                                         // productModel에 리스트의 정보 추가
}

void ShopManagerForm::productModel_clear()                                                                  /*productModel의 정보 지우는 슬롯*/
{
    productModel->clear();                                                                                  // productModel 정보 삭제
    emit clear_productTableView();                                                                         // 시그널 방출
}

int ShopManagerForm::makeId( )                                                                              /*id 자동생성하는 함수*/
{
    if(model->rowCount() == 0) {
        return 100;                                                                                         // 모델에 데이터 없으면 100부터 시작
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;                                                                                        // 그렇지않으면 모델의 마지막 id에 1을 더해 리턴
    }
}

void ShopManagerForm::removeItem()                                                                          /*모델의 행 정보 삭제하는 슬롯*/
{
    QModelIndex index = ui->tableView->currentIndex();                                                      // 테이블 뷰에서 현재 인덱스 값 가져옴
    model->removeRow(index.row());                                                                          // 현재 인덱스의 행 정보를 삭제
    model->select();                                                                                        // 모델 선택
    ui->tableView->update();                                                                                // 테이블 뷰 갱신
}

void ShopManagerForm::removeOption()                                                                        /*옵션 콤보박스의 내용 비우는 슬롯*/
{
    ui->optionComboBox->clear();
}

void ShopManagerForm::showContextMenu(const QPoint &pos)                                                    /*마우스 우측키 눌렀을 경우 발생하는 슬롯*/
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);                                                     // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                                                                  // 마우스 커서 좌표에서 메뉴를 생성
}

void ShopManagerForm::on_searchPushButton_clicked()                                                         /*서치버튼 눌렀을 경우 발생하는 슬롯*/
{
    searchModel->clear();                                                                                   // 서치모델 비우기
    searchModel->setHorizontalHeaderLabels(header);                                                         // 서치모델 헤더설정

    int i = ui->searchComboBox->currentIndex();                                                             // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;                                                                // 문자열이 일치하는지 비교하는 플래그

    // 문자열이 일치하는 인덱스 값들을 가져옴
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
        QString totalPrice = model->data(index.siblingAtColumn(8)).toString();                               // 문자열이 일치하는 인덱스 값에 해당하는 정보들을 변수에 저장
        QStringList strings;
        strings << QString::number(id) << name << option << productName << quantity << shopDate << QString::number(client_id)
                << QString::number(product_id) << totalPrice;                                                // stringList에 해당 정보들 저장

        QList<QStandardItem *> items;
        for (int i = 0; i < 9; ++i) {
            items.append(new QStandardItem(strings.at(i)));                                                  // QStandardItem 타입의 리스트에 stringList 삽입
        }
        searchModel->appendRow(items);                                                                       // 서치모델에 리스트 정보 올림
    }
}

void ShopManagerForm::on_addPushButton_clicked()                                                             /*추가버튼 눌렀을 경우 발생하는 슬롯*/
{
    int id = makeId( );
    int client_id = ui->clientIDLineEdit->text().toInt();
    int product_id = ui->productIDLineEdit->text().toInt();
    QString name = client_name.find(client_id).value();
    QString productName = product_name.find(product_id).value();
    QString option = ui->optionComboBox->currentText();
    QString quantity = ui->quantitySpinBox->text();
    QString shopDate = ui->shopDateEdit->text();
    QString price = product_price.find(product_id).value();
    QString totalPrice = QString::number(price.toInt() * quantity.toInt());                                   // 라인에딧에 기입한 제품정보들을 변수에 저장
    QSqlQuery query(db);

    query.prepare("insert into shopManager(ID, Name, ProductName, Option, Quantity, ShopDate, ClientID, ProductID, TotalPrice) "
                  "values (:id, :name, :productName, :option, :quantity, :shopDate, :client_id, :product_id, :totalPrice)");    // 쿼리문으로 테이블에 새로운 제품정보들 추가
    query.bindValue(":id", QString::number(id));
    query.bindValue(":name", name);
    query.bindValue(":productName", productName);
    query.bindValue(":option", option);
    query.bindValue(":quantity", quantity);
    query.bindValue(":shopDate", shopDate);
    query.bindValue(":client_id", QString::number(client_id));
    query.bindValue(":product_id", QString::number(product_id));
    query.bindValue(":totalPrice", totalPrice);
    query.exec();
    model->select();                                                                                            // 모델 선택
}

void ShopManagerForm::on_tableView_clicked(QModelIndex index)                                                   /*테이블 뷰 눌렀을 경우 발생하는 슬롯*/
{
    index = ui->tableView->currentIndex();                                                                      // 테이블 뷰에서 현재 인덱스 값 가져옴
    int id;
    int client_id, product_id;
    QString option, quantity, shopDate;

    id = model->data(index.siblingAtColumn(0)).toInt();
    client_id = model->data(index.siblingAtColumn(6)).toInt();
    product_id = model->data(index.siblingAtColumn(7)).toInt();
    option = model->data(index.siblingAtColumn(3)).toString();
    quantity = model->data(index.siblingAtColumn(4)).toString();
    shopDate = model->data(index.siblingAtColumn(5)).toString();                                                // 현재 인덱스 값에 해당하는 정보들 변수에 저장

    ui->idLineEdit->setText(QString::number(id));
    ui->clientIDLineEdit->setText(QString::number(client_id));
    ui->productIDLineEdit->setText(QString::number(product_id));
    ui->optionComboBox->clear();
    ui->optionComboBox->addItem(option);
    ui->quantitySpinBox->setValue(quantity.toInt());
    ui->shopDateEdit->setDate(QDate::fromString(shopDate));                                                     // 라인에딧에 각각의 변수들의 정보들을 표시
}

void ShopManagerForm::on_productTableView_clicked(QModelIndex index)                                            /*productTableView를 클릭했을 경우 발생하는 슬롯*/
{
    int id;
    QString option;

    list.clear();                                                                                               // 옵션에 대한 정보를 담을 리스트 비움
    ui->optionComboBox->clear();                                                                                // 옵션 콤보박스 비움

    id = productModel->data(index.siblingAtColumn(0)).toInt();                                                  // productTableView에 선택된 현재 인덱스의 아이디
    option = productModel->data(index.siblingAtColumn(4)).toString();                                           // productTableView에 선택된 현재 인덱스의 옵션

    ui->productIDLineEdit->setText(QString::number(id));                                                        // productIDLineEdit에 아이디 정보 표시
    list = option.split("/");                                                                                   // 옵션을 '/'단위로 끊어서 리스트에 저장
    foreach(auto i, list)
    {
        ui->optionComboBox->addItem(i);                                                                         // 리스트의 내용을 콤보박스에 추가
    }
}

void ShopManagerForm::on_searchProductPushButton_clicked()                                                      /*searchProductPushButton을 눌렀을 경우 발생하는 슬롯*/
{
    searchProductModel->clear();                                                                                // searchProductModel 비움
    searchProductModel->setHorizontalHeaderLabels(p_header);                                                    // searchProductModel의 헤더 설정

    int i = ui->searchProductComboBox->currentIndex();                                                          // sarchComboBox의 현재 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;                                                                    // 문자열이 일치하는지 비교하는 플래그

    // 문자열이 일치하는 항목들의 인덱스 값들을 불러옴
    QModelIndexList indexes = productModel->match(productModel->index(0, i), Qt::EditRole, ui->searchProductLineEdit->text(), -1, Qt::MatchFlags(flag));

    foreach(auto index, indexes)
    {
        int id = productModel->data(index.siblingAtColumn(0)).toInt();
        QString productName = productModel->data(index.siblingAtColumn(1)).toString();
        QString price = productModel->data(index.siblingAtColumn(2)).toString();
        QString productType = productModel->data(index.siblingAtColumn(3)).toString();
        QString option = productModel->data(index.siblingAtColumn(4)).toString();                               // 문자열이 일치하는 항목들의 정보를 변수에 저장
        QStringList strings;
        strings << QString::number(id) << productName << price << productType << option;                        // stringList에 해당 정보들을 저장

        QList<QStandardItem *> items;
        for (int i = 0; i < 5; ++i) {
            items.append(new QStandardItem(strings.at(i)));                                                     // QStandardItem 타입의 리스트에 stringList 삽입
        }
        searchProductModel->appendRow(items);                                                                   // searchProductModel에 리스트 추가
    }
}

void ShopManagerForm::on_searchProductTableView_clicked(QModelIndex index)                                      /*searchProductTableView를 클릭했을 경우 발생하는 슬롯*/
{
    int id;
    QString option;

    list.clear();                                                                                               // 옵션의 정보를 저장할 리스트 비움
    ui->optionComboBox->clear();                                                                                // 옵션 콤보박스 비움

    id = searchProductModel->data(index.siblingAtColumn(0)).toInt();                                            // searchProductModel의 현재 인덱스 값의 아이디를 가져옴
    option = searchProductModel->data(index.siblingAtColumn(4)).toString();                                     // searchProductModel의 현재 인덱스 값의 옵션에 대한 정보 가져옴

    ui->productIDLineEdit->setText(QString::number(id));                                                        // productIDLineEdit에 표시
    list = option.split("/");                                                                                   // 옵션의 정보를 '/'단위로 끊어 리스트에 저장
    foreach(auto i, list)
    {
        ui->optionComboBox->addItem(i);                                                                         // 옵션 콤보박스에 리스트에 저장된 내용 삽입
    }
}

bool ShopManagerForm::createConnection()                                                                        /*데이터베이스 생성하는 함수*/
{
    db = QSqlDatabase::addDatabase("QSQLITE", "shopConnection");                                                // QSQLITE를 데이터베이스로 사용,
    db.setDatabaseName("shop1.data");                                                                           // 데이터베이스 이름 설정
    db.open();                                                                                                  // 데이터베이스 오픈

    QSqlQuery query(db);                                                                                        // 쿼리문으로 테이블 및 컬럼 생성
    query.exec("CREATE TABLE IF NOT EXISTS shopManager(ID INTERGER Primary Key, "
               "Name VARCHAR(20) NOT NULL, ProductName VARCHAR(20), Option VARCHAR(20), "
               "Quantity VARCHAR(20), ShopDate VARCHAR(20), ClientID INTERGER, ProductID INTERGER, TotalPrice VARCHAR(20));");

    return true;
}

