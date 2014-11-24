#ifndef GENERATOR_H
#define GENERATOR_H
#include "variable.h"
#include <QString>
class Configuration;
class Generator
{
private:
    int channel;              // 0 or 1
    double frequency;         // Частота
    QString modification;     // на будущее - автомодификация для измерений.
    bool enabled;          // Вкл/выкл
    Variable* assigned_var;   // Значения
public:


// Конструктор
    Generator(int ch);
// Операции для изменения полей
    void setEnabled(bool);
    void setModification(QString mod_str);
    void setFrequency(double freq);
//  Геттеры
    bool isEnabled();
    QString getModification();
    double getFrequency();
    int getChannel();
    Variable *getAssignedVar();
    void assign(Variable *var);
    friend class Configuration;
};

#endif // GENERATOR_H
