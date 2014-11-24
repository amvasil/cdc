#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QDebug>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "inttypes.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pr = NULL;
    client = NULL;
    count_updated_vars = 0;

    ui->widget->setAxisTitle(0,"V");
    ui->widget->setAxisTitle(2,"time, s");
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_initButton_clicked()
{
    pr = new Project("project_1");
    Configuration *config = new Configuration("config_1");
    config->getPitaya(0)->setEnabled(true);
    config->setEnabledGenerator(0,0,1);
    config->setEnabledOscilloscopes(0,0,1);
    pr->addConfiguration(config);
    Variable *var1 = new Variable("v_ocs");
    config->assignToOscilloscope(0,0,var1);
    Variable *var2 = new Variable("v_gen");
    for(int i=0;i<16384;i++){
        var2->values.append(1.5*(sin(2*M_PI*i/16384.0)));
    }
//    for(int i=0;i<8192;i++){
//        var2->values.append((double)i/8192.0*4.0-2.0);
//    }
//    for(int i=8191;i<=0;i--){
//        var2->values.append((double)i/8192.0*4.0-2.0);
//    }
    double freq = 1000;
    config->assignToGenerator(0,0,var2,freq);
    Test *test = new Test("test_1");
    test->setConfiguration(config);
    test->setDuration(0.002);
    test->setGeneratorPause(10);
    test->setPowerOnPause(1);
    pr->addTest(test);
    pr->addVariable(var1);
    pr->addVariable(var2);

}

void MainWindow::on_saveButton_clicked()
{
    pr->Vars2XMLfile("vars.dat");
    pr->Configs2XMLfiles();
    pr->Tests2XMLfiles();
}

void MainWindow::on_openButton_clicked()
{
    if(!pr)
        pr = new Project("project_1");
    pr->clear();
    qDebug()<<"1";
    pr->XMLfile2Vars("vars.dat");
    qDebug()<<"2";
    pr->XMLfile2Config("config_1.conf");
    qDebug()<<"3";
    pr->XMLfile2Test("test_1.test");
    qDebug()<<"4";
}



void MainWindow::on_pushButton_clicked()
{
    client->sendCommand("START");
}

void MainWindow::on_pushButton_2_clicked()
{
    QList<Variable*> vars = pr->getCurrentTest()->getConfiguration()->getMeasuredVariables();
    for(int i=0;i<vars.size();i++)
    {
        client->sendRequest("GETVAR",vars.at(i)->getName());
    }
    count_updated_vars = 0;
}

void MainWindow::slot_variables_received()
{
    if(client->getReceivedType()=="TESTDONE")
    {
        qDebug()<<"test done";
        return;
    }
    if(client->getReceivedType()!="VAR")
    {
        qDebug()<<"received not variable:"+client->getReceivedType();
        return;
    }
    Variable *var = new Variable;
    QString vardata = client->getReceivedData();
    var->readXMLstring(vardata);
    pr->updateVariable(var);
    count_updated_vars++;
    if(count_updated_vars==pr->getCurrentTest()->getConfiguration()->getMeasuredVariables().size())
    {
        count_updated_vars=0;
        qDebug()<<"received all vars"<<var->getSize()<<var->getName();
        int buffer_size = 16384;
        double adc_time = 1/125000000.0f;
        long int cycles_count = pr->getCurrentTest()->getDuration()/adc_time;
        int presc_values[] = {1,8,64,1024,8192,65536};
        for(int i=0;i<6;i++)
        {
            if((double)cycles_count/(double)presc_values[i]<=buffer_size)
            {
                prescaler = presc_values[i];
                qDebug()<<"prescaler is"<<prescaler;
                break;
            }

        }
        repaintPlot();
    }
}

void MainWindow::repaintPlot()
{
    Variable *var = pr->getCurrentTest()->getConfiguration()->getOscilloscopes(0)[0]->getAssignedVar();
    if(var->getValues().isEmpty())
    {
        qDebug()<<"error: var empty";
        return;
    }

    ui->widget->setTitle(var->name);
    QwtPlotCurve *curve = new QwtPlotCurve("Curve");
    double max_time = pr->getCurrentTest()->getDuration();
    QVector<double> time;
    for(int i=0;i<16384;i++)
    {
        double t = (double)i/(double)125000000L*(double)prescaler;
//        if(t<=max_time)
          time.append(t);
    }
    QVector<double> values = var->values.mid(0,time.size()).toVector();
    //QVector<double> values = pr->getCurrentTest()->getConfiguration()->getGenerators(0)[0]->getAssignedVar()->values.toVector();
    curve->setSamples(time,values);
    curve->attach(ui->widget);
    ui->widget->setAxisAutoScale(0);
    ui->widget->setAxisScale(1,-2.0,2.0);
    ui->widget->replot();
}

void MainWindow::on_connectButton_clicked()
{
    client = new NetClient(this,QHostAddress("192.168.1.1"),2325);
    connect(client,SIGNAL(received_data()),this,SLOT(slot_variables_received()));
    connect(client,SIGNAL(connected()),this,SLOT(slot_connected()));
    client->bind();
    client->connectToServer(QHostAddress("192.168.1.100"),2325);
}

void MainWindow::slot_connected()
{
    qDebug()<<"connected";
}

void MainWindow::on_transmitButton_clicked()
{
    if(!pr || !client)
        return;
    //ui->transmitButton->setEnabled(false);

    Test *test = pr->getCurrentTest();
    Configuration *conf = test->getConfiguration();
    QList<Variable*> vars = conf->getAssignedVariables();
    for(int i=0;i<vars.size();i++)
    {
        client->sendVariable(vars.at(i)->getXMLstring());
    }



}

void MainWindow::on_pushButton_4_clicked() //config
{
    Configuration *conf = pr->getCurrentTest()->getConfiguration();
    client->sendConfiguration(conf->getXMLstring());
}

void MainWindow::on_pushButton_3_clicked() //test
{
    Test *test = pr->getCurrentTest();
    client->sendTest(test->getXMLstring());
}


