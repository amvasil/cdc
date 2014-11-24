#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "pitaya.h"
#include "supply.h"
/*Конфигурация – описание электрической схемы подключения объекта тестирования к комплексу.
* Тесты, совместимые по конфигурации, могут запускаться последовательно без переконфигурирования комплекса
*/
class Configuration
{
private:
    QString name;                             // Имя
    QList<ControlledSupply*> csupplies;      // Управляемые
    QList<UncontrolledSupply*> uncsupplies;  // Неуправляемые
    Pitaya* pitayas[2];                      // Сcылки на pitaya
public:

// Конструктор
    Configuration(QString _name);
// Включение всех объектов 0 питайи
    void initSample();
//  Присвоить ген/осц нужные var
    void assignToGenerator(int pitaya, int channel, Variable *var,double freq);
    void setEnabledGenerator(int num_pitaya, int num_channel,bool w);
    void assignToOscilloscope(int pitaya, int channel, Variable *var);
    void setEnabledOscilloscopes(int num_pitaya, int num_channel,bool w);
    void setPitayaEnabled(int number,bool enabled);
    ControlledSupply *getControlledSypply(int number);
    void setControlledSupply(ControlledSupply *supply);
    void setUncontrolledSupplyEnabled(int number, int voltage, bool enabled);


// Геттеры
    Oscilloscope **getOscilloscopes(int pitaya);
    Generator **getGenerators(int pitaya);
    Pitaya *getPitaya(int number);
    QString getName();
// XML
    QString getXMLstring();
    void getXML(QXmlStreamWriter*);
    void readXML(QXmlStreamReader*, QList<Variable *> *lst);
};

class InternalException
{
public:
    QString what;
    InternalException(QString what_happened):what(what_happened){}
};

#endif // CONFIGURATION_H
