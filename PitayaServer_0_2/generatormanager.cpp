#include "generatormanager.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
void GeneratorManager::write_data_fpga(uint32_t ch, const int32_t *data, const GeneratorManager::awg_param_t *awg)
{


    uint32_t i;

    fpga_awg_init();

    if(ch == 0) {
        /* Channel A */
        g_awg_reg->state_machine_conf = 0x000041;
        g_awg_reg->cha_scale_off      = awg->offsgain;
        g_awg_reg->cha_count_wrap     = awg->wrap;
        g_awg_reg->cha_count_step     = awg->step;
        g_awg_reg->cha_start_off      = 0;

        for(i = 0; i < n; i++) {
            g_awg_cha_mem[i] = data[i];
        }
    } else {
        /* Channel B */
        g_awg_reg->state_machine_conf = 0x410000;
        g_awg_reg->chb_scale_off      = awg->offsgain;
        g_awg_reg->chb_count_wrap     = awg->wrap;
        g_awg_reg->chb_count_step     = awg->step;
        g_awg_reg->chb_start_off      = 0;

        for(i = 0; i < n; i++) {
            g_awg_chb_mem[i] = data[i];
        }
    }

    /* Enable both channels */
    /* TODO: Should this only happen for the specified channel?
         *       Otherwise, the not-to-be-affected channel is restarted as well
         *       causing unwanted disturbances on that channel.
         */
    g_awg_reg->state_machine_conf = 0x110011;

    fpga_awg_exit();

}

GeneratorManager::GeneratorManager(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_out()));
    data = NULL;
}

void GeneratorManager::initialize(Generator *gen[])
{
    if(gen[0]->isEnabled())
    {
        enabled[0] = true;
        vars[0] = gen[0]->getAssignedVar();
        frequency[0] = gen[0]->getFrequency();
    }
    else
    {
        enabled[0] = false;
    }
    if(gen[1]->isEnabled())
    {
        enabled[1] = true;
        vars[1] = gen[1]->getAssignedVar();
        frequency[1] = gen[1]->getFrequency();
    }
    else
    {
        enabled[1] = false;
    }
}

void GeneratorManager::start()
{
    timer->setInterval(time);
    timer->setSingleShot(true);
    params = new awg_param_t;
    /* Various locally used constants - HW specific parameters */
        const int dcoffs = -155;


        if(!data)
            data = new int32_t[n];
        for (int channel = 0;channel < 2;channel++)
        {

            if(!enabled[channel])
            {
                for(int i=0;i<n;i++)
                    data[i] = 0;
                //dummy for frequency
                params->offsgain = (dcoffs << 16) + 0x1fff;
                params->step = round(65536 * 50.0/c_awg_smpl_freq * n);
                params->wrap = round(65536 * (n-1));

            }
            else
            {

                /* This is where frequency is used... */
                params->offsgain = (dcoffs << 16) + 0x1fff;
                params->step = round(65536 * frequency[channel]/c_awg_smpl_freq * n);
                params->wrap = round(65536 * (n-1));

                uint32_t amp = 8192;

                if(vars[channel]->values.size()!=n)
                    return;

                for (int i=0;i<n;i++)
                {
                    data[i] = round(amp*vars[channel]->values.at(i)/c_max_amplitude);
                }

            }
            write_data_fpga(channel,data,params);
        }

        if(time>0)
        {
            timer->start(time);
        }
}

void GeneratorManager::on_timer_out()
{
    const int dcoffs = -155;
    for (int channel = 0;channel < 2;channel++)
    {

        for(int i=0;i<n;i++)
            data[i] = 0;
        //dummy for frequency
        params->offsgain = (dcoffs << 16) + 0x1fff;
        params->step = round(65536 * 50.0/c_awg_smpl_freq * n);
        params->wrap = round(65536 * (n-1));
        write_data_fpga(channel,data,params);

    }
}
