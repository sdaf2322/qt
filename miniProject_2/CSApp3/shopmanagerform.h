#ifndef SHOPMANAGERFORM_H
#define SHOPMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QtSql>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlTableModel>
#include "clientmanagerform.h"
#include "productmanagerform.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ShopManagerForm;
}

class ShopManagerForm : public QWidget
{
    Q_OBJECT

signals:
    void clear_productTreeWidget();                                                                         // productTreeWidget의 내용을 지우기 위한 시그널
    void send_product_price(int, QString);                                                                  // 제품가격에 대한 정보를 보내는 시그널

public:
    explicit ShopManagerForm(QWidget *parent, ClientManagerForm*, ProductManagerForm*);
    ~ShopManagerForm();

    void LoadData();                                                                                        // 프로그램 실행시 데이터를 불러오는 함수
    bool createConnection();                                                                                // 데이터베이스를 생성하는 함수

private slots:
    void showContextMenu(const QPoint &);                                                                   // 마우스 우측키를 눌렀을 경우 메뉴 생성
    void removeItem();
    void on_addPushButton_clicked();                                                                        // 추가버튼을 눌렀을 경우 발생하는 슬롯
    void on_searchPushButton_clicked();                                                                     // 서치버튼을 눌렀을 경우 발생하는 슬롯

    void get_Product_Info(int, QString, QString, QString, QString);                                         // 제품정보를 받아 테이블 뷰에 입력하는 슬롯
    void on_productTableView_clicked(QModelIndex index);                                                    // productTableView를 클릭할 경우 발생하는 슬롯
    void removeOption();                                                                                    //
    void on_searchProductPushButton_clicked();                                                              // 제품서치 버튼을 눌렀을 경우
    void on_searchProductTableView_clicked(QModelIndex index);                                              // searchProductTableView를 클릭할 경우 발생하는 슬롯
    void productModel_clear();                                                                              // productModel의 내용을 지우는 슬롯
    void on_tableView_clicked(QModelIndex index);                                                           // 테이블 뷰를 클릭했을 경우 발생하는 슬롯

    void get_Client_Name(int id, QString name);                                                             // 고객명에 대한 정보를 받는 슬롯
    void get_Product_Price(int id, QString price);                                                          // 제품가격에 대한 정보를 받는 슬롯
    void get_ProductName(int id, QString name);                                                             // 제품명에 대한 정보를 받는 슬롯

private:
    int makeId();                                                                                           // 주문정보를 추가할 경우 아이디가 자동으로 생성되는 함수
    QList<int> clientIDList;
    QList<int> productIDList;
    QStringList list;

    ClientManagerForm* cm;
    ProductManagerForm* pm;

    Ui::ShopManagerForm *ui;
    QMenu* menu;
    QSqlDatabase db;
    QSqlTableModel* model;

    QMap<int, QString> client_name;
    QMap<int, QString> product_price;
    QMap<int, QString> product_name;
    QList<QString> stringlist;
    QStringList header;
    QStringList p_header;
    QStringList sp_header;
    QStandardItemModel* searchModel;
    QStandardItemModel* productModel;
    QStandardItemModel* searchProductModel;

};
#endif // SHOPMANAGERFORM_H
