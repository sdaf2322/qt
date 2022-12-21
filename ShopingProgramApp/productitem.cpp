#include "productitem.h"

#include <QLabel>
#include <iostream>

using namespace std;

ProductItem::ProductItem(int id, QString productName, QString price, QString productType, QString option)
{
    setText(0, QString::number(id));
    setText(1, productName);
    setText(2, price);
    setText(3, productType);
    setText(4, option);
}

QString ProductItem::getProductName() const
{
    return text(1);
}

void ProductItem::setProductName(QString& productName)
{
    setText(1, productName);
}

QString ProductItem::getPrice() const
{
    return text(2);
}

void ProductItem::setPrice(QString& price)
{
    setText(2, price);
}

QString ProductItem::getProductType() const
{
    return text(3);
}

void ProductItem::setProductType(QString& productType)
{
    setText(3, productType);
}

QString ProductItem::getOption() const
{
    return text(4);
}

void ProductItem::setOption(QString& option)
{
    setText(4, option);
}

int ProductItem::id() const
{
    return text(0).toInt();
}

// Define copy assignment operator.
bool ProductItem::operator==(const ProductItem &other) const {
    return (this->text(1) == other.text(1));
}
