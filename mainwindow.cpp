#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DPSTSwitch.h"
#include "PiStepper.h"

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int DEFAULT_DIRECTION = 0;
int DEFAULT_STEPS = 0;
int DEFAULT_SPEED = 80;

// Create a PiStepper object
PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

        ui->dir_lineEdit->setText(QString::number(DEFAULT_DIRECTION));
        ui->step_lineEdit->setText(QString::number(DEFAULT_STEPS));
        ui->speed_lineEdit->setText(QString::number(DEFAULT_SPEED));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::move_button_clicked() {




    // Read the text from QLineEdit
    QString directionText = ui->dir_lineEdit->text();

    // Convert the text to an integer
    bool ok;
    int dirValue = directionText.toInt(&ok);

    if (ok) {
        // The conversion was successful, use the integer value as needed
    } else {
        // The conversion failed (e.g., the text was not a valid integer)
        // Handle the error, maybe show a message box or set a default value
    }


    // Read the text from QLineEdit
    QString stepText = ui->step_lineEdit->text();

    // Convert the text to an integer
    int stepValue = stepText.toInt(&ok);

    if (ok) {
        // The conversion was successful, use the integer value as needed
    } else {
        // The conversion failed (e.g., the text was not a valid integer)
        // Handle the error, maybe show a message box or set a default value
    }

    // Read the text from QLineEdit
    QString speedText = ui->step_lineEdit->text();

    // Convert the text to an integer
    int speedValue = speedText.toInt(&ok);

    if (ok) {
        stepper.setSpeed(speedValue);
    } else {
        // The conversion failed (e.g., the text was not a valid integer)
        // Handle the error, maybe show a message box or set a default value
    }

    stepper.moveSteps(stepValue,dirValue);

}

