#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerform.h"
#include "productmanagerform.h"
#include "shopmanagerform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientForm = new ClientManagerForm(this);               // ClientManagerForm 클래스 정의
    clientForm->setWindowTitle(tr("Client Info"));          // 윈도우 타이틀 설정
    connect(clientForm, SIGNAL(destroyed()),                //
            clientForm, SLOT(deleteLater()));

    productForm = new ProductManagerForm(this);               // ClientManagerForm 클래스 정의
    productForm->setWindowTitle(tr("Product Info"));          // 윈도우 타이틀 설정
    connect(productForm, SIGNAL(destroyed()),                //
            productForm, SLOT(deleteLater()));

    shopForm = new ShopManagerForm(this);               // ClientManagerForm 클래스 정의
    shopForm->setWindowTitle(tr("Shop Info"));          // 윈도우 타이틀 설정
    connect(shopForm, SIGNAL(destroyed()),                //
            shopForm, SLOT(deleteLater()));

    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(shopForm);

    ui->mdiArea->setActiveSubWindow(cw);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClient_triggered()            // 액션클라이언트 눌렀을 경우
{
    if(clientForm != nullptr) {
        clientForm->setFocus();                         // 고객 페이지로 넘어가는 듯
    }
}

void MainWindow::on_actionProduct_triggered()            // 액션클라이언트 눌렀을 경우
{
    if(productForm != nullptr) {
        productForm->setFocus();                         // 고객 페이지로 넘어가는 듯
    }
}

void MainWindow::on_actionShop_triggered()            // 액션클라이언트 눌렀을 경우
{
    if(shopForm != nullptr) {
        shopForm->setFocus();                         // 고객 페이지로 넘어가는 듯
    }
}
