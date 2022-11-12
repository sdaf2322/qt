#ifndef SHOPMANAGERFORM_H
#define SHOPMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QtSql>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlTableModel>

#include "shopitem.h"
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
    void clear_productTreeWidget();
    void send_product_price(int, QString);

public:
    explicit ShopManagerForm(QWidget *parent, ClientManagerForm*, ProductManagerForm*);
    ~ShopManagerForm();

    void LoadData();
    bool createConnection();

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_searchPushButton_clicked();

    void get_Product_Info(int, QString, QString, QString, QString);
    void productTreeWidget_update(int, QString, QString, QString, QString);
    void on_productTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void removeOption();
    void on_searchProductPushButton_clicked();
    void on_searchProductTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void productTreeWidget_clear();
    void on_tableView_clicked(QModelIndex index);

    void get_Client_Name(int id, QString name);
    void get_Product_Price(int id, QString price);
    void get_ProductName(int id, QString name);

private:
    int makeId();
    QList<int> clientIDList;
    QList<int> productIDList;
    QMap<int, ShopItem*> shopList;
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

public:
    const QMap<int, ShopItem*>& getList()
    {
        return shopList;
    }

};
#endif // SHOPMANAGERFORM_H
