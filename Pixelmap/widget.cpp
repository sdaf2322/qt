#include "widget.h"
#include <GL/glu.h>

#ifdef Q_OS_MACOS

#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    setWindowTitle("OpenGL Human-Body");
    resize(600, 600);
}

Widget::~Widget()
{
}


void Widget::initializeGL( )
{
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Widget::resizeGL(int w, int h) {
    glViewport(0,0,(GLint)w,(GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
}

void Widget::paintGL( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLubyte data[32*32*3];

    for(int y = 0; y < 32; y++)
    {
        for(int x = 0; x < 32; x++)
        {
            data[y*32*3+x*3+0] = 0xff;
            data[y*32*3+x*3+1] = 0xff;
            data[y*32*3+x*3+2] = 0x00;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2f(0.0,0.0);
    glDrawPixels(32, 32, GL_RGB, GL_UNSIGNED_BYTE, data);

    glFlush();

}
