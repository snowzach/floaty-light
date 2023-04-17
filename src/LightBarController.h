#ifndef __LIGHTBAR_CONTROLLER_H__
#define __LIGHTBAR_CONTROLLER_H__

#include <Arduino.h>
#include <Logger.h>
#include "config.h"
#include "VescData.h"
#include <Adafruit_NeoPixel.h>

#define LOG_TAG_LIGHTBAR "LightBar"

class LightBarController : public Adafruit_NeoPixel {
    public:
        LightBarController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData);
        void init();
        void loop();

    private:
        AdcState lastAdcState;
        unsigned long lastAdcStateChange;
        AdcState mapSwitchState(uint16_t intState, boolean isAdc1Enabled);
        uint32_t getBatteryChargeColor(double percent);
        uint32_t getDutyCycleColor(double percent);
        VescData *vescData;
};

#endif