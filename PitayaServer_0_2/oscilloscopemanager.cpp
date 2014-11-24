#include "oscilloscopemanager.h"
#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "hardware/main_osc.h"
#include "hardware/fpga_osc.h"
//#include "version.h"
}

OscilloscopeManager::OscilloscopeManager(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer()));
    timer->setSingleShot(true);
    retries = 150000;
}

void OscilloscopeManager::setTime(double t)
{
    double adc_time = 1/125000000.0f;
    long int cycles_count = t/adc_time;
    int presc_values[] = {1,8,64,1024,8192,65536};
    for(int i=0;i<6;i++)
    {
        if((double)cycles_count/(double)presc_values[i]<=buffer_size)
        {
            prescaler = presc_values[i];
            break;
        }

    }
    time = adc_time*prescaler*buffer_size;

}

void OscilloscopeManager::initialize(Oscilloscope *osc[])
{
    if(osc[0]->isEnabled())
    {
        enabled[0] = true;
        vars[0] = osc[0]->getAssignedVar();
    }
    else
    {
        enabled[0] = false;
    }
    if(osc[1]->isEnabled())
    {
        enabled[1] = true;
        vars[1] = osc[1]->getAssignedVar();
    }
    else
    {
        enabled[1] = false;
    }
}

void OscilloscopeManager::measure()
{
    float t_params[PARAMS_NUM] = { 0, 1e6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    t_params[GAIN1_PARAM] = 0; //lv
    t_params[GAIN2_PARAM] = 0; //lv
    int presc_values[] = {1,8,64,1024,8192,65536};
    int presc = 1;
    for(int i=0;i<6;i++)
    {
        if(presc_values[i]==prescaler)
        {
            presc = i;
            break;
        }
    }
    t_params[TIME_RANGE_PARAM] = presc; //decimation #1
    t_params[EQUAL_FILT_PARAM] = 0; //no filtering
    t_params[SHAPE_FILT_PARAM] = 0;

    /* Initialization of Oscilloscope application */
    if(rp_app_init() < 0) {
        fprintf(stderr, "rp_app_init() failed!\n");
        return;
    }

    /* Setting of parameters in Oscilloscope main module */
    if(rp_set_params((float *)&t_params, PARAMS_NUM) < 0) {
        fprintf(stderr, "rp_set_params() failed!\n");
        return;
    }

    timer->start(1000);


}

void OscilloscopeManager::on_timer()
{
    int sig_num, sig_len;
    int ret_val;
    float **s;
    s = (float **)malloc(SIGNALS_NUM * sizeof(float *));
    for(int i = 0; i < SIGNALS_NUM; i++) {
        s[i] = (float *)malloc(SIGNAL_LENGTH * sizeof(float));
    }
    if((ret_val = rp_get_signals(&s, &sig_num, &sig_len)) >= 0) {
        /* Signals acquired in s[][]:
             * s[0][i] - TODO
             * s[1][i] - Channel ADC1 raw signal
             * s[2][i] - Channel ADC2 raw signal
             */
        for(int i=0;i<2;i++)
        {
            if(enabled[i])
            {
                vars[i]->values.clear();
                vars[i]->values.reserve(buffer_size);
                for(int j=0;j<buffer_size;j++)
                {
                    vars[i]->values.append(s[i+1][j]);
                }
            }
        }
        if(rp_app_exit() < 0) {
            fprintf(stderr, "rp_app_exit() failed!\n");
            return;
        }
        emit finished();
        return;
    }

    if(retries-- == 0) {
        fprintf(stderr, "Signal scquisition was not triggered!\n");
        if(rp_app_exit() < 0) {
            fprintf(stderr, "rp_app_exit() failed!\n");
            return;
        }
        emit error();
    }
    else
        timer->start(1000);


}
