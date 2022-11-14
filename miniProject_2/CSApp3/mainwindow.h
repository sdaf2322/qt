#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManagerForm;
class ProductManagerForm;
class ShopManagerForm;
class ServerForm;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionClient_triggered();                                           // 고객액션을 누를 경우 발생하는 슬롯
    void on_actionProduct_triggered();                                          // 제품액션을 누를 경우 발생하는 슬롯
    void on_actionShop_triggered();                                             // 주문액션을 누를 경우 발생하는 슬롯
    void on_actionChatting_triggered();                                         // 채팅액션을 누를 경우 발생하는 슬롯

private:
    Ui::MainWindow *ui;
    ClientManagerForm *clientForm;
    ProductManagerForm *productForm;
    ShopManagerForm* shopForm;
    ServerForm* serverForm;
};
#endif // MAINWINDOW_H
