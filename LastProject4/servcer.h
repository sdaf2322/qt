#ifndef SERVCER_H
#define SERVCER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Servcer; }
QT_END_NAMESPACE

class Servcer : public QWidget
{
    Q_OBJECT

public:
    Servcer(QWidget *parent = nullptr);
    ~Servcer();

private:
    Ui::Servcer *ui;
};
#endif // SERVCER_H
