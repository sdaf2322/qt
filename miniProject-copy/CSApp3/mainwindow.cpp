#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clientmanagerform.h"
#include "productmanagerform.h"
#include "shopmanagerform.h"
#include "serverform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serverForm = new ServerForm(this);
    serverForm->setWindowTitle(tr("Chatting Server"));
    qDebug("%s %d", __FILE__, __LINE__);
    clientForm = new ClientManagerForm(this);
    productForm = new ProductManagerForm(this);
    shopForm = new ShopManagerForm(this, clientForm, productForm);

    connect(clientForm, SIGNAL(destroyed()), clientForm, SLOT(deleteLater()));
    connect(clientForm, SIGNAL(Send_Client_Name(int,QString)), shopForm, SLOT(get_Client_Name(int, QString)));
    clientForm->setWindowTitle(tr("Client Info"));
    qDebug("%s %d", __FILE__, __LINE__);

    connect(productForm, SIGNAL(destroyed()),
            productForm, SLOT(deleteLater()));
    connect(productForm, SIGNAL(Add_Product_Info(int, QString, QString, QString, QString)), shopForm, SLOT(get_Product_Info(int, QString, QString, QString, QString)));
    connect(productForm, SIGNAL(remove_item()), shopForm, SLOT(productTreeWidget_clear()));

    connect(shopForm, SIGNAL(clear_productTreeWidget()), productForm, SLOT(signal_emit()));
    connect(productForm, SIGNAL(Send_Product_Price(int,QString)), shopForm, SLOT(get_Product_Price(int,QString)));
    connect(productForm, SIGNAL(Send_ProductName(int,QString)), shopForm, SLOT(get_ProductName(int,QString)));

    connect(productForm, SIGNAL(Send_Product_Info(int, QString, QString, QString, QString)), shopForm, SLOT(productTreeWidget_update(int, QString, QString, QString, QString)));
    qDebug("%s %d", __FILE__, __LINE__);
    connect(productForm, SIGNAL(Remove_productTreeWidget_Item()), shopForm, SLOT(removeOption()));
    qDebug("%s %d", __FILE__, __LINE__);
    productForm->setWindowTitle(tr("Product Info"));
    qDebug("%s %d", __FILE__, __LINE__);

    connect(shopForm, SIGNAL(destroyed()),
            shopForm, SLOT(deleteLater()));
    qDebug("%s %d", __FILE__, __LINE__);
    shopForm->setWindowTitle(tr("Shop Info"));
    qDebug("%s %d", __FILE__, __LINE__);

    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(shopForm);
    ui->mdiArea->addSubWindow(serverForm);
    ui->mdiArea->setActiveSubWindow(cw);
    qDebug("%s %d", __FILE__, __LINE__);

    connect(clientForm, SIGNAL(clientAdded(int,QString)), serverForm, SLOT(addClient(int,QString)));
    productForm->LoadData();
    qDebug("%s %d", __FILE__, __LINE__);
    clientForm->LoadData();
    qDebug("%s %d", __FILE__, __LINE__);
    shopForm->LoadData();
    qDebug("%s %d", __FILE__, __LINE__);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClient_triggered()
{
    if(clientForm != nullptr) {
        clientForm->setFocus();
    }
}

void MainWindow::on_actionProduct_triggered()
{
    if(productForm != nullptr) {
        productForm->setFocus();
    }
}

void MainWindow::on_actionShop_triggered()
{
    if(shopForm != nullptr) {
        shopForm->setFocus();
    }
}

void MainWindow::on_actionChatting_triggered()
{
    if(serverForm != nullptr) {
        serverForm->setFocus();
    }
}


