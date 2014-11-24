#ifndef PROJECT_H
#define PROJECT_H
#include <QFile>
#include "test.h"
class Project
{
private:
    QString name;                           // Имя проекта
    Test *current_test; //текущий редактируемый или проводимый тест
    QList<Test*> tests;                     // Список тестов
    QList<Variable*> variables;             // Список переменных
    QList<Configuration*> configurations;   // Список конфигураций
public:


// Констуктор
    Project(QString _name);
//  Деструктор
// Работа с полями
    void addConfiguration(Configuration* conf);
    void addVariable(Variable *variable0);
    void addTest(Test *test);
    void setCurrentTest(Test* test);
//  XML
    void Vars2XMLfile(QString filename);
    void Tests2XMLfiles();
    void Configs2XMLfiles();

    void XMLfile2Vars(QString filename);
    void XMLfile2Test(QString filename);
    void XMLfile2Config(QString filename);

    void clear();
};

#endif // PROJECT_H
