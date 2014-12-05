#include "configuration.h"

Configuration::Configuration(QString _name)
{
    name = _name;
    pitayas[0]=new Pitaya(0);
    pitayas[1]=new Pitaya(1);

    enabled_DigGen=false;
    for(int i=0;i<8;i++)
    {
        digout[i]=false;
        enabled_digout[i]=false;
    }

    // Добавляем источники
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
                    wr->writeEmptyElement("generator");
                    wr->writeAttribute("channel",QString::number(pitaya->generators[k]->getChannel()));
                }
            }
            for(int k=0;k<2;k++)
            {
                if(pitaya->oscilloscopes[k]!=NULL&&pitaya->oscilloscopes[k]->isEnabled())
                {
                    wr->writeEmptyElement("oscilloscope");
                    wr->writeAttribute("channel",QString::number(pitaya->oscilloscopes[k]->getChannel()));
                }
            }
            wr->writeEndElement();
        }
    }
    wr->writeEndElement();

    // Про цифровые выходы
    QString temp;
    for(int i=0;i<8;i++)
    {
        if(enabled_digout[i])
            temp.append(QString::number(i)+';');
    }
    if(temp.size()!=0)
    {
        temp.remove(temp.size()-1,1);
        wr->writeStartElement("DigOut");
        wr->writeCharacters(temp);
        wr->writeEndElement();
    }

    // Про генератор
    wr->writeEmptyElement("DigGen");
    wr->writeAttribute("enabled",enabled_DigGen?"1":"0");

    // Про источники
    wr->writeStartElement("ControlledSupplies");
    ControlledSupply* cs;
    for(int j=0;j<csupplies.size();j++)
    {
        cs=csupplies.at(j);
        if(cs->isEnabled())
        {
            wr->writeEmptyElement("csupply");
            wr->writeAttributes(cs->getAttributes());
            wr->writeAttribute("var_name",cs->assigned_var==NULL?"":cs->assigned_var->getName());
            wr->writeAttribute("fuse",QString::number(cs->getFuse()));
            wr->writeAttribute("does_measure",cs->isDoesMeasure()?"1":"0");
            wr->writeAttribute("negative",cs->isNegative()?"1":"0");
        }
    }
    wr->writeEndElement();

    wr->writeStartElement("UncontrolledSupplies");
    UncontrolledSupply* uncs;
    for(int j=0;j<uncsupplies.size();j++)
    {
        uncs=uncsupplies.at(j);
        if(uncs->isEnabled())
        {
            wr->writeEmptyElement("uncsupply");
            wr->writeAttributes(uncs->getAttributes());
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
                          (rd->name()=="config")))
    {
        rd->readNext();
        if(rd->isStartElement())
        {
            if(rd->name()=="config")
            {
                name=rd->attributes().value("name").toString();
                continue;
            }
            if(rd->name()=="csupply")
            {
                temp_attr=rd->attributes();
                ControlledSupply *cs = getControlledSypply(temp_attr.value("number").toString().toInt(&ok));
                cs->setVoltage(temp_attr.value("voltage").toString().toDouble(&ok));
                cs->setEnabled(temp_attr.value("enabled").toString().toInt(&ok));
                cs->setFuse(temp_attr.value("fuse").toString().toDouble(&ok));
                cs->setDoesMeasure(temp_attr.value("does_measure").toString().toInt(&ok));
                cs->setNegative(temp_attr.value("negative").toString().toInt(&ok));

                for(int i=0;i<lst->size();i++)
                {
                    if(lst->at(i)->getName()==temp_attr.value("var_name"))
                        cs->assigned_var=lst->at(i);
                }
                continue;
            }
            if(rd->name()=="uncsupply")
            {
                temp_attr=rd->attributes();
                setUncontrolledSupplyEnabled(temp_attr.value("number").toString().toInt(&ok),
                                             temp_attr.value("voltage").toString().toDouble(&ok),true);
                continue;
            }
            if(rd->name()=="pitaya")
            {
                temp_attr=rd->attributes();
                tpit=pitayas[temp_attr.value("number").toString().toInt()];
                tpit->setMaster(temp_attr.value("master").toString().toInt(&ok));
                tpit->setEnabled(true);
                while(!rd->atEnd()&&!(rd->isEndElement()&&
                                      (rd->name()=="pitaya")))
                {
                    rd->readNext();
                    if(rd->name()=="generator")
                    {
                        tpit->getGenerator(rd->attributes().value("channel").toString().toInt(&ok))->setEnabled(true);

                    }
                    if(rd->name()=="oscilloscope")
                    {
                         tpit->getOscilloscope(rd->attributes().value("channel").toString().toInt(&ok))->setEnabled(true);
                    }
                }
            }
            if(rd->name()=="DigOut")
            {
                rd->readNext();
                QStringList lst=rd->text().toString().split(';');
                for(int i=0;i<lst.size();i++)
                {
                    if(lst.at(i).toInt()<8)
                    {
                        enabled_digout[lst.at(i).toInt()]=true;
                    }
                }
            }
            if(rd->name()=="DigGen")
            {
                enabled_DigGen=rd->attributes().value("enabled").toString().toInt();
            }
        }
        if(rd->hasError())
            throw "Ошибка при анализе XML";
    }
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
    for(int i=0;i<csupplies.size();i++)
    {
        if(csupplies.at(i)->number == number){
            return csupplies.at(i);
        }
    }
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

QList<Variable *> Configuration::getAssignedVariables()
{
    QList<Variable*> temp;
    for(int p=0;p<2;p++)
    {
        for(int ch=0;ch<2;ch++)
        {
            if(pitayas[p]->oscilloscopes[ch]->getAssignedVar()!=NULL && pitayas[p]->oscilloscopes[ch]->isEnabled())
                temp.append(pitayas[p]->oscilloscopes[ch]->getAssignedVar());
            if(pitayas[p]->generators[ch]->getAssignedVar()!=NULL && pitayas[p]->oscilloscopes[ch]->isEnabled())
                temp.append(pitayas[p]->generators[ch]->getAssignedVar());
        }
    }
    for(int i=0;i<csupplies.size();i++)
    {
        if(csupplies.at(i)->isEnabled() && csupplies.at(i)->isDoesMeasure() && csupplies.at(i)->assigned_var != NULL)
        {
            temp.append(csupplies.at(i)->assigned_var);
        }
    }
    return temp;
}

QList<Variable *> Configuration::getMeasuredVariables()
{
    QList<Variable*> temp;
    for(int p=0;p<2;p++)
    {
        for(int ch=0;ch<2;ch++)
        {
            if(pitayas[p]->oscilloscopes[ch]->getAssignedVar()!=NULL && pitayas[p]->oscilloscopes[ch]->isEnabled())
                temp.append(pitayas[p]->oscilloscopes[ch]->getAssignedVar());
        }
    }
    for(int i=0;i<csupplies.size();i++)
    {
        if(csupplies.at(i)->isEnabled() && csupplies.at(i)->isDoesMeasure() && csupplies.at(i)->assigned_var != NULL)
        {
            temp.append(csupplies.at(i)->assigned_var);
        }
    }
    return temp;
}

void Configuration::setEnabledDigOut(int n, bool en)
{
    if(n<8)
    {
        enabled_digout[n]=en;
    }
}
void Configuration::setDigOut(int n, bool m)
{
    if(n<8)
    {
        digout[n]=m;
        setEnabledDigOut(n,m);
    }
}
bool Configuration::isEnabledDigOut(int n)
{
    if(n<8)
    {
        return enabled_digout[n];
    }
}
bool Configuration::getDigOut(int n)
{
    if(n<8)
    {
        return digout[n];
    }
}
void Configuration::setEnabledDigGen(bool w)
{
    enabled_DigGen=w;
}
bool Configuration::isEnabledDigGen()
{
    return enabled_DigGen;
}
