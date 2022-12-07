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
    clientForm = new ClientManagerForm(this);
    productForm = new ProductManagerForm(this);
    shopForm = new ShopManagerForm(this, clientForm, productForm);

    connect(clientForm, SIGNAL(destroyed()), clientForm, SLOT(deleteLater()));
    connect(clientForm, SIGNAL(Send_Client_Name(int,QString)), shopForm, SLOT(get_Client_Name(int, QString)));
    clientForm->setWindowTitle(tr("Client Info"));

    connect(productForm, SIGNAL(destroyed()),
            productForm, SLOT(deleteLater()));

    connect(productForm, SIGNAL(Add_Product_Info(int, QString, QString, QString, QString)), shopForm, SLOT(get_Product_Info(int, QString, QString, QString, QString)));
    connect(productForm, SIGNAL(remove_item()), shopForm, SLOT(productModel_clear()));
    connect(shopForm, SIGNAL(clear_productTableView()), productForm, SLOT(signal_emit()));

    connect(productForm, SIGNAL(Send_Product_Price(int,QString)), shopForm, SLOT(get_Product_Price(int,QString)));
    connect(productForm, SIGNAL(Send_ProductName(int,QString)), shopForm, SLOT(get_ProductName(int,QString)));

    connect(productForm, SIGNAL(Remove_productTableView_Item()), shopForm, SLOT(removeOption()));
    productForm->setWindowTitle(tr("Product Info"));

    connect(shopForm, SIGNAL(destroyed()),
            shopForm, SLOT(deleteLater()));
    shopForm->setWindowTitle(tr("Shop Info"));

    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(shopForm);
    ui->mdiArea->addSubWindow(serverForm);
    ui->mdiArea->setActiveSubWindow(cw);

    connect(clientForm, SIGNAL(clientAdded(int,QString)), serverForm, SLOT(addClient(int,QString)));
    productForm->LoadData();
    clientForm->LoadData();
    shopForm->LoadData();
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


