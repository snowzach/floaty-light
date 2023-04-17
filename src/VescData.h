//
// Created by David Anthony Gey on 14.10.22.
//

#ifndef RESCUE_VESCDATA_H
#define RESCUE_VESCDATA_H

#include <cstdint>

#define STATE_STARTUP 0
#define STATE_FAULT_SWITCH_HALF 8
#define STATE_FAULT_SWITCH_FULL 9
#define STATE_FAULT_STARTUP 11
#define STATE_FAULT_QUICKSTOP 13

enum  AdcState { ADC_NONE, ADC_HALF_ADC1, ADC_HALF_ADC2, ADC_FULL, ADC_INIT };

struct VescData {
    double dutyCycle = 0.0;
    double erpm = 0.0;
    double batteryLevel = 0.0;
    uint8_t state = 0;
    uint16_t switchState = 0;
    double adc1 = 0.0;
    double adc2 = 0.0;
};

#endif //RESCUE_VESCDATA_H
