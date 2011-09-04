#include "myglwidget.h"

#include <QList>
#include <QMap>
#include <QMouseEvent>

#include <QDebug>

namespace {
    QList<QRect> buttons;
    QList<QRect> checks;
    QList<QRect> sliders;
    QMap<QString,bool> results;
    QMap<QString,bool> next_results;
    QMap<QString,float> fresults;
    QMap<QString, float> next_fresults;

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
    glClearColor(0.f, 0.f, 0.f, 0.f);
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
    glColor3f(1.f, 0.f, 0.f);
    Q_FOREACH(const QRect& rect, buttons)
    {
        glVertex2f(rect.left(), rect.bottom());
        glVertex2f(rect.left(), rect.top());
        glVertex2f(rect.right(), rect.top());
        glVertex2f(rect.right(), rect.bottom());
    };
    Q_FOREACH(const QRect& rect, checks)
    {
        if(next_results[rectToString(rect)])
            glColor3f(0.f, 1.f, 0.f);
        else
            glColor3f(0.f, 0.f, 1.f);
        glVertex2f(rect.left(), rect.bottom());
        glVertex2f(rect.left(), rect.top());
        glVertex2f(rect.right(), rect.top());
        glVertex2f(rect.right(), rect.bottom());
    }
    glEnd();
    glColor3f(0.f, 1.f, 1.f);
    glBegin(GL_LINES);
    Q_FOREACH(const QRect& rect, sliders)
    {
        glVertex2f(rect.left(), rect.bottom());
        glVertex2f(rect.left(), rect.top());

        glVertex2f(rect.left(), rect.bottom());
        glVertex2f(rect.right(), rect.bottom());

        glVertex2f(rect.left(), rect.top());
        glVertex2f(rect.right(), rect.top());

        glVertex2f(rect.right(), rect.bottom());
        glVertex2f(rect.right(), rect.top());

        int line_pos = rect.left() + next_fresults[rectToString(rect)]*rect.width();
        glVertex2f(line_pos, rect.bottom());
        glVertex2f(line_pos, rect.top());
    }
    glEnd();
}

// This clears the previous result set (so that unread events
// don't get passed on during later frames), and sets the clicked
// flag for any rectangles that overlap the mouse.
void MyGLWidget::mousePressEvent(QMouseEvent *evt)
{
    results = next_results;
    fresults = next_fresults;
    Q_FOREACH(const QRect& rect, buttons)
    {
        if(rect.contains(evt->pos())) {
            results[rectToString(rect)] = true;
        }
    }
    Q_FOREACH(const QRect& rect, checks)
    {
        if(rect.contains(evt->pos())) {
            QString str = rectToString(rect);
            results[str] = !next_results[str];
        }
    }
    Q_FOREACH(const QRect& rect, sliders)
    {
        if(rect.contains(evt->pos())) {
            float result = float(evt->pos().x() - rect.left()) / float(rect.width());
            fresults[rectToString(rect)] = result;
        }
    }
}

// This gets called once just before control
// is passed to the GUI script
void clearControls()
{
    buttons.clear();
    checks.clear();
    next_results.clear();
    next_fresults.clear();
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
    return result;
}

// This is a lot like the push button code. The differences
// 1. It sets a default value for itself in the results map
// 2. It does not remove itself from the results map once its result is read.
bool checkBox(const QRect &rect, bool checked)
{
    QString rect_as_string = rectToString(rect);
    bool result = checked;
    checks.append(rect);
    if(results.count(rect_as_string)) {
        result = results[rect_as_string];
    }
    next_results.insert(rect_as_string, result);
    return result;
}

// The slider doesn't quite "slide" yet, but it changes its value
// based on where in it the user clicks.
// Currently locked to 0..1. Scaling the range is easy.
float slider(const QRect& rect, float value)
{
    QString rect_as_string = rectToString(rect);
    float result = value;
    sliders.append(rect);
    if(fresults.count(rect_as_string)) {
        result = fresults[rect_as_string];
    }
    next_fresults.insert(rect_as_string, result);
    return result;
}
