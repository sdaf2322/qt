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

    GLfloat xAngle, yAngle, zAngle;
    GLboolean bNormal;

    void getNormal(GLfloat a[3], GLfloat b[3], GLfloat c[3],
                                               GLfloat normal[3]);

protected:
    void keyPressEvent(QKeyEvent*) override;

private:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
};
#endif // WIDGET_H
