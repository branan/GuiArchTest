#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ui_state(false)
{
    ui->setupUi(this);
    QTimer* t = new QTimer(this);
    t->setInterval(0);
    t->start();
    connect(t, SIGNAL(timeout()), SLOT(doUiTick()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


// This is the "backend"
// Normally this should establish some sort of GUI context
// here we just use a single global context
void MainWindow::doUiTick()
{
    clearControls();
    onGUI();
    ((QGLWidget*)centralWidget())->updateGL();
}


// This should look more-or-less like a Unity GUI function
void MainWindow::onGUI()
{
    int top = 0;
    if(ui_state) {
        top = 100;
    }
    QRect r(0, top, 100, 100);
    if(pushButton(r))
        ui_state = !ui_state;
}
