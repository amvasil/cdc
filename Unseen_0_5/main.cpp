#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int j=2,i=1;
    for(int i=0;i<=50;i++)
        j=i;
    qDebug()<<j;

    w.show();

    return a.exec();
}
