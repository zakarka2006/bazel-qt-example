#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setAttribute(Qt::WA_OpaquePaintEvent);
    window.setAttribute(Qt::WA_NoSystemBackground);
    window.show();
    return app.exec();
}
