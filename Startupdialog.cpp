#include "Startupdialog.h"
#include "ui_Startupdialog.h"
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);

    ui->image_label->setPixmap(QPixmap(":/images/blueLED.png"));
    ui->image_label->show();
}

StartupDialog::~StartupDialog()
{
    delete ui;
}
