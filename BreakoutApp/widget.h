#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QMediaPlayer>

class QLabel;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void keyPressEvent(QKeyEvent*);
    void mouseMoveEvent(QMouseEvent* e);
    static const int MOVE_SPEED = 3;
    void timerEvent(QTimerEvent*);
    void moveObjects();
    void checkCollision();
private:
    static const int NO_OF_BRICKS = 30;

    QLabel* ball;
    QLabel* paddle;
    QLabel* bricks[NO_OF_BRICKS];
    int timerld;
    int xDir, yDir;
    QMediaPlayer* bgPlayer;
    QMediaPlayer* effectPlayer;
};
#endif // WIDGET_H
