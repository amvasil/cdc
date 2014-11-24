#include <QCoreApplication>
#include "pitayamainserver.h"
#include <stdio.h>
QCoreApplication* qApplication;
int main(int argc, char *argv[])
{
    qApplication = new QCoreApplication (argc, argv);
    PitayaMainServer *ms = new PitayaMainServer;
    ms->startListening(QHostAddress("192.168.1.100"),2425);
    int res = qApplication->exec();
    delete qApplication;
    delete ms;
    return res;
}
