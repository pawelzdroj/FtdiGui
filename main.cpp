#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.resize(450,300);
//    window.decorate();
    window.show();

    return a.exec();
}
