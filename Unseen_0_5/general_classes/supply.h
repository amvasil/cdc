#ifndef SUPPLY_H
#define SUPPLY_H
#include "variable.h"
class Configuration;
class Supply
{
protected:
    double voltage;         // Напряжение
    int number;             // 1 to 4 controlled, 5 & 6 uncontrolled
    bool enabled;           // Вкл/выкл
public:
// Конструктор
    Supply(int n,double v);
// Операции для изменения полей
    void setEnabled(bool);
    void setNumber(int);
    void setVoltage(double);
//  Геттеры
    bool isEnabled();
    int getNumber();
    double getVoltage();
// XML
    QXmlStreamAttributes getAttributes();
    friend class Configuration;
};

class ControlledSupply: public Supply
{
private:
    double fuse;                    // Предохранитель
    bool does_measure;
    bool negative;
    //const double min_voltage=3.0;
    //const double max_voltage=20.0;
public:
   Variable* assigned_var;         // Значения

// Конструктор
    ControlledSupply(int n,double v=0,double f=1,bool dm=0,bool neg=0);
// Операции для изменения полей
    void setFuse(double);
    void setDoesMeasure(bool);
    void setNegative(bool);
    void assign(Variable* var);
//  Геттеры
    bool isDoesMeasure();
    bool isNegative();
    double getFuse();
};

class UncontrolledSupply: public Supply
{
public:
// Конструктор
    UncontrolledSupply(int n,double v);
};
#endif // SUPPLY_H
