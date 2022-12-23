#include "widget.h"

#include <QTimer>

#ifdef Q_OS_MACOS
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#endif

#define ONE 1

Widget::Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    xAngle = yAngle = zAngle = 0;
    rotation = 0;
    drawInOrtho = GL_TRUE;

    setWindowTitle("OpenGL Cube");
    resize(300, 300);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout( )), SLOT(update( )));
    timer->start(250);
}

Widget::~Widget()
{
}


void Widget::initializeGL( ) {
    initializeOpenGLFunctions( );
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void Widget::resizeGL(int w, int h) {
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity( );
    glOrtho(-10, 10, -10, 10, -10, 10);
    glClearColor(0.5, 0.5, 0.5, 0.0);
}

void Widget::paintGL( )
{
    // 정육면체의 각 면들을 설정
    // 정육면체이기 때문에 6개의 face 를 설정했다.
    // 방향성 때문에 순서에 일관성이 있다.
    static GLubyte front[]  = {0,1,3,2};    // front face
    static GLubyte back[]   = {4,5,7,6};    // back face
    static GLubyte top[]    = {5,1,6,2};    // top face
    static GLubyte bottom[] = {4,0,7,3};    // bottom face
    static GLubyte left[]   = {5,1,4,0};       // left face
    static GLubyte right[]  = {6,2,7,3};       // right face

    // X,Y,Z 로 된 좌표(꼭지점)이다.
    // 예를 들면 face 에서 설정된 0 은 (-5,-5,-,5) 이다.
    static GLshort vertices[] = {-5,-5,-5,  -5,-5,5,  5,-5,5,
                       5,-5,-5,  -5,5,-5,   -5,5,5,   5,5,5,  5,5,-5};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);       // 3차원에 객체를 그릴 때 사용
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );

    if(!drawInOrtho)
      glTranslated(0, 0, -30);

    // Y 축 기준으로 45도 회전(Rotate)
    glRotated(45, ONE, 0, 0);
    glRotated(rotation++, 0, ONE,0);

    glEnableClientState(GL_VERTEX_ARRAY);
    // 사용할 vertex 를 등록
    glVertexPointer(3, GL_SHORT, 0, vertices);

    // 6개의 face 를 각각 Draw
    // 각 face 를 다른 색으로 표현한다.
    glColor4d(ONE,0,0,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, front);

    glColor4d(0,ONE,0,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, back);

    glColor4d(0,0,ONE,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, top);

    glColor4d(ONE,ONE,0,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, bottom);

    glColor4d(0,ONE,ONE,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, left);

    glColor4d(ONE,0,ONE,0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, right);

    glDisableClientState(GL_VERTEX_ARRAY);
    glFlush( );
}

void Widget::keyPressEvent(QKeyEvent* event)
{
    switch(event->key( )) {

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
    };

//    QString str = QString("Cube3D : x : %1, y : %2, z : %3")                 .arg(xAngle).arg(yAngle).arg(zAngle);
//    setWindowTitle(str);
    update( );
}
