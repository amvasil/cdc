#include "project.h"
#include <QDebug>
Project::Project(QString _name)
{
    name = _name;
    current_test = NULL;
}


void Project::addConfiguration(Configuration *conf)
{
    configurations.append(conf);
}

void Project::addVariable(Variable *variable)
{
    variables.append(variable);
}

void Project::addTest(Test *test)
{
    tests.append(test);
    current_test = test;
}

void Project::setCurrentTest(Test *test)
{
    //добавить обработку переключения всего
    //TODO
    current_test = test;
}

void Project::updateVariable(Variable *var)
{

    for(int i=0;i<variables.size();i++)
    {
        qDebug()<<"found var"<<variables.at(i)->getName();
        if(variables.at(i)->getName() == var->getName())
        {
            variables.at(i)->values.clear();
            variables.at(i)->values.append(var->getValues());
            return;
        }
    }
    variables.append(var); //maybe shouldn't be here
}

Test *Project::getCurrentTest()
{
    return current_test;
}

void Project::Vars2XMLfile(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Text))
        throw "Ошибка. Не удалось открыть "+filename+" для записи";
    QXmlStreamWriter wr(&f);
    wr.setAutoFormatting(true);

    wr.writeStartDocument();
    wr.writeStartElement("variables");
    for(int i=0;i<variables.size();i++)
    {
        variables.at(i)->getXML(&wr);
    }
    wr.writeEndElement();
    wr.writeEndDocument();

    f.close();
}
void Project::Configs2XMLfiles()
{
    for(int i=0;i<configurations.size();i++)
    {
        QString filename = configurations.at(i)->getName().append(".conf");
        QFile f(filename);
        if(!f.open(QIODevice::WriteOnly|QIODevice::Text))
            throw "Ошибка. Не удалось открыть "+filename+" для записи";

        QXmlStreamWriter wr(&f);
        wr.setAutoFormatting(true);
        wr.writeStartDocument();
        for(int i=0;i<configurations.size();i++)
        {
            configurations.at(i)->getXML(&wr);
        }
        wr.writeEndDocument();
        f.close();
    }

}
void Project::Tests2XMLfiles()
{
    for(int i=0;i<tests.size();i++)
    {
        QString filename = tests.at(i)->getName().append(".test");
        QFile f(filename);
        if(!f.open(QIODevice::WriteOnly|QIODevice::Text))
            throw "Ошибка. Не удалось открыть "+filename+" для записи";

        QXmlStreamWriter wr(&f);
        wr.setAutoFormatting(true);
        wr.writeStartDocument();
        for(int i=0;i<tests.size();i++)
        {
            tests.at(i)->getXML(&wr);
        }
        wr.writeEndDocument();
        f.close();
    }
}
void Project::XMLfile2Test(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
        throw "Ошибка. Не удалось открыть "+filename+" для чтения";
    QXmlStreamReader rd(&f);
    tests.append(new Test(filename.remove(".test")));
    tests.back()->readXML(&rd,&configurations,&variables);
    if(rd.hasError())
          throw "Ошибка при анализе XML";
    current_test = tests.back();
    f.close();
}
void Project::XMLfile2Config(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
        throw "Ошибка. Не удалось открыть "+filename+" для чтения";
    QXmlStreamReader rd(&f);
    configurations.append(new Configuration(filename.remove(".conf")));
    configurations.back()->readXML(&rd,&variables);
    if(rd.hasError())
          throw "Ошибка при анализе XML";
    f.close();
}

void Project::clear()
{
    while(!tests.isEmpty())
    {
        delete tests.takeLast();
    }
    while(!configurations.isEmpty())
    {
        delete configurations.takeLast();
    }
    while(!variables.isEmpty())
    {
        delete variables.takeLast();
    }
    current_test = NULL;
}
void Project::XMLfile2Vars(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
        throw "Ошибка. Не удалось открыть "+filename+" для чтения";
    QXmlStreamReader rd(&f);
    while(!rd.atEnd())
    {
          rd.readNext();
          if(rd.isStartElement()&&rd.name()=="variables")
          {
              while(!(rd.isEndElement()&&rd.name()=="variables"))
              {
                    variables.append(new Variable);
                    variables.back()->readXML(&rd);
              }
              variables.pop_back();
          }
    }
    if(rd.hasError())
          throw "Ошибка при анализе XML";
    f.close();
}
Configuration* Project::atConfiguration(int n)
{
    if(n<configurations.size())
    {
        return configurations.at(n);
    }
}
Variable* Project::atVariable(int n)
{
    if(n<variables.size())
    {
        return variables.at(n);
    }
}
Test* Project::atTest(int n)
{
    if(n<tests.size())
    {
        return tests.at(n);
    }
}

void Project::removeConfiguration(int n)
{
    if(n<configurations.size())
    {
        configurations.removeAt(n);
    }
}

void Project::removeTest(int n)
{
    if(n<tests.size())
    {
        tests.removeAt(n);
    }
}
void Project::removeVariable(int n)
{
    if(n<variables.size())
    {
        variables.removeAt(n);
    }
}
Project::~Project()
{
    for(int i=0;i<tests.size();i++)
    {
        delete tests.at(i);
    }
    for(int i=0;i<configurations.size();i++)
    {
        delete configurations.at(i);
    }
    for(int i=0;i<variables.size();i++)
    {
        delete variables.at(i);
    }
}
