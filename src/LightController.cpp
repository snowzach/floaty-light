#include "LightController.h"
#include <vector>

#define ABS(N) ((N<0)?(-N):(N))

LightController::LightController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData)
        : Adafruit_NeoPixel(pixels, pin, type) {
    this->vescData = vescData;
}

void LightController::init() {
    Logger::notice(LOG_TAG_LIGHT, "initializing ...");
    begin();
    show();
}

int direction = 0;
const double erpm_idle = 10;

void LightController::loop() {

    if(vescData->erpm > erpm_idle) {
        if(direction < 255) direction += 1;
    } else if(vescData->erpm < -erpm_idle) {
        if(direction > -255) direction -= 1;
    } else {
        // It's idle
        if(direction) direction += (direction > 0 ? -1 : 1);  
    }

    for(int x=0; x < LIGHT_NUMPIXELS/2; x++) setPixelColor(x, getColor((direction+255)/511.0, ABS(direction)));
    for(int x=LIGHT_NUMPIXELS/2; x < LIGHT_NUMPIXELS; x++) setPixelColor(x, getColor((255-direction)/511.0, ABS(direction)));

    show();
}

double map01(double x, double start, double end) {
    return (x * (end-start)) + start;
}

uint32_t LightController::getColor(double percent, double brightness) {
    return Color(
        map01(percent, LIGHT_REAR_COLOR_RED, LIGHT_FRONT_COLOR_RED) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_GREEN, LIGHT_FRONT_COLOR_GREEN) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_BLUE, LIGHT_FRONT_COLOR_BLUE) * (brightness / 255.0),
        map01(percent, LIGHT_REAR_COLOR_WHITE, LIGHT_FRONT_COLOR_WHITE) * (brightness / 255.0)
    );
}
