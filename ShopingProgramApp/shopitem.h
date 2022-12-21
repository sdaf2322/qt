#ifndef SHOPITEM_H
#define SHOPITEM_H

#include "clientmanagerform.h"
#include "productmanagerform.h"

#include <QTreeWidgetItem>

class ShopItem : public QTreeWidgetItem
{
public:
    explicit ShopItem(int id = 0, int = 0, int = 0, QString = "", QString = "", QString = "");

    QString getShopdate() const;
    QString getQuantity() const;	// 구매날짜 리턴하는 함수													// 구매수량 리턴하는 함수
    QString getOption() const;													// 주문 옵션 리턴하는 함수														// 주문번호 리턴하는 함수
    int getClientID() const;													// 고객ID 리턴하는 함수
    int getProductID() const;
    int id() const;                                             // 상품번호 리턴하는 함수
    void setShopdate(QString&);
    void setQuantity(QString&);// 구매날짜 수정하는 함수									// 구매수량 리턴하는 함수
    void setOption(QString&);											// 주문 옵션 리턴하는 함수

    bool operator==(const ShopItem &other) const;

private:
    ClientManagerForm* cm;
    ProductManagerForm* pm;
};

#endif // SHOPITEM_H
