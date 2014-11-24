#ifndef PITAYAMAINSERVER_H
#define PITAYAMAINSERVER_H

#include <QObject>
#include "general_classes/configuration.h"
#include "general_classes/test.h"
#include <QString>
#include <QList>
#include "oscilloscopemanager.h"
#include "generatormanager.h"
#include "QCoreApplication"
#include "net/netserver.h"
extern QCoreApplication* qApplication;
class PitayaMainServer : public QObject
{
    Q_OBJECT
public:
    explicit PitayaMainServer(QObject *parent = 0);
    void init(Configuration *conf, Test *_test);
    //void initSample();
    void configure();
    void startTest();
    void parseVariable(QString& str);
    void parseConfiguration(QString& str);
    void parseTest(QString& str);
    void startListening(QHostAddress ip,quint16 port);
private:
    QTimer *timer;
    Configuration *config;
    Test *test;
    OscilloscopeManager *osc;
    GeneratorManager *gen;
    double pause_time;
    NetServer *server;
    QList<Variable*> variables;
    unsigned char led_state;
    void turnLedOn(int number);
    void turnLedOff(int number);
signals:

public slots:
    void slot_connected();
    void on_pause_finished();
    void on_finished_acquire();
    void slot_data_received();
};

#endif // PITAYAMAINSERVER_H
