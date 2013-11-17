#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Taiko");
    a.setApplicationName("ThaiDictionary");
    MainWindow w;
    w.show();
    
    return a.exec();
}
