#include <QtTest>

// add necessary includes here

class cat : public QObject
{
    Q_OBJECT

public:
    explicit cat(QObject* parent = nullptr);
    void test();

private slots:
    void meow();
    void sleep();
    void speak(QString value);
};

void cat::test_case1()
{

}

QTEST_APPLESS_MAIN(cat)

#include "tst_cat.moc"
