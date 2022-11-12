#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include "productitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ProductManagerForm;
}

class ProductManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManagerForm(QWidget *parent = nullptr);
    ~ProductManagerForm();

    void LoadData();
    bool createConnection();

signals:
    void Add_Product_Info(int, QString, QString, QString, QString);
    void Send_Product_Info(int, QString, QString, QString, QString);
    void Remove_productTreeWidget_Item();
    void remove_item();
    void Send_Product_Price(int, QString);
    void Send_ProductName(int, QString);

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_tableView_clicked(QModelIndex index);
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();
    void signal_emit();

private:
    int makeId();

    QMap<int, ProductItem*> productList;

    Ui::ProductManagerForm *ui;
    QMenu* menu;
    QSqlDatabase db;
    QSqlQuery query;
    QSqlTableModel* model;

public:
    const QMap<int, ProductItem*>& getList()
    {
        return productList;
    }
};

#endif // PRODUCTMANAGERFORM_H
