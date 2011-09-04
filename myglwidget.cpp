#include "myglwidget.h"

#include <QList>
#include <QMap>
#include <QMouseEvent>

#include <QDebug>

namespace {
    QList<QRect> buttons;
    QMap<QString,bool> results;
    QMap<QString,bool> next_results;

    QString rectToString(const QRect& r)
    {
        return QString("%1,%2,%3,%4").arg(r.left()).arg(r.right()).arg(r.top()).arg(r.bottom());
    }
}

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void MyGLWidget::initializeGL()
{
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glColor3f(1.f, 0.f, 1.f);
}

void MyGLWidget::resizeGL(int w, int h)
{
    glMatrixMode(GL_MODELVIEW);
    glScalef(1.f, -1.f, 1.f);
    glScalef(2.f/w, 2.f/h, 1.f);
    glTranslatef(-w/2, -h/2, 0.f);
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    Q_FOREACH(const QRect& rect, buttons)
    {
        glVertex2f(rect.left(), rect.bottom());
        glVertex2f(rect.left(), rect.top());
        glVertex2f(rect.right(), rect.top());
        glVertex2f(rect.right(), rect.bottom());
    };
    glEnd();
}

// This clears the previous result set (so that unread events
// don't get passed on during later frames), and sets the clicked
// flag for any rectangles that overlap the mouse.
void MyGLWidget::mousePressEvent(QMouseEvent *evt)
{
    results = next_results;
    next_results.clear();
    Q_FOREACH(const QRect& rect, buttons)
    {
        if(rect.contains(evt->pos())) {
            results[rectToString(rect)] = true;
        }
    }
}

// This gets called once just before control
// is passed to the GUI script
void clearControls()
{
    buttons.clear();
    next_results.clear();
}

// This adds a new rectangle to the draw queue,
// and checks if this button has existed in the past,
// and received a click. If it has, the click is returned.
bool pushButton(const QRect& rect)
{
    bool result = false;
    QString rect_as_string = rectToString(rect);
    buttons.append(rect);
    if(results.count(rect_as_string)) {
        result = results[rect_as_string];
        results.remove(rect_as_string);
    }
    next_results[rect_as_string] = false;
    return result;
}
