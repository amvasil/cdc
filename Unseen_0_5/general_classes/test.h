#ifndef TEST_H
#define TEST_H

#include "configuration.h"
#include "variable.h"
/*
 * Тест – описание воздействий и список получаемых сигналов.
 * Содержит сведения о конфигурации. Может содержать инструкции
 * по последовательной модификации воздействий, например, для снятия частотных характеристик.
 */
class Test
{
private:
    QString name;
    double powerOnPause;
    double generatorPause;
    double duration;
    Configuration* configuration; //указатели
    QList<Variable*> used_variables; //переменные, используемые в тесте. Хранит указатели на переменные в Project.
public:
    void addVariable(Variable *var);
    Configuration* getConfiguration();
    //QList<PostScript*> scripts;
// Конструктор
    Test(QString _name);
    //void addVariable(Variable *var);
// Работа с полями
    void setConfiguration(Configuration *conf);
    void setPowerOnPause(double);
    void setGeneratorPause(double);
    void setDuration(double);

// Геттеры
    QString getName();
    double getPowerOnPause();
    double getGeneratorPause();
    double getDuration();
// XML
    QString getXMLstring();
    void getXML(QXmlStreamWriter*);
    void readXML(QXmlStreamReader*,QList<Configuration *>* lst,QList<Variable *>* var);
// Прочее
    //void initSample();

};

#endif // TEST_H
