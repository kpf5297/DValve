#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <PiStepper.h>
#include <QMessageBox>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_settings_toolButton_clicked();
    void on_relative_toolButton_clicked();
    void on_absolute_toolButton_clicked();

    void on_actionExit_Valve_Program_triggered();

    void on_cal_clicked();

    void on_absMove_clicked();
    void on_fullOpen_clicked();
    void on_fullClose_clicked();

    void on_relMove_clicked();

    void on_timer_updateProgressBar();

    void on_settingsOk_clicked();
    void on_settingsCancel_clicked();

private:
    Ui::MainWindow *ui;
    PiStepper *stepper;
    QTimer *timer; // Timer to update display outputs
};
#endif // MAINWINDOW_H
