#include "productmanagerform.h"
#include "ui_productmanagerform.h"
//#include "productitem.h"

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
    sizes << 540 << 400;                                                                                             // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                                                              // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));                                                  // 종료액션 누르면 항목 삭제

    menu = new QMenu;                                                                                                // 마우스 우측 버튼 눌렀을 때 메뉴 생성
    menu->addAction(removeAction);                                                                                   // 메뉴에 종료액션 추가
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);                                                      // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));         // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸


    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));                                     // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴

    searchModel = new QStandardItemModel(0, 5);                                                                      // productModel의 내용을 저장할 모델 생성
    header << "ID" << "ProductName" << "Price" << "ProductType" << "Option";                                         // searchModel의 헤더로 쓰일 항목 리스트
    searchModel->setHorizontalHeaderLabels(header);                                                                  // searchModel 헤더 설정
    ui->searchTableView->setModel(searchModel);                                                                      // searchTableView에 출력할 모델 설정
}

void ProductManagerForm::LoadData()                                                          /*프로그램 실행시 데이터를 불러오는 함수*/
{
    QModelIndex index;
    int i = 0;
    createConnection();                                                                      // 데이터베이스 생성
    model = new QSqlTableModel(this, db);                                                    // 사용할 데이터베이스 설정
    model->setTable("productManager");                                                       // 데이터베이스에 테이블 생성
    ui->tableView->setModel(model);                                                          // 테이블 뷰에 출력할 모델 설정
    model->select();                                                                         // 모델 선택
    QSqlQuery query("SELECT * FROM productManager");                                         // 쿼리문으로 productManager 테이블의 모든 내용을 불러옴

    while(i < model->rowCount())                                                             // 테이블의 모든 정보를 가져오기 위한 반복문
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString productName = model->data(model->index(i, 1)).toString();
        QString price = model->data(model->index(i, 2)).toString();
        QString productType = model->data(model->index(i, 3)).toString();
        QString option = model->data(model->index(i, 4)).toString();                         // 테이블 내의 제품정보들을 각각의 변수에 저장
        emit Add_Product_Info(id, productName, price, productType, option);                  // 제품정보를 시그널로 방출
        emit Send_Product_Price(id, price);                                                  // 제품가격에 대한 정보를 시그널로 방출
        emit Send_ProductName(id, productName);                                              // 제품명에 대한 정보를 시그널로 방출
        i++;
    }
}

ProductManagerForm::~ProductManagerForm()                                                    /*ProductManagerForm 소멸자*/
{
    delete ui;

    model->submitAll();                                                                      //
    QSqlDatabase db = model->database();
    if(db.isOpen())
        db.close();                                                                          // 데이터베이스 종료
}

int ProductManagerForm::makeId( )                                                            /*id 자동생성하는 함수*/
{
    if(model->rowCount() == 0) {
        return 100;                                                                          // 모델에 데이터가 없으면 100부터 시작
    } else {
        auto id = model->data(model->index(model->rowCount()-1, 0)).toInt();
        return ++id;                                                                         // 그렇지않으면 모델의 마지막 아이디에 1을 더해 리턴
    }
}

void ProductManagerForm::removeItem()                                                        /*테이블 뷰의 내용을 제거하는 슬롯*/
{
    QModelIndex index = ui->tableView->currentIndex();                                       // 현재 인덱스 값 가져옴
    model->removeRow(index.row());                                                           // 현재 인덱스에서 행의 정보를 가져와 삭제
    model->select();                                                                         // 모델 선택
    ui->tableView->update();                                                                 // 테이블 뷰 갱신
    ui->idLineEdit->clear();                                                                 // idLineEdit 비움
    emit remove_item();                                                                      // 시그널 방출
    emit Remove_productTreeWidget_Item();                                                    // 시그널 방출
}

void ProductManagerForm::signal_emit()                                                       /*시그널을 방출하는 슬롯*/
{
    QString productName, price, productType, option;
    int id;
    int i = 0;

    while(i < model->rowCount())                                                             // 테이블의 모든 데이터를 가져오기 위한 반복문
    {
        int id = model->data(model->index(i, 0)).toInt();
        QString productName = model->data(model->index(i, 1)).toString();
        QString price = model->data(model->index(i, 2)).toString();
        QString productType = model->data(model->index(i, 3)).toString();
        QString option = model->data(model->index(i, 4)).toString();
        emit Add_Product_Info(id, productName, price, productType, option);                  // 제품정보를 시그널로 방출
        i++;
    }
}

void ProductManagerForm::showContextMenu(const QPoint &pos)                                  /*마우스 우측키 눌렀을 경우 발생하는 슬롯*/
{
    QPoint globalPos = ui->tableView->mapToGlobal(pos);                                      // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                                                   // 마우스 커서 좌표에서 메뉴를 생성
}

void ProductManagerForm::on_searchPushButton_clicked()                                       /*서치버튼 눌렀을 경우 발생하는 슬롯*/
{
    searchModel->clear();                                                                    // 서치모델 내용 지움
    searchModel->setHorizontalHeaderLabels(header);                                          // 서치모델 헤더설정
    int i = ui->searchComboBox->currentIndex();                                              // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;                                                 // 문자열이 서로 일치하는지 비교하는 플래그

    // 문자열이 일치하는 항목들의 인덱스 값들을 가져옴
    QModelIndexList indexes = model->match(model->index(0, i), Qt::EditRole, ui->searchLineEdit->text(), -1, Qt::MatchFlags(flag));

    foreach(auto index, indexes)
    {
        int id = model->data(index.siblingAtColumn(0)).toInt();
        QString productName = model->data(index.siblingAtColumn(1)).toString();
        QString price = model->data(index.siblingAtColumn(2)).toString();
        QString productType = model->data(index.siblingAtColumn(3)).toString();
        QString option = model->data(index.siblingAtColumn(4)).toString();
        QStringList strings;
        strings << QString::number(id) << productName << price << productType << option;     // 문자열이 일치하는 항목들의 정보들을 리스트에 넣음

        QList<QStandardItem *> items;
        for (int i = 0; i < 5; ++i) {
            items.append(new QStandardItem(strings.at(i)));                                  // QStandardItem 형의 리스트에 리스트를 삽입
        }
        searchModel->appendRow(items);                                                       // 서치모델에 리스트 내용을 추가
    }
}

void ProductManagerForm::on_modifyPushButton_clicked()                                       /*변경버튼 눌렀을 경우 발생하는 슬롯*/
{
    QModelIndex index = ui->tableView->currentIndex();                                       // 테이블 뷰에서 현재 선택된 인덱스 값을 가져옴
    int id;
    QString productName, price, productType, option;
    id = model->data(index.siblingAtColumn(0)).toInt();
    productName = ui->productNameLineEdit->text();
    price = ui->priceLineEdit->text();
    productType = ui->productTypeLineEdit->text();
    option = ui->optionLineEdit->text();                                                     // 라인에딧에 새롭게 정의된 제품정보들을 변수에 저장
    QSqlQuery query(db);
    query.prepare("UPDATE productManager SET "
                  "ProductName=:productName, Price=:price, ProductType=:productType, Option=:option "
                  "WHERE ID=:id");                                                           // 쿼리문으로 id에 해당하는 행의 제품정보들을 변경된 정보로 수정
    query.bindValue(":productName", productName);
    query.bindValue(":price", price);
    query.bindValue(":productType", productType);
    query.bindValue(":option", option);
    query.bindValue(":id", id);
    query.exec();
    model->select();                                                                         // 모델 선택
    emit remove_item();                                                                      // 시그널 방출
}

void ProductManagerForm::on_addPushButton_clicked()                                          /*추가버튼을 눌렀을 경우 발생하는 슬롯*/
{
    int id = makeId( );                                                                      // 아이디 자동생성
    QString productName = ui->productNameLineEdit->text();
    QString price = ui->priceLineEdit->text();
    QString productType = ui->productTypeLineEdit->text();
    QString option = ui->optionLineEdit->text();                                             // 라인에딧에 정의된 새롭게 추가될 제품정보를 변수에 저장
    QSqlQuery query(db);
    query.prepare("insert into productManager(ID, ProductName, Price, ProductType, Option) "
                  "values (:id, :productName, :price, :productType, :option)");              // 쿼리문을 이용하여 테이블에 데이터를 새롭게 추가
    query.bindValue(":id", id);
    query.bindValue(":productName", productName);
    query.bindValue(":price", price);
    query.bindValue(":productType", productType);
    query.bindValue(":option", option);
    query.exec();
    model->select();                                                                         // 모델 선택
    emit Add_Product_Info(id, productName, price, productType, option);                      // 새롭게 추가된 제품정보를 시그널로 방출
    emit Send_Product_Price(id, price);                                                      // 새롭게 추가된 제품가격을 시그널로 방출
    emit Send_ProductName(id, productName);                                                  // 새롭게 추가된 제품명을 시그널로 방출
}

void ProductManagerForm::on_tableView_clicked(QModelIndex index)                             /*테이블 뷰를 클릭했을 경우 발생하는 슬롯*/
{
    index = ui->tableView->currentIndex();                                                   // 테이블 뷰에서 현재 선택된 인덱스 값을 가져옴
    int id;
    QString productName, price, productType, option;
    id = model->data(index.siblingAtColumn(0)).toInt();
    productName = model->data(index.siblingAtColumn(1)).toString();
    price = model->data(index.siblingAtColumn(2)).toString();
    productType = model->data(index.siblingAtColumn(3)).toString();
    option = model->data(index.siblingAtColumn(4)).toString();                               // 선택된 인덱스 값에 해당하는 제품정보들을 변수에 저장

    ui->idLineEdit->setText(QString::number(id));
    ui->productNameLineEdit->setText(productName);
    ui->priceLineEdit->setText(price);
    ui->productTypeLineEdit->setText(productType);
    ui->optionLineEdit->setText(option);                                                     // 라인에딧에 인덱스 값에 해당하는 제품정보를 표시
}

bool ProductManagerForm::createConnection()                                                  /*데이터베이스 생성하는 함수*/
{
    db = QSqlDatabase::addDatabase("QSQLITE", "productConnection");                          // 데이터베이스 QSQLITE 사용
    db.setDatabaseName("product1.data");                                                     // 데이터베이스 이름 설정
    db.open();                                                                               // 데이터베이스 오픈

    QSqlQuery query(db);                                                                     // 쿼리문으로 데이터베이스에 테이블 생성
    query.exec("CREATE TABLE IF NOT EXISTS productManager(ID INTERGER Primary Key, "
               "ProductName VARCHAR(20) NOT NULL, Price VARCHAR(20), ProductType VARCHAR(20), Option VARCHAR(20));");

    return true;
}
