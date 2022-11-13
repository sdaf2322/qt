#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>
#include <QStandardItemModel>

#include "clientitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ClientManagerForm;
}

class ClientManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManagerForm(QWidget *parent = nullptr);
    ~ClientManagerForm();

    void LoadData();
    bool createConnection();

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_tableView_clicked(QModelIndex index);
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();

private:
    int makeId();

    QMap<int, ClientItem*> clientList;
    Ui::ClientManagerForm *ui;
    QMenu* menu;
    QSqlDatabase db;
    QSqlTableModel* model;
    QStandardItemModel* searchModel;
    QStringList header;

signals:
    void clientAdded(int, QString);
    void Send_Client_Name(int, QString);

public:
    const QMap<int, ClientItem*>& getList()
    {
        return clientList;
    }
};

#endif // CLIENTMANAGERFORM_H
