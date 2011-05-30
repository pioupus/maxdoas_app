#include <QtGui/QApplication>

#include "mainwindow.h"
#include "maxdoassettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("MaxDoas");
    a.setOrganizationName("CentroDeCienciasDeLaAtmosfera_UNAM");
    a.setOrganizationDomain("www.atmosfera.unam.mx");
    MainWindow w;
    w.show();

    return a.exec();
}
