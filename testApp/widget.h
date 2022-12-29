#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>
#include <QtTest/QtTest>
#include <QTest>

class Cat : public QObject
{
    Q_OBJECT

public:
    explicit Cat(QObject* parent = nullptr);
    void test();

private slots:
    void meow();
    void sleep();
    void speak(QString value);
};
#endif // WIDGET_H
