#include "oscilloscope.h"

Oscilloscope::Oscilloscope(int ch)
{
    channel = ch;
    enabled=false;
    assigned_var = NULL;
}
void Oscilloscope::setEnabled(bool w)
{
    enabled=w;
}
void Oscilloscope::setPostprocess(QString postpr_str)
{
    postprocess=postpr_str;
}
bool Oscilloscope::isEnabled()
{
    return enabled;
}
QString Oscilloscope::getPostprocess()
{
    return postprocess;
}
int Oscilloscope::getChannel()
{
    return channel;
}

Variable *Oscilloscope::getAssignedVar()
{
    return assigned_var;
}

void Oscilloscope::assign(Variable *var)
{
    assigned_var = var;
}
