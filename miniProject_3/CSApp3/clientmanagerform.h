#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QHash>
#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>
#include <QStandardItemModel>

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

    void LoadData();                                                    // 프로그램 실행시 데이터를 불러오는 함수
    bool createConnection();                                            // 데이터베이스를 생성하는 함수

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_tableView_clicked(QModelIndex index);                       // 테이블 뷰를 선택했을 경우 발생하는 슬롯
    void showContextMenu(const QPoint &);                               // 마우스 우측키를 눌렀을 때 보이는 메뉴
    void removeItem();
    void on_addPushButton_clicked();                                    // 추가 버튼 눌렀을 경우 발생하는 슬롯
    void on_modifyPushButton_clicked();                                 // 변경 버튼을 눌렀을 경우 발생하는 슬롯
    void on_searchPushButton_clicked();                                 // 서치버튼을 눌렀을 경우 발생하는 슬롯

private:
    int makeId();                                                       // 항목이 추가될 때 아이디를 자동으로 생성하는 함수

    Ui::ClientManagerForm *ui;
    QMenu* menu;
    QSqlDatabase db;
    QSqlTableModel* model;
    QStandardItemModel* searchModel;
    QStringList header;

signals:
    void clientAdded(int, QString);                                     // 고객정보가 추가될 때 발생하는 시그널
    void Send_Client_Name(int, QString);                                // 고객 아이디와 고객명을 보내는 시그널

};

#endif // CLIENTMANAGERFORM_H
