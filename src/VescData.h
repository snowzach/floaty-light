//
// Created by David Anthony Gey on 14.10.22.
//

#ifndef RESCUE_VESCDATA_H
#define RESCUE_VESCDATA_H

#include <cstdint>

struct VescData {
    double dutyCycle = 0.0;
    double erpm = 0.0;
    double batteryLevel = 0.0;
    uint16_t switchState = 0;
    double adc1 = 0.0;
    double adc2 = 0.0;
};

#endif //RESCUE_VESCDATA_H
