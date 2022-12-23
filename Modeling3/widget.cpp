#include "widget.h"
#include <GL/glut.h>


#ifdef Q_OS_MACOS

#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    xAngle = 0, yAngle = 0, zAngle = 0;
    left = -1, right = 1;
    bottom = -1, top = 1;
    bNear = -1, bFar = 1;
    fov = 45;

    setWindowTitle("OpenGL Cube");
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
    lastWidth = w;
    lastHeight = h;

    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity( );
    glOrtho(-1, 1, -1, 1, -1, 1);
}

void Widget::paintGL( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch(bProjection){
    case 0:
        glOrtho(left, right, bottom, top, bNear, bFar);
        break;
    case 1:
        glFrustum(left, right, bottom, top, bNear, bFar);
        break;
    case 2:
        GLfloat aspect = (GLfloat)lastWidth / (GLfloat)lastHeight;
        gluPerspective(fov, aspect, bNear, bFar);
        break;
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    if(bProjection != 0)
        glTranslatef(0,0,-2);

    glRotatef(xAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

    if(bObject == 0){
        drawPyramid();
    }else{
        GLUquadricObj *pQuad;
        pQuad = gluNewQuadric();
        gluQuadricDrawStyle(pQuad, GLU_LINE);
        glTranslatef(0.0, 0.0, -0.6);
        glColor3f(1,1,1);
        gluCylinder(pQuad, 0.3, 0.3, 1.2, 20, 20);
    }

    glPopMatrix();
    glFlush();
}

void Widget::drawPyramid()
{
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1,1,1);
    glVertex3f(0.0, 0.0, 0.8);
    glColor3f(1,0,0);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, 0.5);

    glColor3f(1,1,0);
    glVertex2f(-0.5, -0.5);

    glColor3f(0, 1, 0);
    glVertex2f(0.5, -0.5);

    glColor3f(0,0,1);
    glVertex2f(0.5, 0.5);
    glEnd();
}

void Widget::keyPressEvent(QKeyEvent* event)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);

    switch(event->key( )) {
    case Qt::Key_1:
        bProjection = 0;
        bNear = -1;
        bFar = 1;
        break;
    case Qt::Key_2:
        bProjection = 1;
        bNear = 1;
        bFar = 10;
        break;
    case Qt::Key_3:
        bProjection = 2;
        bNear = 1;
        bFar = 10;
        break;
    case Qt::Key_4:
        bObject = 0;
        break;
    case Qt::Key_5:
        bObject = 1;
        break;
    case Qt::Key_A:
        yAngle += 2;
        break;
    case Qt::Key_S:
        yAngle -= 2;
        break;
    case Qt::Key_D:
        xAngle += 2;
        break;
    case Qt::Key_F:
        xAngle -= 2;
        break;
    case Qt::Key_G:
        zAngle += 2;
        break;
    case Qt::Key_H:
        zAngle -= 2;
        break;
    case Qt::Key_J:
        xAngle = yAngle = zAngle = 0.0;
        break;
    case Qt::Key_Q:
        left += 0.1;
        break;
    case Qt::Key_W:
        left -= 0.1;
        break;
    case Qt::Key_E:
        right += 0.1;
        break;
    case Qt::Key_R:
        right -= 0.1;
        break;
    case Qt::Key_T:
        bottom -= 0.1;
        break;
    case Qt::Key_Y:
        bottom += 0.1;
        break;
    case Qt::Key_U:
        top -= 0.1;
        break;
    case Qt::Key_I:
        top += 0.1;
        break;
    case Qt::Key_O:
        bNear -= 0.1;
        break;
    case Qt::Key_P:
        bNear += 0.1;
        break;
    case Qt::Key_Z:
        bFar -= 0.1;
        break;
    case Qt::Key_X:
        bFar += 0.1;
        break;
    case Qt::Key_C:
        fov -= 1;
        break;
    case Qt::Key_V:
        fov += 1;
        break;
    case Qt::Key_B:
        left = -1, right = 1, bottom = -1, top = 1;
        if(bProjection == 0){
            bNear = -1, bFar = 1;
        }else{
            bNear = 1, bFar = 10;
        }
        break;
    };
    update();
}
