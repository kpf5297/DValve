#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "PiStepper.h"
#include "DigitalPin.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QThread>


int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int DEFAULT_DIRECTION = 0;
int DEFAULT_STEPS = 0;
int DEFAULT_OPEN_DURATION = 90;
int FULL_COUNT_RANGE = 1700;


PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);

DigitalPin triggerPin(16, DigitalPin::Direction::Input, "Trigger");

QTimer* MainWindow::externalTriggerTimer = nullptr;

int MainWindow::absolutePosition = 0;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    externalTriggerTimer = new QTimer(this);
    connect(externalTriggerTimer, &QTimer::timeout, this, &MainWindow::checkTriggerPin);
    externalTriggerTimer->start(1000); // 1 second (may need to be adjusted for response time)

    ui->direction_comboBox->addItem("Open", QVariant(1));                           // Add the Open option to the dropdown
    ui->direction_comboBox->addItem("Closed", QVariant(0));                         // Add the Closed option to the dropdown
    ui->step_lineEdit->setText(QString::number(DEFAULT_STEPS));                     // Set the default steps value
    ui->open_triggered_checkBox->setChecked(false);                                 // Set the default value for the triggered mode checkbox
    ui->open_duration_lineEdit->setText(QString::number(DEFAULT_OPEN_DURATION));    // Set the default open duration values
    ui->tabWidget->setCurrentIndex(0);

    stepper.homeMotor();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::move_button_clicked() {

    // Convert the text to an integer
    bool ok;

    // Read the text from QLineEdit
    QString stepText = ui->step_lineEdit->text();

    // Convert the text to an integer
    int stepValue = stepText.toInt(&ok);

    // If the conversion failed, show an error message
    if (!ok) {
        QMessageBox::warning(this, "Error", "Invalid step value");
        return;
    }

    // Verify that the step value is within the range
    if (stepValue < 0 || stepValue > FULL_COUNT_RANGE) {
        QMessageBox::warning(this, "Error", "Step value must be between 0 and 1700");
        return;
    }

    // Remaining steps check 
    if (absolutePosition + stepValue > FULL_COUNT_RANGE) {
        QMessageBox::warning(this, "Error", "Cannot move past the maximum position");

        // Enter the maximum position in the QLineEdit
        ui->step_lineEdit->setText(QString::number(FULL_COUNT_RANGE - absolutePosition));

        return;
    } else if (absolutePosition + stepValue < 0) {
        QMessageBox::warning(this, "Error", "Cannot move past the minimum position");

        // Enter the minimum position in the QLineEdit
        ui->step_lineEdit->setText(QString::number(-absolutePosition));
        
        return;
    }

    int directionValue = ui->direction_comboBox->currentData().toInt();

    stepper.moveSteps(stepValue,directionValue);

    // Update the absolute position
    if (directionValue == 1) {
        absolutePosition += stepValue;
    } else {
        absolutePosition -= stepValue;
    }

}

void MainWindow::onHome_button_clicked()
{
    stepper.homeMotor();

    // Reset the absolute position
    absolutePosition = 0;
}

void MainWindow::on_test_button_clicked() {
    bool ok;
    QString openTimeText = ui->open_duration_lineEdit->text();
    int openTime = openTimeText.toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Error", "Invalid open duration value");
        return;
    }

    // Ensure value is in the range (Minimum of 8.5 seconds)
    if (openTime < 8.5) {
        openTime = 8.5;
        ui->open_duration_lineEdit->setText(QString::number(openTime));
    }

    std::cout << "Opening for " << std::to_string(openTime) << " seconds" << std::endl;

    stepper.moveStepsOverDuration(FULL_COUNT_RANGE,openTime);

    // The GUI will remain responsive using this delay
    QTimer::singleShot(1000, this, SLOT(someSlotFunction()));

    stepper.homeMotor();
}

void MainWindow::checkTriggerPin() {
    if (triggerPin.read() == 0) { // Trigger pin is LOW
        qDebug() << "Trigger pin is LOW. Initiating action.";
        on_test_button_clicked();
    }
}

void MainWindow::triggered_mode_enable_checked() {
    if (ui->open_triggered_checkBox->isChecked()) {
        externalTriggerTimer->start(100); // Start polling every 100 ms
    } else {
        externalTriggerTimer->stop();
    }
}





