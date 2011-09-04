#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void mousePressEvent(QMouseEvent*);
};

// script-accessible functions for various controls.
bool pushButton(const QRect& rect);
bool checkBox(const QRect& rect, bool checked=true);
float slider(const QRect& rect, float value = 0.5f);

// "internal" functions - used by the GUI system, but not accseed by scripts
void clearControls();

#endif // MYGLWIDGET_H
