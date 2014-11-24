#include "variable.h"
#include <QStringList>
Variable::Variable(QString name)
{
    this->name=name;
}
void Variable::addValue(double mean)
{
    values.append(mean);
}
QString Variable::getName()
{
    return name;
}
QList<double> &Variable::getValues()
{
    return values;
}
double Variable::getValue(int n)
{
    if(n>=values.size()) throw "Превышение размера массива";
    return values.at(n);
}

void Variable::getXML(QXmlStreamWriter *wr)
{
    wr->setAutoFormatting(true);
    wr->writeStartElement("var");
    wr->writeAttribute("name",name);
    int s = values.size()-1;
    for(int j=0;j<=s;j++)
    {
        wr->writeCharacters(QString::number(values.at(j)).append(
                                (j==s)?"":","));
    }
    wr->writeCharacters("\n\t");
    wr->writeEndElement();
}
int Variable::getSize()
{
    return values.size();
}
void Variable::readXML(QXmlStreamReader* rd)
{
        rd->readNext();
        while(!rd->isEndElement()&&!rd->atEnd())
        {
            if(rd->isStartElement()&&rd->name()=="var")
            {
                name=rd->attributes().value("name").toString();
                rd->readNext();
                QString temp=rd->text().toString();
                if(temp.contains(QRegExp("[0-9]")))
                {
                    QStringList lst = temp.split(",");
                    values.clear();
                    for(int i=0;i<lst.size();i++)
                    {
                        if(lst.at(i).isEmpty())
                            continue;
                        bool ok=0;
                        values.append(lst.at(i).toDouble(&ok));
                        if(!ok)
                            throw "Ошибка чтения переменных";
                    }
                }
            }
            rd->readNext();
        }
        if(rd->hasError())
            throw "Ошибка при анализе XML";
}
void Variable::readXMLstring(QString& str)
{
    QXmlStreamReader rd(str);
    readXML(&rd);
}
