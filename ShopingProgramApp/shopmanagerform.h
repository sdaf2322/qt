#ifndef SHOPMANAGERFORM_H
#define SHOPMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "shopitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ShopManagerForm;
}

class ShopManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShopManagerForm(QWidget *parent = nullptr);
    ~ShopManagerForm();

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_searchPushButton_clicked();

private:
    int makeId();

    QMap<int, ShopItem*> shopList;
    Ui::ShopManagerForm *ui;
    QMenu* menu;
};
#endif // SHOPMANAGERFORM_H
