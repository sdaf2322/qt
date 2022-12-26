#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtOpenGL>
#include <GL/glut.h>

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    GLfloat nx, ny;
    GLboolean bScissor;
    GLboolean bStencil;
    GLboolean bEqual;

protected:
    void keyPressEvent(QKeyEvent*) override;

private:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
};
#endif // WIDGET_H
