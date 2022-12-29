#include "widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QObject(parent)
{
    age = 0;
}

void Widget::setAge(int value){
    age = value;
}

void Widget::testFail()
{
    QFAIL("NO REASON JUST FAIL");
}

void Widget::testAge(){
    if(!age) QFAIL("Age is not set!");
}

void Widget::testWidget(){
    int value = 45;

    QVERIFY(age > 0 && age < 100);

    if(age > 40) QWARN("Age is over 40");
    if(age < 21) QFAIL("Must be an adult");

    QCOMPARE(age, value);
}


