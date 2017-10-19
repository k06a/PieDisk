#include <QApplication>
#include "DirInfo.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<DirInfo*>("DirInfo*");

    MainWindow w;
    w.show();

    return a.exec();
}
