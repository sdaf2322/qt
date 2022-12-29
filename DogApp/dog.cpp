#include "dog.h"

Dog::Dog(QObject* parent) : QObject(parent){
    qInfo() << "Constructor";
}

void Dog::initTestCase(){
    qInfo() << "initTestCase";
}

void Dog::init(){
    qInfo() << "init";
}

void Dog::cleanup()
{
    qInfo() << "cleanup";
}

void Dog::cleanupTestCase()
{
    qInfo() << "cleanupTestCase";
}

void Dog::bark(){
    qInfo() << "bark bark bakr";
}

void Dog::rollover(){
    qInfo() << "*rolls*";
}

void Dog::simple(){
    QString str1 = QLatin1String("This is a test string");
    QString str2 = QLatin1String("This is a test string");
    QCOMPARE(str1.localeAwareCompare(str2), 0);
    QBENCHMARK_ONCE{
        str1.localeAwareCompare(str2);
    }
}
