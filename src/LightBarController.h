#ifndef __LIGHTBAR_CONTROLLER_H__
#define __LIGHTBAR_CONTROLLER_H__

#include <Arduino.h>
#include <Logger.h>
#include "config.h"
#include "ILedController.h"

#define LOG_TAG_LIGHTBAR "LightBar"

#ifdef LIGHT_BAR_ENABLED
 #include <Adafruit_NeoPixel.h>
 extern Adafruit_NeoPixel lightPixels;
#endif //LIGHT_BAR_ENABLED

enum  ErrorCode { ERR_NONE };
enum  AdcState { ADC_NONE, ADC_HALF_ADC1, ADC_HALF_ADC2, ADC_FULL, ADC_INIT };

class LightBarController : public Adafruit_NeoPixel {
    public:
        LightBarController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData);
        void init();
        void loop();

    private:
        int calcVal(int value);
        AdcState mapSwitchState(uint16_t intState, boolean isAdc1Enabled);
        VescData *vescData;
};

#endif