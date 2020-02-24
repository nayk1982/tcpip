#include <QApplication>
#include <QStyleFactory>
#include "nayk/AppCore"
#include "mainwindow.h"
//==============================================================================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!app_core::initializeApplication( "tcpip",
                                         "nayk",
                                         "nayk1982.github.com"
                                         )) {
        return 0;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
//==============================================================================
