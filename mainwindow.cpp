#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stepper(new PiStepper(27, 17, 22, 200, 1))
    , timer(new QTimer(this))
    , logModel(new QStringListModel(this))
    , scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // Setup start page graphics
    setupStartPageGraphics();

    ui->speed_lineEdit->setText(QString::number(stepper->getSpeed()));
    ui->microstep_lineEdit->setText(QString::number(stepper->getMicrostepping()));
    ui->occlusionPctSet_lineEdit->setText(QString::number(stepper->getPercentOpen()));

    ui->rel_openClose_comboBox->addItem("Open Valve");
    ui->rel_openClose_comboBox->addItem("Close Valve");

    ui->qu1_comboBox->addItem("Open Valve");
    ui->qu1_comboBox->addItem("Close Valve");

    ui->qu2_comboBox->addItem("Open Valve");
    ui->qu2_comboBox->addItem("Close Valve");
    ui->qu3_comboBox->addItem("Open Valve");
    ui->qu3_comboBox->addItem("Close Valve");
    ui->qu4_comboBox->addItem("Open Valve");
    ui->qu4_comboBox->addItem("Close Valve");

    // Set up log list view
    logListView = ui->log_listView;
    logListView->setModel(logModel);
    logMessages.clear();

    connect(ui->actionExit_Valve_Program, &QAction::triggered, this, &MainWindow::on_actionExit_Valve_Program_triggered);
    connect(ui->cal_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_cal_clicked()));
    connect(ui->fullOpen_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullOpen_clicked()));
    connect(ui->fullClose_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullClose_clicked()));
    connect(ui->pctMove_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_absMove_clicked()));
    connect(ui->relative_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_relMove_clicked()));

    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_updateProgressBar);

    connect(ui->settings_buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::on_settingsOk_clicked);

    connect(ui->estop_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_emergencyStop_clicked()));

    connect(ui->startPage_buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::on_startPageOk_clicked);
    connect(ui->startPage_buttonBox, &QDialogButtonBox::rejected, this, &MainWindow::on_actionExit_Valve_Program_triggered);

    setUIEnabled(false); // Disable UI elements

    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStartPageGraphics() {
    QString imagePath = ":/images/motor_valve.png";  // Resource path
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        QMessageBox::warning(this, "Image Load Error", "Failed to load the image: " + imagePath);
        return;
    }

    // Set the pixmap to the QLabel and scale it to fit
    ui->startPage_imageLabel->setPixmap(pixmap.scaled(ui->startPage_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::on_settings_toolButton_clicked() {
    // Update displays
    ui->speed_lineEdit->setText(QString::number(stepper->getSpeed()));
    ui->microstep_lineEdit->setText(QString::number(stepper->getMicrostepping()));

    // Display settings tab
    ui->stackedWidget->setCurrentIndex(2);
    addLogMessage("Settings tab opened.");
}

void MainWindow::on_relative_toolButton_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    addLogMessage("Relative move tab opened.");
}

void MainWindow::on_absolute_toolButton_clicked() {
    ui->occlusionPctSet_lineEdit->setText(QString::number(stepper->getPercentOpen()));
    ui->stackedWidget->setCurrentIndex(1);
    addLogMessage("Absolute move tab opened.");
}

void MainWindow::on_actionExit_Valve_Program_triggered() {
    addLogMessage("Exiting valve program.");
    QApplication::quit();
}

void MainWindow::on_cal_clicked() {
    stepper->calibrate();
    addLogMessage("Calibration completed.");
}

void MainWindow::on_fullOpen_clicked() {
    stepper->moveToFullyOpen();
    addLogMessage("Moving to fully open position.");
}

void MainWindow::on_fullClose_clicked() {
    stepper->moveToFullyClosed();
    addLogMessage("Moving to fully closed position.");
}

void MainWindow::on_absMove_clicked() {
    bool ok;
    float value = ui->occlusionPctSet_lineEdit->text().toFloat(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid input", "Please enter a valid number");
        addLogMessage("Invalid input for percent occlusion.");
        return;
    }

    if (value < 1.0 || value > 100.0) {
        QMessageBox::warning(this, "Out of range", "Please enter a number between 1 and 100");
        addLogMessage("Percent occlusion out of range.");
        return;
    }

    stepper->moveToPercentOpen(value, []() {
        std::cout << "Move to Percent Open operation completed." << std::endl;
    });
    addLogMessage(QString("Moving valve to %1% open position.").arg(value));
}

void MainWindow::on_relMove_clicked() {
    QString direction = ui->rel_openClose_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;
    int value = ui->rel_stepCount_linEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        addLogMessage("Invalid input for relative move steps.");
        return;
    }

    int availableSteps = (dir == 1) ? (stepper->getFullRangeCount() - stepper->getCurrentStepCount()) : stepper->getCurrentStepCount();

    if (value < 1 || value > availableSteps) {
        QMessageBox::warning(this, "Out of range", QString("Please enter a number between 1 and %1").arg(availableSteps));
        addLogMessage("Relative move steps out of range.");
        return;
    }

    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Move Steps operation completed." << std::endl;
    });
    addLogMessage(QString("Moving valve %1 steps %2.").arg(value).arg(dir == 1 ? "open" : "closed"));
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
        addLogMessage("Invalid speed input.");
        return;
    }

    if (userSpeed < 1 || userSpeed > MAX_SPEED) {
        QMessageBox::warning(this, "Out of Range", "Please enter a whole number between 1 and 20");
        addLogMessage("Speed input out of range.");
        return;
    } else {
        stepper->setSpeed(userSpeed);
        addLogMessage(QString("Speed set to %1 RPM.").arg(userSpeed));
    }
}

void MainWindow::on_settingsCancel_clicked() {
    addLogMessage("Settings change canceled.");
}

void MainWindow::on_emergencyStop_clicked() {
    stepper->emergencyStop();
    setUIEnabled(false); // Disable UI elements
    QMessageBox::warning(this, "Emergency Stop Pressed", "Motor Operation Stopped");
    addLogMessage("Emergency stop activated.");
}

void MainWindow::addLogMessage(const QString &message) {
    logMessages.append(message);
    logModel->setStringList(logMessages);
    logListView->scrollToBottom();
}

void MainWindow::setUIEnabled(bool enabled) {
    ui->settings_toolButton->setEnabled(enabled);
    ui->absolute_toolButton->setEnabled(enabled);
    ui->relative_toolButton->setEnabled(enabled);
    ui->cal_commandLinkButton->setEnabled(enabled);
    ui->fullOpen_commandLinkButton->setEnabled(enabled);
    ui->fullClose_commandLinkButton->setEnabled(enabled);
    ui->pctMove_commandLinkButton->setEnabled(enabled);
    ui->relative_commandLinkButton->setEnabled(enabled);
    // ui->estop_commandLinkButton->setEnabled(enabled);
}

void MainWindow::on_startPageOk_clicked() {
    setUIEnabled(false); // Disable UI elements
    stepper->calibrate();
    setUIEnabled(true); // Enable UI elements after calibration
    ui->stackedWidget->setCurrentIndex(1); // Switch to another page after calibration
}
