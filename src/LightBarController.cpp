#include "LightBarController.h"
#include <vector>
#include <Logger.h>

const std::vector<int> lightBarIndexADC1 = LIGHT_BAR_LED_INDEX_ADC1;
const std::vector<int> lightBarIndexADC2 = LIGHT_BAR_LED_INDEX_ADC2;
const std::vector<int> lightBarIndexBatteryCharge = LIGHT_BAR_LED_INDEX_BATTERY_CHARGE;
const std::vector<int> lightBarIndexDutyCycle = LIGHT_BAR_LED_INDEX_DUTY_CYCLE;

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

LightBarController::LightBarController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData)
        : Adafruit_NeoPixel(pixels, pin, type) {
    this->vescData = vescData;
}

void LightBarController::init() {
    Logger::notice(LOG_TAG_LIGHTBAR, "initializing ...");
    begin();
    show();
}

AdcState lastAdcState = ADC_INIT;
unsigned long lastAdcStateChange = 0;

// updates the light bar, depending on the LED count
void LightBarController::loop() {
    if(abs(vescData->erpm) > LIGHT_BAR_OFF_ERPM) {
        for(int i=0; i<LIGHT_BAR_NUMPIXELS; i++) 
          setPixelColor(i, 0, 0, 0, 0);
        show();
        return;
    }

    for(int i : lightBarIndexBatteryCharge) setPixelColor(i, getBatteryChargeColor(vescData->batteryLevel));
    for(int i : lightBarIndexDutyCycle) setPixelColor(i, getDutyCycleColor(vescData->dutyCycle*100.0));

    AdcState adcState = mapSwitchState(vescData->switchState, vescData->adc1 > vescData->adc2);
    if (adcState != lastAdcState) {
        switch (adcState) {
            case ADC_NONE:
                for(int i : lightBarIndexADC1) setPixelColor(i, LIGHT_BAR_ADC_OFF_COLOR_RED, LIGHT_BAR_ADC_OFF_COLOR_GREEN, LIGHT_BAR_ADC_OFF_COLOR_BLUE, LIGHT_BAR_ADC_OFF_COLOR_WHITE);
                for(int i : lightBarIndexADC2) setPixelColor(i, LIGHT_BAR_ADC_OFF_COLOR_RED, LIGHT_BAR_ADC_OFF_COLOR_GREEN, LIGHT_BAR_ADC_OFF_COLOR_BLUE, LIGHT_BAR_ADC_OFF_COLOR_WHITE);
                break;
            case ADC_HALF_ADC1:
                for(int i : lightBarIndexADC1) setPixelColor(i, LIGHT_BAR_ADC_ON_COLOR_RED, LIGHT_BAR_ADC_ON_COLOR_GREEN, LIGHT_BAR_ADC_ON_COLOR_BLUE, LIGHT_BAR_ADC_ON_COLOR_WHITE);
                for(int i : lightBarIndexADC2) setPixelColor(i, LIGHT_BAR_ADC_OFF_COLOR_RED, LIGHT_BAR_ADC_OFF_COLOR_GREEN, LIGHT_BAR_ADC_OFF_COLOR_BLUE, LIGHT_BAR_ADC_OFF_COLOR_WHITE);
                break;
            case ADC_HALF_ADC2:
                for(int i : lightBarIndexADC1) setPixelColor(i, LIGHT_BAR_ADC_OFF_COLOR_RED, LIGHT_BAR_ADC_OFF_COLOR_GREEN, LIGHT_BAR_ADC_OFF_COLOR_BLUE, LIGHT_BAR_ADC_OFF_COLOR_WHITE);
                for(int i : lightBarIndexADC2) setPixelColor(i, LIGHT_BAR_ADC_ON_COLOR_RED, LIGHT_BAR_ADC_ON_COLOR_GREEN, LIGHT_BAR_ADC_ON_COLOR_BLUE, LIGHT_BAR_ADC_ON_COLOR_WHITE);
                break;
            case ADC_FULL:
                for(int i : lightBarIndexADC1) setPixelColor(i, LIGHT_BAR_ADC_ON_COLOR_RED, LIGHT_BAR_ADC_ON_COLOR_GREEN, LIGHT_BAR_ADC_ON_COLOR_BLUE, LIGHT_BAR_ADC_ON_COLOR_WHITE);
                for(int i : lightBarIndexADC2) setPixelColor(i, LIGHT_BAR_ADC_ON_COLOR_RED, LIGHT_BAR_ADC_ON_COLOR_GREEN, LIGHT_BAR_ADC_ON_COLOR_BLUE, LIGHT_BAR_ADC_ON_COLOR_WHITE);
                break;
        }
    }
    lastAdcState = adcState;
    show();
}

AdcState LightBarController::mapSwitchState(uint16_t intState, boolean isAdc1Enabled) {
    switch (intState) {
        case 0:
            return AdcState::ADC_NONE;
        case 1:
            return isAdc1Enabled ? AdcState::ADC_HALF_ADC1 : AdcState::ADC_HALF_ADC2;
        case 2:
            return AdcState::ADC_FULL;
        default:
            Logger::error(LOG_TAG_LIGHTBAR, "Unknown switch state");
    }
    return AdcState::ADC_NONE;
}

int scaleRange(int x, int srcFrom, int srcTo, int destFrom, int destTo) {
    long int a = ((long int) destTo - (long int) destFrom) * ((long int) x - (long int) srcFrom);
    long int b = (long int) srcTo - (long int) srcFrom;
    return (a / b) + destFrom;
}

uint32_t LightBarController::getBatteryChargeColor(double percent) {
    return ColorHSV(MAX(scaleRange(percent * 100, 0, 100, -30, 85), 0) * 256.0, 255, LIGHT_BAR_BATTERY_BRIGHTNESS);
}

uint32_t LightBarController::getDutyCycleColor(double percent) {
    return ColorHSV(MAX(MIN(scaleRange(percent, 0, 100, 128, -30), 85), 0) * 256.0, 255, LIGHT_BAR_DUTY_CYCLE_BRIGHTNESS);
}
