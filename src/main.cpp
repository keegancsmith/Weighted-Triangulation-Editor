#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QString filename;
    if (argc == 2)
        filename = argv[1];

    QApplication app(argc, argv);
    MainWindow window(filename);
    window.show();

    return app.exec();
}
