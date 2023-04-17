#ifndef __LIGHT_CONTROLLER_H__
#define __LIGHT_CONTROLLER_H__

#include <Arduino.h>
#include <Logger.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "VescData.h"

#define LOG_TAG_LIGHT "Light"

class LightController : public Adafruit_NeoPixel {
    public:
        LightController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData);
        void init();
        void loop();

    private:
        AdcState lastAdcState;
        VescData *vescData;
        int direction;
        AdcState mapSwitchState(uint16_t intState, boolean isAdc1Enabled);
        uint32_t bootColor(double percent);
        uint32_t getColor(double percent, double brightness);
};

#endif