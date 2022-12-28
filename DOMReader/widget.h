#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class DOMReader : public QWidget
{
    Q_OBJECT

public:
    DOMReader(QWidget *parent = nullptr);
    ~DOMReader();
};
#endif // WIDGET_H
