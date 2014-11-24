#include "configuration.h"

Configuration::Configuration(QString _name)
{
    name = _name;
    pitayas[0]=new Pitaya(0);
    pitayas[1]=new Pitaya(1);




    for(int i=0;i<4;i++)
    {
        csupplies.append(new ControlledSupply(i+1,(i<2)?3.0:10.0));
    }
    uncsupplies.append(new UncontrolledSupply(5,3.3));
    uncsupplies.append(new UncontrolledSupply(5,5));
    uncsupplies.append(new UncontrolledSupply(5,9));
    uncsupplies.append(new UncontrolledSupply(5,20));
    uncsupplies.append(new UncontrolledSupply(5,24));

    uncsupplies.append(new UncontrolledSupply(6,-3.3));
    uncsupplies.append(new UncontrolledSupply(6,-5));
    uncsupplies.append(new UncontrolledSupply(6,-9));
    uncsupplies.append(new UncontrolledSupply(6,-20));
    uncsupplies.append(new UncontrolledSupply(6,-24));
}
void Configuration::initSample()
{
    pitayas[0]->generators[0]->setEnabled(true);
    pitayas[0]->generators[1]->setEnabled(true);
    pitayas[0]->oscilloscopes[0]->setEnabled(true);
    pitayas[0]->oscilloscopes[1]->setEnabled(true);
}

void Configuration::assignToGenerator(int pitaya, int channel, Variable *var, double freq)
{
    pitayas[pitaya]->generators[channel]->assigned_var = var;
    pitayas[pitaya]->generators[channel]->setFrequency(freq);


}

void Configuration::assignToOscilloscope(int pitaya, int channel, Variable *var)
{
    pitayas[pitaya]->oscilloscopes[channel]->assigned_var = var;


}

Oscilloscope **Configuration::getOscilloscopes(int pitaya)
{
    return pitayas[pitaya]->oscilloscopes;
}

Generator **Configuration::getGenerators(int pitaya)
{
    return pitayas[pitaya]->generators;
}

Pitaya *Configuration::getPitaya(int number)
{
    return pitayas[number];
}
QString Configuration::getXMLstring()
{
    QString res;
    QXmlStreamWriter wr(&res);
    getXML(&wr);
    return res;
}
void Configuration::getXML(QXmlStreamWriter *wr)
{
    wr->setAutoFormatting(true);
    wr->writeStartElement("config");
    wr->writeAttribute("name",name);
    // Про питайи
    wr->writeStartElement("pitayas");
    Pitaya *pitaya;
    // Поменять на 2, когда будет 2
    for(int j=0;j<2;j++)
    {
        if(pitayas[j]!=NULL)
            pitaya=pitayas[j];
        else continue;
        if(pitaya->isEnabled())
        {
            wr->writeStartElement("pitaya");
            wr->writeAttribute("number",QString::number(pitaya->getNumber()));
            wr->writeAttribute("master",pitaya->isMaster()?"1":"0");
            for(int k=0;k<2;k++)
            {
                if(pitaya->generators[k]!=NULL&&pitaya->generators[k]->isEnabled())
                {
                    wr->writeStartElement("generator");
                    wr->writeAttribute("channel",QString::number(pitaya->generators[k]->getChannel()));
                    wr->writeEndElement();
                }
            }
            for(int k=0;k<2;k++)
            {
                if(pitaya->oscilloscopes[k]!=NULL&&pitaya->oscilloscopes[k]->isEnabled())
                {
                    wr->writeStartElement("oscilloscope");
                    wr->writeAttribute("channel",QString::number(pitaya->oscilloscopes[k]->getChannel()));
                    wr->writeEndElement();
                }
            }
            wr->writeEndElement();
        }
    }
    wr->writeEndElement();
    // Про источники
    wr->writeStartElement("ControlledSupply");
    ControlledSupply* cs;
    for(int j=0;j<csupplies.size();j++)
    {
        cs=csupplies.at(j);
        if(cs->isEnabled())
        {
            wr->writeStartElement("csupplies");
            wr->writeAttributes(cs->getAttributes());
            wr->writeAttribute("var_name",cs->assigned_var==NULL?"":cs->assigned_var->getName());
            wr->writeAttribute("fuse",QString::number(cs->getFuse()));
            wr->writeAttribute("does_measure",cs->isDoesMeasure()?"1":"0");
            wr->writeAttribute("negative",cs->isNegative()?"1":"0");
            wr->writeEndElement();
        }

    }
    wr->writeEndElement();

    wr->writeStartElement("UncontrolledSupply");
    UncontrolledSupply* uncs;
    for(int j=0;j<uncsupplies.size();j++)
    {
        uncs=uncsupplies.at(j);
        if(uncs->isEnabled())
        {
            wr->writeStartElement("uncsupplies");
            wr->writeAttributes(uncs->getAttributes());
            wr->writeEndElement();
        }
    }
    wr->writeEndElement();
    wr->writeEndElement();
}

QString Configuration::getName()
{
    return name;
}
void Configuration::readXML(QXmlStreamReader *rd,QList<Variable*> *lst)
{
    bool ok=true;           // Проверка перевода чисел
    Pitaya *tpit;
    QXmlStreamAttributes temp_attr;
    rd->readNext();
    while(!rd->atEnd()&&!(rd->isEndElement()&&
                          (rd->name()=="config"||rd->name()=="configurations")))
    {
        if(rd->isStartElement()&&rd->name()=="config")
            name=rd->attributes().value("name").toString();
        if(rd->isStartElement()&&
                (rd->name()=="ControlledSupply"||
                 rd->name()=="UncontrolledSupply"||
                 rd->name()=="pitayas"))
        {
                rd->readNext();
                while(!rd->isEndElement())
                {
                    if(rd->name()=="csupplies")
                    {
//                        csupplies.append(new ControlledSupply(temp_attr.value("number").toInt(&ok),
//                                                              temp_attr.value("voltage").toDouble(&ok)));
                        ControlledSupply *cs = getControlledSypply(temp_attr.value("number").toInt(&ok));
                        cs->setVoltage(temp_attr.value("voltage").toDouble(&ok));
                        setControlledSupply(cs);
                        temp_attr=rd->attributes();
                        for(int i=0;i<lst->size();i++)
                        {
                            if(lst->at(i)->getName()==temp_attr.value("var_name"))
                                csupplies.back()->assigned_var=lst->at(i);
                        }
                        csupplies.back()->setEnabled(temp_attr.value("enabled").toInt(&ok));
                        csupplies.back()->setFuse(temp_attr.value("fuse").toDouble(&ok));
                        csupplies.back()->setDoesMeasure(temp_attr.value("does_measure").toInt(&ok));
                        csupplies.back()->setNegative(temp_attr.value("negative").toInt(&ok));
                        rd->readNext();
                    }
                    if(rd->name()=="uncsupplies")
                    {
                        temp_attr=rd->attributes();
                        setUncontrolledSupplyEnabled(temp_attr.value("number").toInt(&ok),
                                                     temp_attr.value("voltage").toDouble(&ok),true);
                        rd->readNext();

                    }
                    /*if(rd->name()=="generator")
                    {
                        if(num_gen>1) throw "В XML описано больше 2 генераторов одной из питай";
                        tpit->generators[rd->re]=new Generator(rd->attributes().value("channel").toInt(&ok));
                        num_gen++;
                        rd->readNext();
                    }
                    if(rd->name()=="oscilloscope")
                    {
                        if(num_osc>1) throw "В XML описано больше 2 осциллографов одной из питай";
                        pitayas[n]->oscilloscopes[num_osc]=new Oscilloscope(rd->attributes().value("channel").toInt(&ok));
                        num_osc++;
                        rd->readNext();
                    }*/
                    if(rd->name()=="pitaya")
                    {
                        //if(n>1) throw "В XML описано больше 2 питай";

                        temp_attr=rd->attributes();
                        tpit=pitayas[temp_attr.value("number").toInt()];
                        tpit->setMaster(temp_attr.value("master").toInt(&ok));
                        tpit->setEnabled(true);
                    }

                    if(!ok)
                        throw "Ошибка чтения XML";
                    rd->readNext();
                }
            }
            rd->readNext();
        }
        if(rd->hasError())
            throw "Ошибка при анализе XML";
}
void Configuration::setEnabledGenerator(int num_pitaya, int num_channel,bool w)
{
    pitayas[num_pitaya]->generators[num_channel]->setEnabled(w);
}
void Configuration::setEnabledOscilloscopes(int num_pitaya, int num_channel,bool w)
{
    pitayas[num_pitaya]->oscilloscopes[num_channel]->setEnabled(w);
}

void Configuration::setPitayaEnabled(int number, bool enabled)
{
    for(int i=0;i<2;i++)
    {
        if(pitayas[number]->generators[i]->isEnabled() || pitayas[number]->oscilloscopes[i]->isEnabled())
            throw InternalException("turning off pitaya while devices still on");
    }
    pitayas[number]->setEnabled(enabled);
}

ControlledSupply *Configuration::getControlledSypply(int number)
{
    return csupplies.at(number);
}

void Configuration::setControlledSupply(ControlledSupply *supply)
{
    for(int i=0;i<csupplies.size();i++)
    {
        if(csupplies.at(i)->number == supply->getNumber()){
            delete csupplies.at(i);
            csupplies.replace(i,supply);
        }
    }
}

void Configuration::setUncontrolledSupplyEnabled(int number, int voltage,bool enabled)
{
    double v = voltage;
    if(voltage<0)
        v = -voltage;
    for(int i=0;i<uncsupplies.size();i++)
    {
        if(uncsupplies.at(i)->number == number && uncsupplies.at(i)->voltage == v)
            uncsupplies.at(i)->setEnabled(enabled);
    }
}
