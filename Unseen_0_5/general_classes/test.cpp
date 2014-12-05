#include "test.h"
#include "cmath"
void Test::addVariable(Variable *var)
{
    used_variables.append(var);
}

Configuration *Test::getConfiguration()
{
    return configuration;
}

Test::Test(QString _name)
{
    name = _name;
}
/*
void Test::addVariable(Variable *var)
{
    used_variables.append(var);
}
*/
void Test::setConfiguration(Configuration *conf)
{
    //Переназначить переменные
    //Некоторые устройства, на которые они назначены, могут быть отключены
    //TODO

    configuration = conf;
}
/*
void Test::initSample()
{
    Variable *var = new Variable();
    var->name = "var_osc1";
    used_variables.append(var);
    configuration->assignToOscilloscope(0,0,var);

    var = new Variable();
    var->name = "var_osc2";
    used_variables.append(var);
    configuration->assignToOscilloscope(0,1,var);

    QList<double> array;
    for(int i=0;i<16384;i++)
        array.append(2.0*sin(i*2*M_PI/16384.0));
    var = new Variable();
    var->name = "var_gen1";
    var->values = array;
    configuration->assignToGenerator(0,0,var,1000);


    QList<double> array2;
    for(int i=0;i<16384;i++)
        array2.append(1.0*cos(i*2*M_PI/16384.0));
    var = new Variable();
    var->name = "var_gen2";
    var->values = array2;
    configuration->assignToGenerator(0,1,var,1000);

    time = new TimeVariable;
    time->name = "time";
    time->duration = 100;


}
*/
QString Test::getName()
{
    return name;
}
QString Test::getXMLstring()
{
    QString res;
    QXmlStreamWriter wr(&res);
    getXML(&wr);
    return res;
}
void Test::getXML(QXmlStreamWriter *wr)
{
    Pitaya* pitaya;
    wr->setAutoFormatting(true);
    wr->writeStartElement("test");
    wr->writeAttribute("name",name);
    wr->writeAttribute("config",configuration->getName());
    wr->writeAttribute("generatorPause",QString::number(generatorPause));
    wr->writeAttribute("powerOnPause",QString::number(powerOnPause));
    wr->writeAttribute("duration",QString::number(duration));
    // Опишем DigOut
    wr->writeStartElement("DigOut");
    for(int i=0;i<8;i++)
    {
        if(configuration->isEnabledDigOut(i))
        {
            wr->writeStartElement("output");
            wr->writeAttribute("num",QString::number(i));
            wr->writeCharacters(configuration->getDigOut(i)?"1":"0");
            wr->writeEndElement();
        }
    }
    wr->writeEndElement();

    // Опишем питайи
    for(int j=0;j<2;j++)
    {
        pitaya=configuration->getPitaya(j);
        if(pitaya->isEnabled())
        {
            wr->writeStartElement("pitaya");
            wr->writeAttribute("number",QString::number(pitaya->getNumber()));
            wr->writeAttribute("master",pitaya->isMaster()?"1":"0");
            for(int k=0;k<2;k++)
            {
                if(configuration->getGenerators(j)[k]->isEnabled())
                {
                    wr->writeStartElement("generator");
                    wr->writeAttribute("channel",QString::number(configuration->getGenerators(j)[k]->getChannel()));
                    wr->writeAttribute("frequency",QString::number(configuration->getGenerators(j)[k]->getFrequency()));
                    wr->writeAttribute("var_name",configuration->getGenerators(j)[k]->getAssignedVar()->getName());
                    wr->writeTextElement("modification",configuration->getGenerators(j)[k]->getModification());
                    wr->writeEndElement();
                }
            }
            for(int k=0;k<2;k++)
            {
                if(configuration->getOscilloscopes(j)[k]->isEnabled())
                {
                    wr->writeStartElement("oscilloscope");
                    wr->writeAttribute("channel",QString::number(configuration->getOscilloscopes(j)[k]->getChannel()));
                    wr->writeAttribute("var_name",configuration->getOscilloscopes(j)[k]->getAssignedVar()==NULL?"":
                                                               configuration->getOscilloscopes(j)[k]->
                                                                                         getAssignedVar()->getName());
                    wr->writeTextElement("postprocess",configuration->getOscilloscopes(j)[k]->getPostprocess());
                    wr->writeEndElement();
                }
            }
            wr->writeEndElement();
        }
    }
    wr->writeEndElement();
}
void Test::readXML(QXmlStreamReader *rd, QList<Configuration *> *lst, QList<Variable *> *var)
{
    QXmlStreamAttributes temp_attr;
    Pitaya *tpit;
    Generator *tgen;
    Oscilloscope *tosc;
    bool ok=true;
    rd->readNext();
    while(!rd->atEnd()&&!(rd->isEndElement()&& rd->name()=="test"))
    {
        if(rd->isEndElement())
        {
            rd->readNext();
            continue;
        }
        if(rd->name()=="test")
        {
                temp_attr=rd->attributes();
                name=temp_attr.value("name").toString();
                powerOnPause=temp_attr.value("powerOnPause").toString().toDouble();
                generatorPause=temp_attr.value("generatorPause").toString().toDouble();
                duration=temp_attr.value("duration").toString().toDouble();
                int i;
                for(i=0;i<lst->size();i++)
                {
                    if(lst->at(i)->getName()==temp_attr.value("config"))
                    {
                        configuration=lst->at(i);
                        break;
                    }
                }
                if(i==lst->size())
                    throw "Конфигурация не найдена";
        }
        if(rd->name()=="pitaya")
        {
            for(int i=0;i<2;i++)
            {
                if(configuration->getPitaya(i)->getNumber()==rd->attributes().value("number").toString().toInt())
                {
                    tpit=configuration->getPitaya(i);
                    break;
                }
            }
        }
        if(rd->name()=="generator")
        {
            for(int i=0;i<2;i++)
            {
                if(tpit->getGenerator(i)->getChannel()==rd->attributes().value("channel").toString().toInt())
                {
                    tgen=tpit->getGenerator(i);
                    break;
                }
            }
            tgen->setFrequency(rd->attributes().value("frequency").toString().toDouble(&ok));
            for(int i=0;i<var->size();i++)
            {
                if(var->at(i)->getName()==rd->attributes().value("var_name"))
                {
                    used_variables.append(var->at(i));
                    tgen->assign(used_variables.back());
                }
            }
            tgen->setEnabled(true);
        }
        if(rd->name()=="modification")
        {
            tgen->setModification(rd->text().toString());
        }
        if(rd->name()=="oscilloscope")
        {
            for(int i=0;i<2;i++)
            {
                if(tpit->getOscilloscope(i)->getChannel()==rd->attributes().value("channel").toString().toInt())
                {
                    tosc=tpit->getOscilloscope(i);
                    break;
                }
            }
            for(int i=0;i<var->size();i++)
            {
                if(var->at(i)->getName()==rd->attributes().value("var_name"))
                {
                    used_variables.append(var->at(i));
                    tosc->assign(used_variables.back());
                }
            }
            tosc->setEnabled(true);
        }
        if(rd->name()=="postprocess")
        {
            tosc->setPostprocess(rd->text().toString());
        }
        rd->readNext();
    }
    if(rd->hasError())
        throw "Ошибка при анализе XML";
}
double Test::getGeneratorPause()
{
    return generatorPause;
}
double Test::getPowerOnPause()
{
    return powerOnPause;
}
void Test::setGeneratorPause(double gp)
{
    generatorPause=gp;
}
void Test::setPowerOnPause(double pop)
{
    powerOnPause=pop;
}
void Test::setDuration(double d)
{
    duration=d;
}
double Test::getDuration()
{
    return duration;
}
