#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H
#include "variable.h"
#include <QString>
class Configuration;
class Oscilloscope
{
private:
    int channel;            // Каналы
    QString postprocess;    //на будущее - постобработка на стороне pitaya.
    bool enabled;           // Вкл/выкл
    Variable *assigned_var; // Значения
public:


// Конструктор
    Oscilloscope(int ch);
// Операции для изменения полей
        void setEnabled(bool);
//  Геттеры
        bool isEnabled();
        QString getPostprocess();
        void setPostprocess(QString postpr_str);
        int getChannel();
        Variable *getAssignedVar();
        void assign(Variable *var);
        friend class Configuration;
};

#endif // OSCILLOSCOPE_H
