#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void move_button_clicked();
    void onHome_button_clicked();
    void on_test_button_clicked();
    void checkTriggerPin();
    void triggered_mode_enable_checked();

private:
    Ui::MainWindow *ui;
    static QTimer* externalTriggerTimer;
    static int absolutePosition;
};
#endif // MAINWINDOW_H
