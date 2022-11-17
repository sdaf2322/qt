#include "widget.h"

#include <QApplication>
#include <QLabel>
#include <QAudioOutput>
#include <QFileInfo>

#define WIDTH 50
#define HEIGHT 12
#define SCR_WIDTH 300
#define SCR_HEIGTH 400

Widget::Widget(QWidget *parent)
    : QWidget(parent), xDir(1), yDir(-1)
{
    ball = new QLabel(this);
    ball->setGeometry(SCR_WIDTH*0.8, SCR_HEIGTH*0.875, 10, 10);
    ball->setStyleSheet("QLabel{background-color:red; border-radius: 5px; }");

    paddle = new QLabel(this);
    paddle->setGeometry(SCR_WIDTH*0.7, SCR_HEIGTH*0.9, WIDTH, HEIGHT);
    paddle->setStyleSheet("QLabel{background-color:blue; }");

    for(int y = 0, i = 0; y < 5; y++)
    {
        for(int x = 0; x < 6; x++, i++)
        {
            bricks[i] = new QLabel(this);
            bricks[i]->setStyleSheet("QLabel{background-color:cyan;"
                                     "border:1px solid black}");
            bricks[i]->setGeometry(x*WIDTH, y*HEIGHT+30, WIDTH, HEIGHT);
        }
    }
    resize(SCR_WIDTH, SCR_HEIGTH);

    setMouseTracking(true);

    timerld = startTimer(10);


    QAudioOutput *bgAudioOutput = new QAudioOutput;
        bgAudioOutput->setVolume(10);

        bgPlayer = new QMediaPlayer( );
        bgPlayer->setAudioOutput(bgAudioOutput);
        bgPlayer->setLoops(QMediaPlayer::Infinite);         /* 무한 반복 */
        bgPlayer->setSource(QUrl::fromLocalFile(QFileInfo("effect2.wav").absoluteFilePath( )));
        bgPlayer->play( );
        /* 효과음 출력을 위한 플레이어 */
        QAudioOutput *bgEffectOutput = new QAudioOutput;
        bgEffectOutput->setVolume(200);

        effectPlayer = new QMediaPlayer( );
        effectPlayer->setAudioOutput(bgEffectOutput);
        effectPlayer->setLoops(QMediaPlayer::Once);
        effectPlayer->setSource(QUrl::fromLocalFile(QFileInfo("effect1.wav").absoluteFilePath( )));

}

void Widget::keyPressEvent(QKeyEvent* e)
{
    switch(e->key())
    {
    case Qt::Key_Left:
        paddle->move(paddle->x()-MOVE_SPEED, paddle->y());
        break;
    case Qt::Key_Right:
        paddle->move(paddle->x()+MOVE_SPEED, paddle->y());
        break;
    case Qt::Key_Escape:
        qApp->exit();
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}

void Widget::mouseMoveEvent(QMouseEvent* e)
{
    int x = e->pos().x();
    x = (x < 0)?0:(x+WIDTH>width())?width()-WIDTH:x;
    paddle->move(x, paddle->y());
}

void Widget::timerEvent(QTimerEvent* e)
{
    Q_UNUSED(e);
    moveObjects();
    checkCollision();

}

void Widget::moveObjects()
{
    ball->move(ball->x() + xDir, ball->y() + yDir);

    if((ball->x() <= 0) || (ball->x()+10 >= SCR_WIDTH))
    {
        xDir *= -1;
    }

    if(ball->y() <= 0)
        yDir = 1;
}

void Widget::checkCollision()
{
    if(ball->geometry().bottom() > height())
    {
        killTimer(timerld);
        qDebug("Game lost");
    }

    int j = 0;
    for(int i = 0; i < NO_OF_BRICKS; i++)
        if(bricks[i]->isHidden())j++;

    if(j==NO_OF_BRICKS)
    {
        killTimer(timerld);
        qDebug("Victory");
    }

    if((ball->geometry()).intersects(paddle->geometry()))
    {
        int paddleLPos = paddle->geometry().left();
        int ballLpos = ball->geometry().left();
        int first = paddleLPos + 8;
        int second = paddleLPos + 16;
        int third = paddleLPos + 24;
        int fourth = paddleLPos + 32;

        if(ballLpos < first) xDir = -1; yDir = -1;
        if(ballLpos >= first && ballLpos < second) xDir = -1; yDir *= -1;
        if(ballLpos >= second && ballLpos < third) xDir = 0; yDir = -1;
        if(ballLpos >= third && ballLpos < fourth) xDir = 1; yDir *= -1;
        if(ballLpos > fourth) xDir = 1; yDir = -1;
    }

    for(int i = 0; i < NO_OF_BRICKS; i++)
    {
        if((ball->geometry()).intersects(bricks[i]->geometry()))
        {
            int ballLeft = ball->geometry().left();
            int ballHeight = ball->geometry().height();
            int ballWidth = ball->geometry().width();
            int ballTop = ball->geometry().top();

            QPoint pointRight(ballLeft + ballWidth + 1, ballTop);
            QPoint pointLeft(ballLeft - 1, ballTop);
            QPoint pointTop(ballLeft, ballTop - 1);
            QPoint pointBottom(ballLeft, ballTop + ballHeight + 1);
            if(!bricks[i]->isHidden())
            {
                if(bricks[i]->geometry().contains(pointRight))
                    xDir = -1;
                else if(bricks[i]->geometry().contains(pointLeft))
                    xDir = 1;
                if(bricks[i]->geometry().contains(pointTop))
                    yDir = 1;
                else if(bricks[i]->geometry().contains(pointBottom))
                    yDir = -1;
                bricks[i]->setHidden(true);
                effectPlayer->play();
            }
        }
    }
}



Widget::~Widget()
{
    delete ball;
    delete paddle;

    for(int i = 0; i < NO_OF_BRICKS; i++)
    {
        delete bricks[i];
    }
}

