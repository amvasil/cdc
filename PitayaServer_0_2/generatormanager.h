#ifndef GENERATORMANAGER_H
#define GENERATORMANAGER_H

#include <QObject>
#include <general_classes/generator.h>
extern "C"{
#include "hardware/fpga_awg.h"
}
#include <QTimer>
class GeneratorManager : public QObject
{
    Q_OBJECT

    /** AWG FPGA parameters */
        typedef struct {
            int32_t  offsgain;   ///< AWG offset & gain.
            uint32_t wrap;       ///< AWG buffer wrap value.
            uint32_t step;       ///< AWG step interval.
        } awg_param_t;
private:
    int32_t *data;
    awg_param_t *params;
    QTimer* timer;
    void write_data_fpga(uint32_t ch,
                             const int32_t *data,
                             const awg_param_t *awg);
    static const uint32_t n = 16*1024;  //buffer size
    static const double c_max_frequency = 62.5e6;
    /** Minimal signal frequency [Hz] */
    static const double c_min_frequency = 0;
    /** Maximal signal amplitude [Vpp] */
    static const double c_max_amplitude = 2.0;

public:
    explicit GeneratorManager(QObject *parent = 0);
    Variable *vars[2];
    double time;

    static const int buffer_size = 16384;
    double frequency[2];
    void initialize(Generator *gen[2]);
    bool enabled[2];
    void start();
signals:

public slots:
    void on_timer_out();
};

#endif // GENERATORMANAGER_H
