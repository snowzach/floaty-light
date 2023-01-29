#include "LightBarController.h"
#include <Adafruit_NeoPixel.h>

const std::vector<int> lightBarIndexADC1 = LIGHT_BAR_LED_INDEX_ADC1;
const std::vector<int> lightBarIndexADC2 = LIGHT_BAR_LED_INDEX_ADC2;
const std::vector<int> lightBarIndexBatteryCharge = LIGHT_BAR_LED_INDEX_BATTERY_CHARGE;
const std::vector<int> lightBarIndexDutyCycle = LIGHT_BAR_LED_INDEX_DUTY_CYCLE;

LightBarController::LightBarController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData)
        : Adafruit_NeoPixel(pixels, pin, type) {
    this->vescData = vescData;
}

void LightBarController::init() {
    Logger::notice(LOG_TAG_LIGHTBAR, "initializing ...");
    begin(); // This initializes the NeoPixel library.
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

    double charge = cellVoltageToPercent(vescData->inputVoltage / BATTERY_CELL_COUNT);
    for(int i : lightBarIndexBatteryCharge) setPixelColor(i, getBatteryChargeColor(charge));

    double dutyCycle = dutyCycleToPercent(vescData->dutyCycle);
    for(int i : lightBarIndexDutyCycle) setPixelColor(i, getDutyCycleColor(dutyCycle));

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
    //Serial.printf("Map Switchstate: %d %d\n", intState, isAdc1Enabled);
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

double LightBarController::cellVoltageToPercent(double v) {
    // This is a custom made table, it should be okay for LIPOs
    if (v >= 4.2) { return 1.0; }
    else if (v >= 4.15) { return 0.98; }
    else if (v >= 4.11) { return 0.97; }
    else if (v >= 4.08) { return 0.96; }
    else if (v >= 4.02) { return 0.95; }
    else if (v >= 3.98) { return 0.80; }
    else if (v >= 3.95) { return 0.70; }
    else if (v >= 3.91) { return 0.65; }
    else if (v >= 3.87) { return 0.60; }
    else if (v >= 3.85) { return 0.55; }
    else if (v >= 3.84) { return 0.50; }
    else if (v >= 3.82) { return 0.45; }
    else if (v >= 3.8) { return 0.40; }
    else if (v >= 3.39) { return 0.35; }
    else if (v >= 3.77) { return 0.30; }
    else if (v >= 3.75) { return 0.25; }
    else if (v >= 3.73) { return 0.20; }
    else if (v >= 3.71) { return 0.15; }
    else if (v >= 3.69) { return 0.10; }
    else if (v >= 3.61) { return 0.05; }
    else { return 0.00; }
}

double LightBarController::dutyCycleToPercent(double dc) {
    if(dc > 80) { return 1.0; }
    else if (dc >= 70) { return 0.90; }
    return (dc / 100.0);
}

double map01(double x, double start, double end) {
    return (x * (end-start)) + start;
}

uint32_t LightBarController::getBatteryChargeColor(double percent) {
    return Color(
        map01(percent, LIGHT_BAR_BATTERY_CHARGE_LOW_COLOR_RED, LIGHT_BAR_BATTERY_CHARGE_HIGH_COLOR_RED),
        map01(percent, LIGHT_BAR_BATTERY_CHARGE_LOW_COLOR_GREEN, LIGHT_BAR_BATTERY_CHARGE_HIGH_COLOR_GREEN),
        map01(percent, LIGHT_BAR_BATTERY_CHARGE_LOW_COLOR_BLUE, LIGHT_BAR_BATTERY_CHARGE_HIGH_COLOR_BLUE),
        map01(percent, LIGHT_BAR_BATTERY_CHARGE_LOW_COLOR_WHITE, LIGHT_BAR_BATTERY_CHARGE_HIGH_COLOR_WHITE)
    );
}

uint32_t LightBarController::getDutyCycleColor(double percent) {
    return Color(
        map01(percent, LIGHT_BAR_DUTY_CYCLE_LOW_COLOR_RED, LIGHT_BAR_DUTY_CYCLE_HIGH_COLOR_RED),
        map01(percent, LIGHT_BAR_DUTY_CYCLE_LOW_COLOR_GREEN, LIGHT_BAR_DUTY_CYCLE_HIGH_COLOR_GREEN),
        map01(percent, LIGHT_BAR_DUTY_CYCLE_LOW_COLOR_BLUE, LIGHT_BAR_DUTY_CYCLE_HIGH_COLOR_BLUE),
        map01(percent, LIGHT_BAR_DUTY_CYCLE_LOW_COLOR_WHITE, LIGHT_BAR_DUTY_CYCLE_HIGH_COLOR_WHITE)
    );
}
