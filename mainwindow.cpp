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
    
    // === Overall UI elements setup ===
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->actionExit_Valve_Program, &QAction::triggered, this, &MainWindow::on_actionExit_Valve_Program_triggered);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_updateProgressBar);
    connect(ui->estop_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_emergencyStop_clicked()));
    
    // === Setup start page graphics ===
    setupStartPageGraphics();

    connect(ui->startPage_buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::on_startPageOk_clicked);
    connect(ui->startPage_buttonBox, &QDialogButtonBox::rejected, this, &MainWindow::on_actionExit_Valve_Program_triggered);

    // === Settings page UI elements setup ===
    ui->speed_lineEdit->setText(QString::number(stepper->getSpeed()));
    ui->microstep_lineEdit->setText(QString::number(stepper->getMicrostepping()));

    connect(ui->cal_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_cal_clicked()));
    connect(ui->settings_buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::on_settingsOk_clicked);

    // === Absolute move UI elements setup ===
    ui->occlusionPctSet_lineEdit->setText(QString::number(stepper->getPercentOpen()));

    connect(ui->fullOpen_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullOpen_clicked()));
    connect(ui->fullClose_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_fullClose_clicked()));
    connect(ui->pctMove_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_absMove_clicked()));

    // === Relative move UI elements setup ===
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

    // Set default values for quick move fields
    ui->qu1_lineEdit->setText("200");
    ui->qu2_lineEdit->setText("50");
    ui->qu3_lineEdit->setText("50");
    ui->qu4_lineEdit->setText("200");

    ui->qu1_comboBox->setCurrentIndex(0); // Open Valve
    ui->qu2_comboBox->setCurrentIndex(0); // Open Valve
    ui->qu3_comboBox->setCurrentIndex(1); // Close Valve
    ui->qu4_comboBox->setCurrentIndex(1); // Close Valve

    connect(ui->relative_commandLinkButton, SIGNAL(clicked()), this, SLOT(on_relMove_clicked()));
    connect(ui->qu1_commandLinkButton, &QCommandLinkButton::clicked, this, &MainWindow::on_quickMove1_clicked);
    connect(ui->qu2_commandLinkButton, &QCommandLinkButton::clicked, this, &MainWindow::on_quickMove2_clicked);
    connect(ui->qu3_commandLinkButton, &QCommandLinkButton::clicked, this, &MainWindow::on_quickMove3_clicked);
    connect(ui->qu4_commandLinkButton, &QCommandLinkButton::clicked, this, &MainWindow::on_quickMove4_clicked);


    // Set up log list view
    logListView = ui->log_listView;
    logListView->setModel(logModel);
    logMessages.clear();


    setUIEnabled(false);    // Disable UI elements before calibration
    timer->start(1000);     // Start timer to update UI elements
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

    if ((userSpeed < 1) || (userSpeed > MAX_SPEED)) {
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
    ui->stackedWidget->setCurrentIndex(3); // Switch to another page after calibration
}

void MainWindow::on_quickMove1_clicked() {
    QString direction = ui->qu1_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;
    int value = ui->qu1_lineEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        addLogMessage("Invalid input for quick move 1.");
        return;
    }

    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Quick Move 1 operation completed." << std::endl;
    });
    addLogMessage(QString("Quick move 1: %1 steps %2.").arg(value).arg(dir == 1 ? "open" : "closed"));
}

void MainWindow::on_quickMove2_clicked() {
    QString direction = ui->qu2_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;
    int value = ui->qu2_lineEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        addLogMessage("Invalid input for quick move 2.");
        return;
    }

    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Quick Move 2 operation completed." << std::endl;
    });
    addLogMessage(QString("Quick move 2: %1 steps %2.").arg(value).arg(dir == 1 ? "open" : "closed"));
}

void MainWindow::on_quickMove3_clicked() {
    QString direction = ui->qu3_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;
    int value = ui->qu3_lineEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        addLogMessage("Invalid input for quick move 3.");
        return;
    }

    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Quick Move 3 operation completed." << std::endl;
    });
    addLogMessage(QString("Quick move 3: %1 steps %2.").arg(value).arg(dir == 1 ? "open" : "closed"));
}

void MainWindow::on_quickMove4_clicked() {
    QString direction = ui->qu4_comboBox->currentText();
    int dir = (direction == "Open Valve") ? 1 : 0;

    bool ok;
    int value = ui->qu4_lineEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number");
        addLogMessage("Invalid input for quick move 4.");
        return;
    }

    stepper->moveStepsAsync(value, dir, []() {
        std::cout << "Quick Move 4 operation completed." << std::endl;
    });
    addLogMessage(QString("Quick move 4: %1 steps %2.").arg(value).arg(dir == 1 ? "open" : "closed"));
}
