#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <QWidget>

class Breakout : public QWidget
{
    Q_OBJECT
protected:
    void moveObjects();
    void checkCollision();
private:

signals:

};

#endif // BREAKOUT_H
