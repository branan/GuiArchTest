#include "myglwidget.h"

#include <QList>
#include <QMap>
#include <QMouseEvent>

#include <QDebug>

namespace {
    QList<QRect> buttons;
    QList<QRect> checks;
    QList<QRect> sliders;
    QMap<QString,QVariant> results;
    QMap<QString,QVariant> prev_results;
    QMap<QString,QVariant> defaults;
    QMap<QString,QVariant> prev_defaults;

    QString rectToString(const QRect& r)
    {
        return QString("%1,%2,%3,%4").arg(r.left()).arg(r.right()).arg(r.top()).arg(r.bottom());
    }

    QVariant handleValues(const QString& key, QVariant def)
    {
        QVariant result = def;
        if(results.count(key)) {
            result = results[key];
        }
        if(def != result) {
            QVariant prev_default = prev_defaults[key];
            QVariant prev_result = prev_results[key];
            if(def == prev_result && result == prev_default)
                defaults[key] = result;
            else if(prev_default == def) // same value as before, so we assume this is just frame-lag
                defaults[key] = def;
            else
                defaults[key] = result;
        } else {
            defaults[key] = result;
        }
        return result;
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
        if(defaults["check-"+rectToString(rect)].toBool())
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

        int line_pos = rect.left() + defaults["slider-"+rectToString(rect)].toFloat()*rect.width();
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
    prev_results = results;
    results = defaults;
    Q_FOREACH(const QRect& rect, buttons)
    {
        if(rect.contains(evt->pos())) {
            results["button-"+rectToString(rect)] = true;
        }
    }
    Q_FOREACH(const QRect& rect, checks)
    {
        if(rect.contains(evt->pos())) {
            QString str = "check-"+rectToString(rect);
            results[str] = !defaults[str].toBool();
        }
    }
    Q_FOREACH(const QRect& rect, sliders)
    {
        if(rect.contains(evt->pos())) {
            float result = float(evt->pos().x() - rect.left()) / float(rect.width());
            results["slider-"+rectToString(rect)] = QVariant(result);
        }
    }
}

// This gets called once just before control
// is passed to the GUI script
void clearControls()
{
    buttons.clear();
    checks.clear();
    sliders.clear();
    prev_defaults = defaults;
    defaults.clear();
}

// This adds a new rectangle to the draw queue,
// and checks if this button has existed in the past,
// and received a click. If it has, the click is returned.
bool pushButton(const QRect& rect)
{
    bool result = false;
    QString rect_as_string = "button-"+rectToString(rect);
    buttons.append(rect);
    if(results.count(rect_as_string)) {
        result = results[rect_as_string].toBool();
        results.remove(rect_as_string);
    }
    return result;
}

// This is a lot like the push button code. The differences
// 1. It sets a default value for itself in the results map
// 2. It does not remove itself from the results map once its result is read.
bool checkBox(const QRect &rect, bool checked)
{
    checks.append(rect);
    return handleValues("check-"+rectToString(rect), checked).toBool();
}

// The slider doesn't quite "slide" yet, but it changes its value
// based on where in it the user clicks.
// Currently locked to 0..1. Scaling the range is easy.
float slider(const QRect& rect, float value)
{
    sliders.append(rect);
    return handleValues("slider-"+rectToString(rect), value).toFloat();
}
