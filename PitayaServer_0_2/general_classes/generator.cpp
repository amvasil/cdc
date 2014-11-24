#include "generator.h"

Generator::Generator(int ch)
{
    channel = ch;
    enabled=false;
    assigned_var = NULL;
}
void Generator::setEnabled(bool w)
{
    enabled=w;
}

void Generator::setModification(QString mod_str)
{
    modification = mod_str;
}

void Generator::setFrequency(double freq)
{
    //add validation
    frequency = freq;
}


bool Generator::isEnabled()
{
    return enabled;
}

QString Generator::getModification()
{
    return modification;
}

double Generator::getFrequency()
{
    return frequency;
}
int Generator::getChannel()
{
    return channel;
}

Variable *Generator::getAssignedVar()
{
    return assigned_var;
}

void Generator::assign(Variable *var)
{
    assigned_var = var;
}
