#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/icons/appIcon.svg"));

    MainWindow mainWindow(&app);

    mainWindow.show();
    mainWindow.setWindowTitle(QObject::tr("AntimAR"));
    mainWindow.resize(1024,768);

    return app.exec();
}
