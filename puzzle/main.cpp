#include <QApplication>

#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(puzzle);

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
