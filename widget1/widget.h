#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtOpenGL>
#include <QtOpenGLWidgets>

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget* parent = 0);
    ~Widget();

    GLfloat rectPosX;
    GLfloat rectPosY;
    GLfloat rectSize;

    GLfloat xStep;
    GLfloat yStep;

    GLfloat windowWidth;
    GLfloat windowHeight;

private:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

public slots:
    void timerFunction();
};

#endif // WIDGET_H
