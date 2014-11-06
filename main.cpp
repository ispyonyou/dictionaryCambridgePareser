#include "widget.h"
#include <QApplication>
#include <QNetworkProxyFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QNetworkProxyFactory::setUseSystemConfiguration( true );

    Widget w;
    w.show();

    return a.exec();
}
