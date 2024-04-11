#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "PiStepper.h"

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int DEFAULT_DIRECTION = 0;
int DEFAULT_STEPS = 0;

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

