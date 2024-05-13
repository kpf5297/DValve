#include "mainwindow.h"
#include "Startupdialog.h"
#include <QStyleFactory>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication a(argc, argv);

    StartupDialog popup;
    if (popup.exec() != QDialog::Accepted) {
        return 0; // Exit app
    }

    MainWindow w;
    w.show();

    return a.exec();
}
