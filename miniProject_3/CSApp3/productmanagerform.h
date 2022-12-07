#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStandardItemModel>

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

    void LoadData();                                                            // 프로그램 실행시 데이터를 불러오는 함수
    bool createConnection();                                                    // 데이터베이스 생성하는 함수

signals:
    void Add_Product_Info(int, QString, QString, QString, QString);             // 제품정보를 보내는 시그널
    void Remove_productTableView_Item();                                       // 트리위젯의 항목을 제거할 경우 발생하는 시그널
    void remove_item();                                                         //
    void Send_Product_Price(int, QString);                                      // 제품의 가격에 대한 정보를 보내는 시그널
    void Send_ProductName(int, QString);                                        // 제품명에 대한 정보를 보내는 시그널

private slots:
    void on_tableView_clicked(QModelIndex index);                               // 테이블 뷰를 클릭했을 경우 발생하는 슬롯
    void showContextMenu(const QPoint &);                                       // 마우스 우측키를 눌렀을 경우 메뉴 생성
    void removeItem();
    void on_addPushButton_clicked();                                            // 추가버튼을 눌렀을 경우 발생하는 슬롯
    void on_modifyPushButton_clicked();                                         // 변경버튼을 눌렀을 경우 발생하는 슬롯
    void on_searchPushButton_clicked();                                         // 서치버튼을 눌렀을 경우 발생하는 슬롯
    void signal_emit();                                                         // 시그널을 발생시키는 슬롯

private:
    int makeId();                                                               // 제품정보 추가시 아이디 자동생성하는 함수

    Ui::ProductManagerForm *ui;
    QMenu* menu;
    QSqlDatabase db;
    QSqlTableModel* model;
    QStandardItemModel* searchModel;
    QStringList header;

};

#endif // PRODUCTMANAGERFORM_H
