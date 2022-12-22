#include "widget.h"
#include "qtglut.h"
Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setWindowTitle("OpenGL Animation");
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
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-1, 1, -1, 1, -1, 1);
}

void Widget::paintGL()
{
//    glClear(GL_COLOR_BUFFER_BIT);
//    glPushMatrix();

//    glRotatef(xAngle, 1.0f, 0.0f, 0.0f);
//    glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
//    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

//    glutWireTeapot(0.3);

//    glPushMatrix();
//    glTranslatef(-0.6, -0.6, 0.0);
//    glutWireCube(0.4);
//    glPopMatrix();


//    glPushMatrix();
//    glTranslatef(-0.6, -0.6, 0.0);
//    glutWireSphere(0.3, 20, 20);
//    glPopMatrix();

//    glPushMatrix();
//    glTranslatef(0.6, 0.6, 0.6);
//    glutWireCone(0.3, 0.6, 20, 10);
//    glPopMatrix();

//    glPopMatrix();
    glutSolidSphere(0.5, 200, 200);
    glFlush();
}
