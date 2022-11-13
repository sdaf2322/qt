#include "shopitem.h"

#include <QLabel>
#include <iostream>

using namespace std;

ShopItem::ShopItem(int id, int clientID, QString cname, int productID, QString pname, QString shopDate, QString quantity, QString option, QString price)
{
    setText(0, QString::number(id));
    setText(1, cname);
    setText(2, pname);
    setText(3, option);
    setText(4, quantity);
    setText(5, shopDate);
    setText(6, QString::number(clientID));
    setText(7, QString::number(productID));
    setText(8, QString::number(price.toInt() * quantity.toInt()));
}

int ShopItem::getClientID() const
{
    return text(6).toInt();
}

int ShopItem::getProductID() const
{
    return text(7).toInt();
}

QString ShopItem::getShopdate() const
{
    return text(5);
}

QString ShopItem::getQuantity() const
{
    return text(4);
}

QString ShopItem::getOption() const
{
    return text(3);
}

void ShopItem::setShopdate(QString& shopDate)
{
    setText(5, shopDate);
}

void ShopItem::setQuantity(QString& quantity)
{
    setText(4, quantity);
}

void ShopItem::setOption(QString& option)
{
    setText(3, option);
}

int ShopItem::id() const
{
    return text(0).toInt();
}

// Define copy assignment operator.
bool ShopItem::operator==(const ShopItem &other) const {
    return (this->text(1) == other.text(1));
}
