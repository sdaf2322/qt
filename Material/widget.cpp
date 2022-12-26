#include "widget.h"
#include <GL/glu.h>


#ifdef Q_OS_MACOS

#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    xAngle = 0, yAngle = 0, zAngle = 0;
    red = 0.5, green = 0.5, blue = 0.5;
    bColorTrack = GL_FALSE;

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    if(bColorTrack){
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT,GL_AMBIENT);
    }else
        glDisable(GL_COLOR_MATERIAL);

    glPushMatrix();
    glRotatef(xAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

    GLfloat arbottom[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, arbottom);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();

    GLfloat arMat1[] = {1.0,0.0,0.0,1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, arMat1);
    glBegin(GL_TRIANGLE_FAN);

    glColor3f(1,1,1);
    glVertex3f(0.0, 0.0, -0.8);
    glColor3f(1,0,0);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, 0.5);

    GLfloat arMat2[] = {0.0,0.0,0.5,1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, arMat2);
    glColor3f(1,1,0);
    glVertex2f(-0.5, -0.5);

    GLfloat arMat3[] = {0.0,0.25,0.0,1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, arMat3);
    glColor3f(1,1,0);
    glVertex2f(0.5, -0.5);

    GLfloat arMat4[] = {0.0, 0.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, arMat4);
    glColor3f(0,0,1);
    glVertex2f(0.5, 0.5);
    glEnd();

    glPopMatrix();
    glFlush();
}

void Widget::keyPressEvent(QKeyEvent* event)
{
    switch(event->key( )) {

    case Qt::Key_Q:
        bColorTrack = GL_TRUE;
        break;
    case Qt::Key_W:
        bColorTrack = GL_FALSE;
        break;

    case Qt::Key_1:
        yAngle += 2;
        break;
    case Qt::Key_2:
        yAngle -= 2;
        break;
    case Qt::Key_3:
        xAngle += 2;
        break;
    case Qt::Key_4:
        xAngle -= 2;
        break;
    case Qt::Key_5:
        zAngle += 2;
        break;
    case Qt::Key_6:
        zAngle -= 2;
        break;
    case Qt::Key_0:
        xAngle = yAngle = zAngle = 0.0;
        break;

    case Qt::Key_A:
        red += 0.1;
        break;
    case Qt::Key_S:
        red -= 0.1;
        break;
    case Qt::Key_D:
        green += 0.1;
        break;
    case Qt::Key_F:
        green -= 0.1;
        break;
    case Qt::Key_G:
        blue += 0.1;
        break;
    case Qt::Key_H:
        blue -= 0.1;
        break;
    case Qt::Key_J:
        red = green = blue = 0.5;
        break;
    };

//    QString str = QString("Cube3D : x : %1, y : %2, z : %3")                 .arg(xAngle).arg(yAngle).arg(zAngle);
//    setWindowTitle(str);
    update( );
}
