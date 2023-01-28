#ifndef __LIGHTBAR_CONTROLLER_H__
#define __LIGHTBAR_CONTROLLER_H__

#include <Arduino.h>
#include <Logger.h>
#include "config.h"
#include "ILedController.h"
#include <Adafruit_NeoPixel.h>

#define LOG_TAG_LIGHTBAR "LightBar"

enum  AdcState { ADC_NONE, ADC_HALF_ADC1, ADC_HALF_ADC2, ADC_FULL, ADC_INIT };

class LightBarController : public Adafruit_NeoPixel {
    public:
        LightBarController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData);
        void init();
        void loop();

    private:
        int calcVal(int value);
        AdcState mapSwitchState(uint16_t intState, boolean isAdc1Enabled);
        double cellVoltageToPercent(double v);
        double dutyCycleToPercent(double v);
        uint32_t getBatteryChargeColor(double percent);
        uint32_t getDutyCycleColor(double percent);
        VescData *vescData;
};

#endif