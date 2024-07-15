#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <PiStepper.h>
#include <QMessageBox>
#include <QTimer>
#include <QGraphicsScene>
#include <QLabel>  // Add this line

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

    // Overall UI slots
    void on_timer_updateProgressBar();
    void on_emergencyStop_clicked();
    void on_startPageOk_clicked();

    // Page selection slots
    void on_settings_toolButton_clicked();
    void on_relative_toolButton_clicked();
    void on_absolute_toolButton_clicked();

    // Menu bar slots
    void on_actionExit_Valve_Program_triggered();

    // Settings page slots
    void on_cal_clicked();
    void on_settingsOk_clicked();
    void on_settingsCancel_clicked();

    // Relative movement page slots
    void on_relMove_clicked();
    void on_quickMove1_clicked();
    void on_quickMove2_clicked();
    void on_quickMove3_clicked();
    void on_quickMove4_clicked();

    // Absolute movement page slots
    void on_absMove_clicked();
    void on_fullOpen_clicked();
    void on_fullClose_clicked();

private:
    Ui::MainWindow *ui;
    PiStepper *stepper; // Stepper motor object
    QTimer *timer;      // Timer to update UI elements

    // Log messages objects
    QListView *logListView;
    QStringListModel *logModel;
    QStringList logMessages;

    QGraphicsScene *scene;
    void setupStartPageGraphics();

    void setUIEnabled(bool enabled);

    void addLogMessage(const QString &message);
};

#endif // MAINWINDOW_H
