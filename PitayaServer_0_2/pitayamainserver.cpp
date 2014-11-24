#include "pitayamainserver.h"
#include <stdio.h>
#include <stdlib.h>
PitayaMainServer::PitayaMainServer(QObject *parent) :
    QObject(parent)
{
    gen = new GeneratorManager(this);
    osc = new OscilloscopeManager(this);
    connect(osc,SIGNAL(finished()),this,SLOT(on_finished_acquire()));
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(on_pause_finished()));

    config = NULL;
    test = NULL;

    led_state = 0;



}

void PitayaMainServer::init(Configuration *conf, Test *_test)
{
    config = conf;
    test = _test;
    pause_time = test->getGeneratorPause();

}

//void PitayaMainServer::initSample()
//{
//    config = new Configuration("config_1");
//    config->initSample();
//    test = new Test("config_1");
//    test->setConfiguration(config);
//    test->initSample();
//    pause_time = 1000;

//}

void PitayaMainServer::configure()
{
    osc->setTime(test->getDuration());
    gen->time = osc->time;
    osc->initialize(config->getOscilloscopes(0));
    gen->initialize(config->getGenerators(0));

}

void PitayaMainServer::startTest()
{
    gen->start();
    timer->start(pause_time);

}

void PitayaMainServer::parseVariable(QString &str)
{
    Variable *var = new Variable;
    var->readXMLstring(str);
    for(int i=0;i<variables.size();i++)
        if(variables.at(i)->name==var->name)
        {
            delete variables.takeAt(i);
            break;
        }
    variables.append(var);
}

void PitayaMainServer::parseConfiguration(QString &str)
{
    QXmlStreamReader rd(str);
    if(config)
        delete config;
    config = new Configuration("cc");
    config->readXML(&rd,&variables);

}

void PitayaMainServer::parseTest(QString &str)
{
    QXmlStreamReader rd(str);
    if(test)
        delete test;
    test = new Test("tt");
    QList<Configuration*> clst;
    clst.append(config);
    test->readXML(&rd,&clst,&variables);
    pause_time = test->getGeneratorPause();
}

void PitayaMainServer::startListening(QHostAddress ip, quint16 port)
{
    server = new NetServer(this,ip,port);
    connect(server,SIGNAL(received_data()),this,SLOT(slot_data_received()));
    connect(server,SIGNAL(connected()),this,SLOT(slot_connected()));
    server->listen();
}

void PitayaMainServer::turnLedOn(int number)
{
    if(number>7 || number<0)
        return;
    led_state = led_state | (1<<number);
    QString command("monitor 0x40000030 0x");
    command.append(QString::number(led_state,16));
    system(command.toLocal8Bit().data());
}

void PitayaMainServer::turnLedOff(int number)
{
    if(number>7 || number<0)
        return;
    led_state = led_state & !(1<<number);
    QString command("monitor 0x40000030 0x");
    command.append(QString::number(led_state,16));
    system(command.toLocal8Bit().data());
}

void PitayaMainServer::slot_connected()
{
    turnLedOn(1);
}

void PitayaMainServer::on_pause_finished()
{
    turnLedOn(2);
    osc->measure();
}

void PitayaMainServer::on_finished_acquire()
{
    printf("finished acquisition\n");
    server->sendCommand("TESTDONE");
    turnLedOff(2);
    //qApplication->quit();
}

void PitayaMainServer::slot_data_received()
{
    QString type = server->getReceivedType();
    if(type=="VAR")
    {
        parseVariable(server->getReceivedData());
        return;
    }
    if(type=="CONFIG")
    {
        parseConfiguration(server->getReceivedData());
        return;
    }
    if(type=="TEST")
    {
        parseTest(server->getReceivedData());
        return;
    }
    if(type == "START")
    {
        if(!config || !test)
        {
            server->sendCommand("FALSESTART");
            return;
        }
        startTest();
        return;

    }
    if(type=="GETVAR")
    {
        Variable *var=NULL;
       for(int i=0;i<variables.size();i++)
       {
           if(variables.at(i)->getName()==server->getReceivedData())
           {
               var = variables.at(i);
               break;
           }
       }
       if(!var)
       {
           server->sendCommand("ERROR");
           return;
       }
       server->sendVariable(var->getXMLstring());
       return;
    }
}
