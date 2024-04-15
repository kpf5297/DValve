#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "PiStepper.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int DEFAULT_DIRECTION = 0;
int DEFAULT_STEPS = 0;
int DEFAULT_OPEN_DURATION = 90;
int FULL_COUNT_RANGE = 1700;

// Create a PiStepper object
PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->direction_comboBox->addItem("Open", QVariant(1)); // Assuming 1 represents open
    ui->direction_comboBox->addItem("Closed", QVariant(0)); // Assuming 0 represents closed

    ui->step_lineEdit->setText(QString::number(DEFAULT_STEPS));

    ui->open_triggered_checkBox->setChecked(false);

    ui->open_duration_lineEdit->setText(QString::number(DEFAULT_OPEN_DURATION));

    // Set a timer to check if external trigger is enabled
    // QTimer *timer = new QTimer(this); // Create a new QTimer
    // connect(timer, SIGNAL(timeout()), this, SLOT(triggered_mode_enable_checked())); // Connect the timeout signal to the triggered_mode_enable_checked slot
    // timer->start(1000);

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

    if (ok) {
        // The conversion was successful, use the integer value as needed
    } else {
        // The conversion failed (e.g., the text was not a valid integer)
        // Handle the error, maybe show a message box or set a default value
    }

    int directionValue = ui->direction_comboBox->currentData().toInt();

    stepper.moveSteps(stepValue,directionValue);

}



void MainWindow::onHome_button_clicked()
{
    stepper.homeMotor();
}

void MainWindow::on_test_button_clicked() {

    QString openTime = ui->open_duration_lineEdit->text();

    // Ensure value is >= 8.5 seconds
    if (openTime.toInt() < 8.5) {
        openTime = "8.5";
        ui->open_duration_lineEdit->setText(openTime);
    }

    stepper.moveStepsOverDuration(FULL_COUNT_RANGE,openTime.toInt());
}



void MainWindow::triggered_mode_enable_checked() {
    if (ui->open_triggered_checkBox->isChecked()) {
        QMessageBox::StandardButton reply = QMessageBox::information(this, "External trigger is enabled. Monitoring external trigger", "Close this dialog box to stop monitoring external trigger.");


    }

    // Uncheck the checkbox
    ui->open_triggered_checkBox->setChecked(false);

}



