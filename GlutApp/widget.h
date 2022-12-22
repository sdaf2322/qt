#ifndef WIDGET_H
#define WIDGET_H

#include <GL/glut.h>
#include <QWidget>
#include <QtOpenGL>
#include <QtOpenGLWidgets>

void glutWireCube(GLdouble size);
void glutSolidCube(GLdouble size);
void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius,
                   GLint nsides, GLint rings);
void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);

#endif // WIDGET_H
