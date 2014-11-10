#include "mainwindow.h"
#include <QApplication>
#include <QNetworkProxyFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QNetworkProxyFactory::setUseSystemConfiguration( true );

    MainWindow w;
    w.show();

    return a.exec();
}

