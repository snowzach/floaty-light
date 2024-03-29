#include "LightController.h"
#include <vector>

#define ABS(N) ((N<0)?(-N):(N))

LightController::LightController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData)
        : Adafruit_NeoPixel(pixels, pin, type) {
    this->vescData = vescData;
}

double map01(double x, double start, double end) {
    return (x * (end-start)) + start;
}

void LightController::init() {
    Logger::notice(LOG_TAG_LIGHT, "initializing ...");
    begin();
    show();
    direction = 0;
}

void LightController::loop() {

    long uptime = millis();
    if(uptime < LIGHT_BOOT_TIME_MS) {
        for(int x=0; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, bootColor(double(LIGHT_BOOT_TIME_MS-uptime) / double(LIGHT_BOOT_TIME_MS)));
    } else {
        AdcState adcState = mapSwitchState(vescData->switchState, vescData->adc1 > vescData->adc2);
        switch(vescData->state) {
            // Shutdown
            case STATE_STARTUP:
            case STATE_FAULT_STARTUP:
            case STATE_FAULT_SWITCH_HALF:
            case STATE_FAULT_SWITCH_FULL:
            case STATE_FAULT_QUICKSTOP:
                if (adcState != lastAdcState) {
                    switch (adcState) {
                        case ADC_NONE:
                            for(int x=0; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, Color(LIGHT_STOP_NONE_COLOR_RED,LIGHT_STOP_NONE_COLOR_GREEN,LIGHT_STOP_NONE_COLOR_BLUE,LIGHT_STOP_NONE_COLOR_WHITE));
                            break;
                        case ADC_HALF_ADC1:
                            for(int x=0; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, Color(LIGHT_STOP_ADC1_COLOR_RED,LIGHT_STOP_ADC1_COLOR_GREEN,LIGHT_STOP_ADC1_COLOR_BLUE,LIGHT_STOP_ADC1_COLOR_WHITE));
                            break;
                        case ADC_HALF_ADC2:
                            for(int x=0; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, Color(LIGHT_STOP_ADC2_COLOR_RED,LIGHT_STOP_ADC2_COLOR_GREEN,LIGHT_STOP_ADC2_COLOR_BLUE,LIGHT_STOP_ADC2_COLOR_WHITE));                        break;
                            break;
                    }
                    lastAdcState = adcState;
                }
                break;

            // Running
            default:
                if(vescData->erpm > ERPM_IDLE) {
                    if(direction < 255) direction += 1;
                } else if(vescData->erpm < -ERPM_IDLE) {
                    if(direction > -255) direction -= 1;
                } else {
                    // It's idle
                    if(direction) direction += (direction > 0 ? -1 : 1);  
                }

                for(int x=0; x < LIGHT_NUMPIXELS/2; x++) setPixelColor(x, getColor((direction+255)/511.0, map01(ABS(direction)/255.0, LIGHT_IDLE_BRIGHTNESS, 255.0)));
                for(int x=LIGHT_NUMPIXELS/2; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, getColor((255-direction)/511.0, map01(ABS(direction)/255.0, LIGHT_IDLE_BRIGHTNESS, 255.0)));
                lastAdcState = ADC_INIT;
        }
    }
    show();
}

uint32_t LightController::bootColor(double percent) {
    return Color(
        map01(percent, 0, LIGHT_BOOT_COLOR_RED),
        map01(percent, 0, LIGHT_BOOT_COLOR_GREEN),
        map01(percent, 0, LIGHT_BOOT_COLOR_BLUE),
        map01(percent, 0, LIGHT_BOOT_COLOR_WHITE)
    );
}

uint32_t LightController::getColor(double percent, double brightness) {
    return Color(
        map01(percent, LIGHT_REAR_COLOR_RED, LIGHT_FRONT_COLOR_RED) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_GREEN, LIGHT_FRONT_COLOR_GREEN) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_BLUE, LIGHT_FRONT_COLOR_BLUE) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_WHITE, LIGHT_FRONT_COLOR_WHITE) * (brightness / 255.0)
    );
}

AdcState LightController::mapSwitchState(uint16_t intState, boolean isAdc1Enabled) {
    switch (intState) {
        case 0:
            return AdcState::ADC_NONE;
        case 1:
            return isAdc1Enabled ? AdcState::ADC_HALF_ADC1 : AdcState::ADC_HALF_ADC2;
        case 2:
            return AdcState::ADC_FULL;
        default:
            Logger::error("VescData", "Unknown switch state");
    }
    return AdcState::ADC_NONE;
}
