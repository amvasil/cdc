#ifndef PITAYA_H
#define PITAYA_H
#include "generator.h"
#include "oscilloscope.h"
class Configuration;
class Pitaya
{
private:
    int number;                     // Номер питайи
    bool master;                    // Мастер
    bool enabled;                   // Вкл/выкл
    Generator* generators[2];       // Генераторы
    Oscilloscope* oscilloscopes[2]; // Осциллографы
public:


// Конструктор
    Pitaya(int n);
// Операции для изменения полей
    void setMaster(bool);
    void setEnabled(bool);
//  Геттеры
     bool isEnabled();
     bool isMaster();
     int getNumber();
     Generator *getGenerator(int number);
     Oscilloscope *getOscilloscope(int number);
     friend class Configuration;
};

#endif // PITAYA_H
