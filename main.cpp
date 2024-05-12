#include "mainwindow.h"
#include "Startupdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StartupDialog popup;
    if (popup.exec() != QDialog::Accepted) {
        return 0; // Exit app
    }

    MainWindow w;
    w.show();

    return a.exec();
}
