#ifndef VARIABLE_H
#define VARIABLE_H
#include <QString>
#include <QList>
#include <QXmlStreamWriter>
#include <QRegExp>
class Variable
{
public:
    QList<double> values; //usually 16384
    QString name;
public:
// Конструктор
    Variable(QString name="new_var");
// Работа с полями
    void addValue(double);
// Геттеры
    QString getName();
    QList<double>& getValues();
    double getValue(int n);
    int getSize();
//  XML
    QString getXMLstring();
    void getXML(QXmlStreamWriter*wr);
    void readXMLstring(QString &);
    void readXML(QXmlStreamReader*);

};

#endif // VARIABLE_H
