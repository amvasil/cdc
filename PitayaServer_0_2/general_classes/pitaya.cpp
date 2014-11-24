#include "pitaya.h"

Pitaya::Pitaya(int n)
{
    enabled=false;
    master=false;
    number=n;
    if(number==0)
        master = true;
    generators[0]=new Generator(0);
    generators[1]=new Generator(1);
    oscilloscopes[0]=new Oscilloscope(0);
    oscilloscopes[1]=new Oscilloscope(1);

}
void Pitaya::setMaster(bool w)
{
    master=w;
}
void Pitaya::setEnabled(bool w)
{
    enabled=w;
}
bool Pitaya::isEnabled()
{
    return enabled;
}

bool Pitaya::isMaster()
{
    return master;
}
int Pitaya::getNumber()
{
    return number;
}

Generator *Pitaya::getGenerator(int number)
{
    return generators[number];
}

Oscilloscope *Pitaya::getOscilloscope(int number)
{
    return oscilloscopes[number];
}
