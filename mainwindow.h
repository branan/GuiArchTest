#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool ui_state;
    bool toggle_1, toggle_2;
    float slider_value;

    void onGUI();

private slots:
    void doUiTick();
};

#endif // MAINWINDOW_H
