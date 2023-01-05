#include "servcer.h"
#include "ui_servcer.h"

Servcer::Servcer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Servcer)
{
    ui->setupUi(this);
}

Servcer::~Servcer()
{
    delete ui;
}

