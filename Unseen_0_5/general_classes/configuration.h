#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QStringList>
#include "pitaya.h"
#include "supply.h"
/*Конфигурация – описание электрической схемы подключения объекта тестирования к комплексу.
* Тесты, совместимые по конфигурации, могут запускаться последовательно без переконфигурирования комплекса
*/
class Configuration
{
private:
    QString name;                            // Имя
    QList<ControlledSupply*> csupplies;      // Управляемые
    QList<UncontrolledSupply*> uncsupplies;  // Неуправляемые
    Pitaya* pitayas[2];                      // Сcылки на pitaya

    bool digout[8];                            // Цифровые выходы одной из питай
    bool enabled_digout[8];                    // Вкл/Выкл
    bool enabled_DigGen;
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
    void setControlledSupply(ControlledSupply *supply);
    void setUncontrolledSupplyEnabled(int number, int voltage, bool enabled);

    QList<Variable*> getAssignedVariables();
    QList<Variable*> getMeasuredVariables();



// Цифровые выходы
    // Работа с полями
    void setEnabledDigOut(int n,bool en);
    // Установка значения автоматически включает этот do
    void setDigOut(int n,bool m=true);
    // Геттеры
    bool isEnabledDigOut(int n);
    bool getDigOut(int n);

// Генератор
    void setEnabledDigGen(bool);
    bool isEnabledDigGen();


// Геттеры
    Oscilloscope **getOscilloscopes(int pitaya);
    Generator **getGenerators(int pitaya);
    Pitaya *getPitaya(int number);
    QString getName();
    ControlledSupply *getControlledSypply(int number);
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
