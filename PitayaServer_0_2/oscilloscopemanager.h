#ifndef OSCILLOSCOPEMANAGER_H
#define OSCILLOSCOPEMANAGER_H

#include <QObject>
#include "general_classes/oscilloscope.h"
#include <QTimer>
class OscilloscopeManager : public QObject
{
    Q_OBJECT

    QTimer *timer;
    int retries;
public:
    explicit OscilloscopeManager(QObject *parent = 0);
    Variable *vars[2];
    double time;
    int prescaler;
    static const int buffer_size = 16384;
    bool enabled[2];
    void setTime(double t);
    void initialize(Oscilloscope *osc[2]);
    void measure();
signals:
    void error();
    void finished();
public slots:
    void on_timer();
};

#endif // OSCILLOSCOPEMANAGER_H
