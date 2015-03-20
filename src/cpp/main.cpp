#include "mainwindow.h"

#include <QtPlugin>
#include <QApplication>

QT_BEGIN_NAMESPACE
Q_IMPORT_PLUGIN(pnp_basictools)
QT_END_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

