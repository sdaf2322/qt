#include "widget.h"

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{

    setWindowTitle("OpenGL Point");
    resize(600, 600);
}

Widget::~Widget()
{
}

void Widget::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Widget::resizeGL(int w, int h)
{
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    glVertex2f(0.0, 0.5);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, 0.5);
    glEnd();

    glFlush();
}
//void Widget::timerFunction()
//{
//    if(rectPosX > windowWidth-rectSize || rectPosX < -windowWidth)
//        xStep = -xStep;

//    if(rectPosY > windowHeight || rectPosY < -windowHeight + rectSize)
//        yStep = -yStep;

//    rectPosX += xStep;
//    rectPosY += yStep;

//    if(rectPosX > (windowWidth - rectSize + xStep))
//        rectPosX = windowWidth - rectSize - 1;
//    else if(rectPosX < -(windowWidth + xStep))
//        rectPosX = -windowWidth - 1;

//    if(rectPosY > (windowHeight + yStep))
//        rectPosY = windowHeight - 1;
//    else if(rectPosY < -(windowHeight - rectSize + yStep))
//        rectPosY = -windowHeight + rectSize - 1;

//    repaint();
//}
