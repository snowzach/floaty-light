#include "LightBarController.h"

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

    double charge = (vescData->inputVoltage - MIN_BATTERY_VOLTAGE) * (1.0 / (MAX_BATTERY_VOLTAGE-MIN_BATTERY_VOLTAGE));
    setPixelColor(2, (1.0-charge) * LIGHT_BAR_BRIGHTNESS, charge * LIGHT_BAR_BRIGHTNESS, 0, 0);
    setPixelColor(3, (1.0-charge) * LIGHT_BAR_BRIGHTNESS, charge * LIGHT_BAR_BRIGHTNESS, 0, 0);
    setPixelColor(6, (1.0-charge) * LIGHT_BAR_BRIGHTNESS, charge * LIGHT_BAR_BRIGHTNESS, 0, 0);
    setPixelColor(7, (1.0-charge) * LIGHT_BAR_BRIGHTNESS, charge * LIGHT_BAR_BRIGHTNESS, 0, 0);

    double duty = (vescData->dutyCycle/100.0);
    if(duty > 75) duty = 1.0;
    setPixelColor(4, duty * LIGHT_BAR_BRIGHTNESS, (1-duty) * LIGHT_BAR_BRIGHTNESS, 0, 0);
    setPixelColor(5, duty * LIGHT_BAR_BRIGHTNESS, (1-duty) * LIGHT_BAR_BRIGHTNESS, 0, 0);

    AdcState adcState = mapSwitchState(vescData->switchState, vescData->adc1 > vescData->adc2);
    if (adcState != lastAdcState) {
        switch (adcState) {
            case ADC_NONE:
                setPixelColor(8, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(9, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(0, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(1, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                break;
            case ADC_HALF_ADC1:
                setPixelColor(8, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(9, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(0, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(1, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                break;
            case ADC_HALF_ADC2:
                setPixelColor(8, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(9, LIGHT_BAR_BRIGHTNESS, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(0, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(1, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                break;
            case ADC_FULL:
                setPixelColor(8, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(9, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(0, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                setPixelColor(1, 0, 0, LIGHT_BAR_BRIGHTNESS, 0); // full purple
                break;
        }
    }
    lastAdcState = adcState;
    show();
}

// map the remaining value to a value between 0 and MAX_BRIGHTNESS
int LightBarController::calcVal(int value) {
    return map(value, 0, 100, 0, LIGHT_BAR_BRIGHTNESS);
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