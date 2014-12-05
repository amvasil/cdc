#include "supply.h"

Supply::Supply(int n, double v)
{
    number = n;
    voltage = v;
}
void Supply::setEnabled(bool w)
{
    enabled=w;
}
bool Supply::isEnabled()
{
    return enabled;
}
QXmlStreamAttributes Supply::getAttributes()
{
    QXmlStreamAttributes res;
    res.append("enabled",enabled?"1":"0");
    res.append("number",QString::number(number));
    res.append("voltage",QString::number(voltage));
    return res;
}
int Supply::getNumber()
{
    return number;
}
double Supply::getVoltage()
{
    return voltage;
}
void Supply::setNumber(int n)
{
    number=n;
}
void Supply::setVoltage(double v)
{
    voltage=v;
}

ControlledSupply::ControlledSupply(int n, double v, double f, bool dm, bool neg):Supply(n,v)
{
    assigned_var=NULL;
    fuse=f;
    does_measure=dm;
    negative=neg;
    enabled=false;
}

void ControlledSupply::setFuse(double f)
{
    fuse=f;
}
void ControlledSupply::setDoesMeasure(bool w)
{
    does_measure=w;
}
void ControlledSupply::setNegative(bool w)
{
    negative=w;
}
bool ControlledSupply::isDoesMeasure()
{
    return does_measure;
}
bool ControlledSupply::isNegative()
{
    return negative;
}
double ControlledSupply::getFuse()
{
    return fuse;
}
void ControlledSupply::assign(Variable *var)
{
    assigned_var=var;
    enabled=true;
}

UncontrolledSupply::UncontrolledSupply(int n, double v):Supply(n,v)
{
    enabled=false;
}
