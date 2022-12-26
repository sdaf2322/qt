#include "widget.h"
#include <GL/glu.h>


#ifdef Q_OS_MACOS

#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    nx = 0, ny = 0;
    bScissor = FALSE;
    bStencil = FALSE;
    bEqual = FALSE;

    setWindowTitle("OpenGL Matrix");
    resize(300, 300);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(bScissor)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    glScissor(10,10,150,150);
    if(bStencil)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);

    glStencilFunc(GL_NEVER, 0x0, 0x0);
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);

    glColor3f(1,1,1);
    GLint arFac[] = {1,1,1,2,3,4,2,3,2};
    GLushort arPat[] = {0xaaaa, 0x33ff, 0x57ff, 0xaaaa, 0xaaaa,
                       0xaaaa, 0x33ff, 0x33ff, 0x57ff};
    glEnable(GL_LINE_STIPPLE);
    glLineWidth(3);
    GLfloat y;
    GLint idx = 0;
    for(y = 0.8; y > -0.8; y -= 0.2){
        glLineStipple(arFac[idx], arPat[idx]);
        glBegin(GL_LINES);{
            glVertex2f(-0.8, y);
            glVertex2f(0.8, y);
        }
        glEnd();
        idx++;
    }

    glStencilFunc(bEqual?GL_EQUAL:GL_NOTEQUAL, 0x1, 0xff);

    glColor3f(0.0,0.0,1.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(nx + 0.0, ny + 0.5);
    glVertex2f(nx - 0.5, ny - 0.5);
    glVertex2f(nx + 0.5, ny - 0.5);
    glEnd();
    glFlush();
}

void Widget::keyPressEvent(QKeyEvent* event)
{
    switch(event->key( )) {

    case Qt::Key_1:
        bScissor = TRUE;
        break;
    case Qt::Key_2:
        bScissor = FALSE;
        break;
    case Qt::Key_3:
        bStencil = TRUE;
        break;
    case Qt::Key_4:
        bStencil = FALSE;
        break;
    case Qt::Key_5:
        bEqual = TRUE;
        break;
    case Qt::Key_6:
        bEqual=FALSE;
        break;
    case Qt::Key_A:
        nx -= 0.1;
        break;
    case Qt::Key_S:
        nx += 0.1;
        break;
    case Qt::Key_D:
        ny += 0.1;
        break;
    case Qt::Key_F:
        ny -= 0.1;
        break;
    };

    update( );
}
