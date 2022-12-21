#include "shopmanagerform.h"
#include "ui_shopmanagerform.h"
#include "shopitem.h"

#include <QFile>
#include <QMenu>

ShopManagerForm::ShopManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShopManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 540 << 400;                    // vertical layout과 입력창 비율
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));                     // 종료 액션
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));         // 삭제 누르면 항목 삭제

    menu = new QMenu;                                                       // 마우스 우측 버튼 눌렀을 때 메뉴 뜸
    menu->addAction(removeAction);                                          // 메뉴 만들어서 추가
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);            // 사용자가 정의한 메뉴를 정책으로 함
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    // 트리위젯에서 오른쪽 마우스 키 누르면 팝업창 뜸

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
    // searchLineEdit에서 엔터키를 눌렀을 경우 서치트리위젯에서 검색한 항목과 일치하는 항목을 가져옴

    QFile file("shoplist.txt");               // 파일 불러오기
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            ShopItem* s = new ShopItem(id, row[1].toInt(), row[2].toInt(), row[3], row[4], row[5]);
            ui->treeWidget->addTopLevelItem(s);
            shopList.insert(id, s);
        }
    }
    file.close( );
}

ShopManagerForm::~ShopManagerForm()     // 파일 저장
{
    delete ui;

    QFile file("shoplist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : shopList) {
        ShopItem* s = v;
        out << s->id() << ", " << s->getClientID() << ", ";
        out << s->getProductID() << ", ";
        out << s->getShopdate() << ", ";
        out << s->getQuantity() << ", ";
        out << s->getOption() << "\n";
    }
    file.close( );
}

int ShopManagerForm::makeId( )            // id 자동생성
{
    if(shopList.size( ) == 0) {
        return 100;
    } else {
        auto id = shopList.lastKey();
        return ++id;
    }
}

void ShopManagerForm::removeItem()        // 아이템 제거
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();      // 트리위젯에서 현재 아이템 가져옴
    if(item != nullptr) {                                       // 아이템 내용이 있으면
        shopList.remove(item->text(0).toInt());               // 아이템의 아이디 삭제, 항목 다 삭제되는 듯
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));    // 가장 탑레벨의 인덱스 값을 반환
        ui->treeWidget->update();                               // 트리위젯 갱신
    }
}

void ShopManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);         // 전역좌표로 만들어서 가져옴
    menu->exec(globalPos);                                       // 마우스 커서 좌표에서 메뉴를 여는 것 같음
}

void ShopManagerForm::on_searchPushButton_clicked()
{
    ui->searchTreeWidget->clear();                                // searchTreeWidget 창 비우기

    int i = ui->searchComboBox->currentIndex();                   // sarchComboBox의 인덱스 값 가져옴
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    // i가 0이 아니면 검색의 대소문자 구분하거나 검색어가 항목에 포함, 0이면 검색의 대소문자 구분???

        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);
        // 트리위젯에서 searchLineEdit에서 입력한 글자와 같은 항목을 가져옴

        foreach(auto i, items)
        {
            ShopItem* s = static_cast<ShopItem*>(i);        // 검색으로 찾은 항목을 매칭시킴
            int id = s->id();                                   // 항목의 아이디를 가져옴
            int clientID = s->getClientID();                        // 이름
            int productID = s->getProductID();               // 전화번호
            QString shopDate = s->getShopdate();
            QString quantity = s->getQuantity();
            QString option = s->getOption();
            ShopItem* item = new ShopItem(id, clientID, productID, shopDate, quantity, option);       // 검색으로 찾은 항목을 다시한번 매칭
            ui->searchTreeWidget->addTopLevelItem(item);        // 매칭한 아이템을 서치트리위젯의 가장 위쪽으로 옮김
        }
}

void ShopManagerForm::on_addPushButton_clicked()                  // 커넥트 없어도 자동으로 연결됨. qt문법
{
    int clientID, productID;
    QString shopDate, quantity, option;                                // 이름, 전화번호, 주소
    int id = makeId( );                                             // 아이디 가져옴
    clientID = ui->clientIdLineEdit->text().toInt();                                // 라인에딧에 이름쓴거 name에 저장
    productID = ui->productIdLineEdit->text().toInt();                       // 라인에딧에 번호쓴거 number에 저장
    shopDate = ui->shopDateLineEdit->text();
    quantity = ui->quantityLineEdit->text();
    option = ui->optionLineEdit->text();

    ShopItem* s = new ShopItem(id, clientID, productID, shopDate, quantity, option);      // 각각의 값 매칭
    shopList.insert(id, s);                                       // clientList에 아이디에 해당하는 값 삽입
    ui->treeWidget->addTopLevelItem(s);                             // 탑레벨로 옮김

}

void ShopManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)    // 커넥트 없어도 자동으로 연결됨. qt문법
{
    Q_UNUSED(column);                                           // 경고메세지 안띄우게 하는 것 같음
    ui->idLineEdit->setText(item->text(0));                     // 아이디 재설정
    ui->clientIdLineEdit->setText(item->text(1));                   // 이름 재설정
    ui->productIdLineEdit->setText(item->text(2));            // 전화번호 재설정
    ui->shopDateLineEdit->setText(item->text(3));
    ui->quantityLineEdit->setText(item->text(4));
    ui->optionLineEdit->setText(item->text(5));  // 주소 재수정
}
