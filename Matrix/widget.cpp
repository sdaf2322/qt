#include "widget.h"
#include <GL/glu.h>


#ifdef Q_OS_MACOS

#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    setWindowTitle("OpenGL Matrix");
    resize(600, 600);
}

Widget::~Widget()
{
}


void Widget::initializeGL( ) {
    initializeOpenGLFunctions( );
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Widget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
}

void Widget::paintGL( )
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    GLfloat transSclae[16] = {
        1.5,0,0,0,
        0,1.5,0,0,
        0,0,1,0,
        0.5,0.5,0,1
    };

    glMultMatrixf(transSclae);
    glutWireTeapot(0.2);

    glPopMatrix();
    glFlush();
}

