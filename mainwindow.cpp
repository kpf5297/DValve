#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stepper(new PiStepper(27, 17, 22, 200, 1))
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    ui->speed_lineEdit->setText(QString::number(stepper->getSpeed()));
    ui->microstep_lineEdit->setText(QString::number(stepper->getMicrostepping()));
    ui->occlusionPctSet_lineEdit->setText(QString::number(stepper->getPercentOpen()));

    ui->rel_openClose_comboBox->addItem("Open Valve");
    ui->rel_openClose_comboBox->addItem("Close Valve");

    connect(ui->actionExit_Valve_Program, &QAction::triggered, this, &MainWindow::on_actionExit_Valve_Program_triggered);
    connect(ui->cal_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_cal_clicked()));
    connect(ui->fullOpen_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullOpen_clicked()));
    connect(ui->fullClose_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullClose_clicked()));
    connect(ui->pctMove_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_absMove_clicked()));
    connect(ui->relative_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_relMove_clicked()));

    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_updateProgressBar);

    connect(ui->settings_buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::on_settingsOk_clicked);

    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_settings_toolButton_clicked() {
    // Update displays
    ui->speed_lineEdit->setText(QString::number(stepper->getSpeed()));
    ui->microstep_lineEdit->setText(QString::number(stepper->getMicrostepping()));

    // Display settings tab
    ui->stackedWidget->setCurrentIndex(2);

}
void MainWindow::on_relative_toolButton_clicked() {
    ui->stackedWidget->setCurrentIndex(3);

}
void MainWindow::on_absolute_toolButton_clicked() {
    ui->occlusionPctSet_lineEdit->setText(QString::number(stepper->getPercentOpen()));

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_actionExit_Valve_Program_triggered() {
    QApplication::quit();
}

void MainWindow::on_cal_clicked() {
    stepper->calibrate();
}

void MainWindow::on_fullOpen_clicked() {
    stepper->moveToFullyOpen();
}

void MainWindow::on_fullClose_clicked() {
    stepper->moveToFullyClosed();
}

void MainWindow::on_absMove_clicked() {

    bool ok;

    float value = ui->occlusionPctSet_lineEdit->text().toFloat(&ok);

    if(!ok) {
        QMessageBox::warning(this, "Invalid input", "Please enter a valid number");
        return;
    }

    if ((value < 1.0) || (value > 100.0)) {
        QMessageBox::warning(this, "Out of range", "Please enter a number between 1 and 100");

        return;
    }

    stepper->moveToPercentOpen(value, []() {
            std::cout << "Move to Percent Open operation completed." << std::endl;
        });
}

void MainWindow::on_relMove_clicked() {

    QString direction = ui->rel_openClose_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;

    float value = ui->rel_stepCount_linEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        return;
    }

    int availableSteps = 0;
    if (dir == 1) {
        availableSteps = stepper->getFullRangeCount() - stepper->getCurrentStepCount();
    } else {
        availableSteps = stepper->getCurrentStepCount();
    }

    if ((value < 1.0) || (value > availableSteps)) {
        QMessageBox::warning(this, "Out of range", QString("Please enter a number between 1 and %1").arg(availableSteps));
        return;
    }



    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Move Steps operation completed." << std::endl;
    });

}


void MainWindow::on_timer_updateProgressBar() {
    int percent = stepper->getPercentOpen();
    ui->valve_pos_progressBar->setValue(percent);
}

void MainWindow::on_settingsOk_clicked() {
    bool ok;
    int userSpeed = ui->speed_lineEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this,"Invalid Input", "Please enter a valid number.");
        return;
    }

    if ((userSpeed < 1) || (userSpeed > 20)) {
        QMessageBox::warning(this, "Out of Range", "Please enter a whole number between 1 and 20");
        return;
    } else {
        stepper->setSpeed(userSpeed);
    }
}

void MainWindow::on_settingsCancel_clicked() {

}
