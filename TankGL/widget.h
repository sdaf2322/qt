#ifndef WIDGET_H
#define WIDGET_H

#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#endif

#include <QtOpenGL>
#include <GL/glut.h>


struct bullet{
    float ypos;
    float xpos;
    float speed;
    bullet* next;
};

struct enemy{
    float ypos;
    float xpos;
    float speed;
    enemy* next;
};

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    GLuint filter;
    GLuint texture[4];
    GLuint base;

    bullet* headB;
    bullet* tailB;
    bullet* currentB;

    bool firstShot;
    bool enemyAround;
    float tankPosHoriz;
    float lastShot;
    float newShot;
    float lastMove, newMove;

    enemy normalEnemy;

    GLUquadricObj* quadratic;

    QTimer timer;

    void updateBullet();
    void updateEnemy();
    int loadGLTextures();

private:
    void initializeGL( ) override;
    void paintGL( ) override;
    void resizeGL(int w, int h) override;

    //void mousePressEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateTimer();
};
#endif // WIDGET_H
