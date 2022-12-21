#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtOpenGL>
#include <QtOpenGLWidgets>

class Widget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    double radius;
    double delta_theta, cord_length;
    double xcenter, ycenter;
    double x, y, theta;
    double current_angle, atime;

private:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

public slots:
    void timerFunction();
};
#endif // WIDGET_H
