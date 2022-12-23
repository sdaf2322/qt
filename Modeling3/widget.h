#ifndef WIDGET_H
#define WIDGET_H
#include <GL/glut.h>
#include <QWidget>
#include <QtOpenGL>
#include <QtOpenGLWidgets>

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void drawPyramid();

    GLfloat xAngle, yAngle, zAngle;
    GLfloat left, right, bottom, top, bNear, bFar;
    GLfloat fov;
    int bProjection;
    int bObject;
    GLsizei lastWidth, lastHeight;

protected:
    void keyPressEvent(QKeyEvent*) override;

private:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
};
#endif // WIDGET_H
