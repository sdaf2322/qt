#include "shopitem.h"

#include <QLabel>
#include <iostream>

using namespace std;

ShopItem::ShopItem(int id, int clientID, int productID, QString shopDate, QString quantity, QString option)
{
    setText(0, QString::number(id));
    setText(1, (cm->search(clientID))->getName());
    setText(2, (pm->search(productID))->getProductName());
    setText(3, option);
    setText(4, quantity);
    setText(5, shopDate);
    setText(6, QString::number(productID));
    setText(7, (pm->search(productID))->getPrice());
}

int ShopItem::getClientID() const
{
    return text(1).toInt();
}

int ShopItem::getProductID() const
{
    return text(2).toInt();
}

QString ShopItem::getShopdate() const
{
    return text(3);
}

QString ShopItem::getQuantity() const
{
    return text(4);
}

QString ShopItem::getOption() const
{
    return text(5);
}

void ShopItem::setShopdate(QString& shopDate)
{
    setText(3, shopDate);
}

void ShopItem::setQuantity(QString& quantity)
{
    setText(4, quantity);
}

void ShopItem::setOption(QString& option)
{
    setText(5, option);
}

int ShopItem::id() const
{
    return text(0).toInt();
}

// Define copy assignment operator.
bool ShopItem::operator==(const ShopItem &other) const {
    return (this->text(1) == other.text(1));
}
