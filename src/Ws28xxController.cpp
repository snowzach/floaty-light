#include "Ws28xxController.h"
#include <Logger.h>

Ws28xxController::Ws28xxController(uint16_t pixels, uint8_t pin, uint8_t type, VescData *vescData)
        : Adafruit_NeoPixel(pixels, pin, type) {
    this->vescData = vescData;
}


// Update the pattern
void Ws28xxController::update() {
    if (stopPattern)
        return;

    if ((millis() - lastUpdate) > interval) { // time to update
        lastUpdate = millis();
        switch (activePattern) {
            case RAINBOW_CYCLE:
                rainbowCycleUpdate();
                break;
            case THEATER_CHASE:
                theaterChaseUpdate();
                break;
            case COLOR_WIPE:
                //ColorWipeUpdate();
                break;
            case CYLON:
                cylonUpdate();
                break;
            case FADE:
                fadeLightUpdate();
                break;
            case RESCUE_FLASH_LIGHT:
                flashLightUpdate();
                break;
            case PULSE:
                pulsatingLightUpdate();
                break;
            case SLIDE:
                slidingLightUpdate();
                break;
            case BATTERY_INDICATOR:
                batteryIndicatorUpdate();
                break;
            default:
                break;
        }
        show();
        increment();
    }
}

// Increment the Index and reset at the end
void Ws28xxController::increment() {
    if (direction == FORWARD) {
        index++;
        if (index >= totalSteps) {
            index = 0;
            onComplete(); // call the comlpetion callback
        }
    } else { // Direction == REVERSE
        --index;
        if (index <= 0) {
            index = totalSteps - 1;
            onComplete(); // call the comlpetion callback
        }
    }
}

// Reverse pattern direction
void Ws28xxController::reverse() {
    if (Logger::getLogLevel() == Logger::VERBOSE) {
      char buf[128];
      snprintf(buf, 128, "reversing pattern %d, direction %d", activePattern, direction);
      Logger::warning(LOG_TAG_WS28XX, buf);
    }
    if (direction == FORWARD) {
        direction = REVERSE;
        index = totalSteps;
    } else {
        direction = FORWARD;
        index = 0;
    }
}

void Ws28xxController::onComplete() {
    if (Logger::getLogLevel() == Logger::VERBOSE) {
      char buf[128];
      snprintf(buf, 128, "onComplete pattern %d, startSequence %d, reverseonComplete %d, repeat %d",
             activePattern, isStartSequence, reverseOnComplete, repeat);
      Logger::verbose(LOG_TAG_WS28XX, buf);
    }
    stopPattern = true;
    blockChange = false;
    if(isStartSequence) {
        isStartSequence = false;
        idleSequence();
        return;
    }
    if(reverseOnComplete){
        reverse();
        stopPattern = false;
        return;
    }
    if(repeat) {
        changePattern(activePattern, true, repeat);
        return;
    }
}

void Ws28xxController::changePattern(Pattern pattern, boolean isForward, boolean repeatPattern) {
    if (!repeatPattern && activePattern == pattern && isForward == (direction == Direction::FORWARD)) {
        return;
    }

    if (blockChange) {
        return;
    }

    if (Logger::getLogLevel() == Logger::VERBOSE) {
        char buf[128];
        snprintf(buf, 128, "changePattern new pattern %d, forward %d, repeat %d", pattern, isForward, repeatPattern);
        Logger::verbose(LOG_TAG_WS28XX, buf);
    }


    stopPattern = false;
    repeat = repeatPattern;
    reverseOnComplete = false;
    switch (pattern) {
        case RAINBOW_CYCLE:
            rainbowCycle(10, isForward ? Direction::FORWARD : Direction::REVERSE);
            break;
        case THEATER_CHASE:
            theaterChase(
                    Color((LIGHT_PRIMARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8),
                    Color((LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8), (uint8_t)400);
            break;
        case COLOR_WIPE:
            break;
        case CYLON:
            cylon(Color(
                    (LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                    (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                    (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8), 55);
            break;
        case FADE:
            fadeLight(map(50, 0, 500, 1, 15),
                      isForward ? Direction::FORWARD : Direction::REVERSE);
            break;
        case RESCUE_FLASH_LIGHT:
            flashLight(80, isForward ? Direction::FORWARD : Direction::REVERSE);
            break;
        case PULSE:
            pulsatingLight(40);
            reverseOnComplete = true;
            break;
        case SLIDE:
            slidingLight(Color((LIGHT_PRIMARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                               (LIGHT_PRIMARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                               (LIGHT_PRIMARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8),
                         Color((LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                               (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                               (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8),
                         LIGHT_START_TIMEOUT / (numPixels() / 4));
            break;
        case BATTERY_INDICATOR:
            clear();
            show();
            batteryIndicator(1000);
            break;
        case NONE:
            stop();
            break;
        default:
            break;
    }
}

// Initialize for a RainbowCycle
void Ws28xxController::rainbowCycle(uint8_t timeinterval, Direction dir) {
    activePattern = Pattern::RAINBOW_CYCLE;
    interval = timeinterval;
    totalSteps = 255;
    index = 0;
    direction = dir;
}

// Update the Rainbow Cycle Pattern
void Ws28xxController::rainbowCycleUpdate() {
    for (int i = 0; i < numPixels(); i++) {
        setPixelColor(i, wheel(((i * 256 / numPixels()) + index) & 255));
    }
}

void Ws28xxController::flashLight(uint8_t timeinterval, Direction dir) {
    activePattern = Pattern::RESCUE_FLASH_LIGHT;
    interval = timeinterval;
    totalSteps = 10;
    index = 0;
    direction = dir;
    if (Logger::getLogLevel() == Logger::VERBOSE) {
        char buf[64];
        snprintf(buf, 64, "flash %s", direction == FORWARD ? "forward" : "backward");
        Logger::verbose(LOG_TAG_WS28XX, buf);
    }
}

void Ws28xxController::flashLightUpdate() {
    for (int i = 0; i < numPixels(); i++) {
        if (i < numPixels() / 2)
            if (direction == FORWARD) {
#ifdef LED_MODE_ODD_EVEN
                if (i % 2 == 0) {
#endif
                    setPixelColor(i, Color(LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS));
#ifdef LED_MODE_ODD_EVEN
                } else {
                    setPixelColor(i, Color(0, 0, 0));
                }
#endif
            } else {
                setPixelColor(i, Color(index % 2 == 0 ? LIGHT_MAX_BRIGHTNESS_BRAKE : LIGHT_MAX_BRIGHTNESS, 0, 0, 0));
            }
        else if (direction == FORWARD) {
            setPixelColor(i, Color(index % 2 == 0 ? LIGHT_MAX_BRIGHTNESS_BRAKE : LIGHT_MAX_BRIGHTNESS, 0, 0, 0));
        } else {
#ifdef LED_MODE_ODD_EVEN
            if (i % 2 == 0) {
#endif
                setPixelColor(i, Color(LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS, LIGHT_MAX_BRIGHTNESS));
#ifdef LED_MODE_ODD_EVEN
            } else {
                setPixelColor(i, Color(0, 0, 0, 0));
            }
#endif
        }
    }
}

void Ws28xxController::fadeLight(uint8_t timeinterval, Direction dir) {
    activePattern = Pattern::FADE;
    interval = timeinterval;
    totalSteps = LIGHT_MAX_BRIGHTNESS;
    direction = dir;
    index = dir == Direction::FORWARD ? 0 : totalSteps - 1;
    if (Logger::getLogLevel() == Logger::VERBOSE) {
        char buf[64];
        snprintf(buf, 64, "fade %s", direction == FORWARD ? "forward" : "backward");
        Logger::verbose(LOG_TAG_WS28XX, buf);
    }
}

void Ws28xxController::fadeLightUpdate() {
    setLight(direction == Direction::FORWARD, index);
}

void Ws28xxController::pulsatingLight(uint8_t timeinterval) {
    activePattern = Pattern::PULSE;
    interval = timeinterval;
    totalSteps = LIGHT_MAX_BRIGHTNESS / 3;
    direction = Direction::FORWARD;
    index = 0;
}

void Ws28xxController::pulsatingLightUpdate() {
    for (int i = 0; i < numPixels(); i++) {
        if(i<numPixels()/2) {
            setPixelColor(i, Color(index, index, index));
        } else {
            setPixelColor(i, Color(index, 0, 0));
        }
    }
}

// Initialize for a Theater Chase
void Ws28xxController::theaterChase(uint32_t col1, uint32_t col2, uint8_t timeinterval, Direction dir) {
    activePattern = Pattern::THEATER_CHASE;
    interval = timeinterval;
    totalSteps = numPixels();
    color1 = col1;
    color2 = col2;
    index = 0;
    direction = dir;
}

// Update the Theater Chase Pattern
void Ws28xxController::theaterChaseUpdate() {
    for (int i = 0; i < numPixels(); i++) {
        if ((i + index) % 3 == 0) {
            setPixelColor(i, color1);
        } else {
            setPixelColor(i, color2);
        }
    }
}

// Initialize for a cylon
void Ws28xxController::cylon(uint32_t col1, uint8_t timeinterval) {
    activePattern = Pattern::CYLON;
    interval = timeinterval;
    totalSteps = (numPixels() - 1) * 2;
    color1 = col1;
    index = 0;
    direction = Direction::FORWARD;
}

// Update the cylon Pattern
void Ws28xxController::cylonUpdate() {
    for (int i = 0; i < numPixels(); i++) {
        if (i == index) { // Scan Pixel to the right
            setPixelColor(i, color1);
        } else if (i == totalSteps - index) { // Scan Pixel to the left
            setPixelColor(i, color1);
        } else { // Fading tail
            setPixelColor(i, dimColor(getPixelColor(i), 4));
        }
    }
}

void Ws28xxController::slidingLight(uint32_t col1, uint32_t col2, uint16_t timeinterval) {
    activePattern = Pattern::SLIDE;
    interval = timeinterval;
    totalSteps = numPixels() / 4;
    color1 = col1;
    color2 = col2;
    index = 0;
    direction = Direction::FORWARD;
}

void Ws28xxController::slidingLightUpdate() {
    setPixelColor(index, color1);
    setPixelColor(numPixels()/2 - 1 - index, color1);
    setPixelColor(numPixels()/2 + index, color2);
    setPixelColor(numPixels()-1 - index, color2);
}

void Ws28xxController::batteryIndicator(uint16_t timeinterval) {
    activePattern = Pattern::BATTERY_INDICATOR;
    interval = timeinterval;
    totalSteps = 100;
    index = 0;
    direction = Direction::FORWARD;
}

void Ws28xxController::batteryIndicatorUpdate() {
    float voltage = vescData->inputVoltage;
    int min_voltage = MIN_BATTERY_VOLTAGE * 100;
    int max_voltage = MAX_BATTERY_VOLTAGE * 100;
    int voltage_range = max_voltage - min_voltage;
    int used = max_voltage - voltage * 100; // calculate how much the voltage has dropped
    int value = voltage_range - used; // calculate the remaining value to lowest voltage
    float diffPerPixel = voltage_range / (numPixels() / 2); // calculate how much voltage a single pixel shall represent
    float count = value / diffPerPixel; // calculate how many pixels must shine

    int whole = count; // number of "full" green pixels
    int remainder = value*100 / voltage_range; // percentage of usage of current pixel

    Serial.printf("batteryIndicatorUpdate: voltage %f, voltage_range %d, diffPerPixel %f, pixel %d, count %f, used %d, value %d, remainder %d",
                  voltage, voltage_range, diffPerPixel, numPixels(), count, used, value, remainder);
    Serial.println();
    for(int i=0; i<numPixels();i++) {
        if(value<0) {
            setPixelColor(i, LIGHT_MAX_BRIGHTNESS, 0, 0);
            continue;
        }
        if (i < numPixels() / 2) {
            if (i <= whole) {
                int val = calcVal(remainder);
                setPixelColor(i, LIGHT_MAX_BRIGHTNESS - val, val, 0);
            } else {
                setPixelColor(i, 0, 0, 0);
            }
        } else {
            if (i <= whole+numPixels()/2) {
                int val = calcVal(remainder);
                setPixelColor(i, LIGHT_MAX_BRIGHTNESS - val, val, 0);
            } else {
                setPixelColor(i, 0, 0, 0);
            }
        }
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Ws28xxController::wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return Color(255 - wheelPos * 3, 0, wheelPos * 3);
    } else if (wheelPos < 170) {
        wheelPos -= 85;
        return Color(0, wheelPos * 3, 255 - wheelPos * 3);
    } else {
        wheelPos -= 170;
        return Color(wheelPos * 3, 255 - wheelPos * 3, 0);
    }
}

void Ws28xxController::init() {
    Logger::notice(LOG_TAG_WS28XX, "initializing ...");
    begin(); // This initializes the NeoPixel library.
    show();
}

void Ws28xxController::stop() {
    Logger::verbose("stop");
    activePattern = NONE;
    for (int i = 0; i < numPixels(); i++) {
        setPixelColor(i, Color(0, 0, 0));
    }
    show();
}

void Ws28xxController::setLight(boolean forward, int brightness) {
    for(int i = 0; i < numPixels(); i++ ) {
      if(i < numPixels()/2){
        if(forward)
          setPixelColor(i, Color(brightness, brightness, brightness, brightness));
        else
        //   setPixelColor(i, Color(LIGHT_MAX_BRIGHTNESS-brightness, 0, 0, 0));
          setPixelColor(i, Color(brightness, 0, 0, 0));
      } else {
        if(forward)
          setPixelColor(i, Color(brightness, 0, 0, 0));
        else
        //   setPixelColor(i, Color(LIGHT_MAX_BRIGHTNESS-brightness, LIGHT_MAX_BRIGHTNESS-brightness, LIGHT_MAX_BRIGHTNESS-brightness, LIGHT_MAX_BRIGHTNESS-brightness));
          setPixelColor(i, Color(brightness, brightness, brightness, brightness));
      }
    }
    show();
}

void Ws28xxController::idleSequence() {
    Pattern pattern;
    switch (LIGHT_IDLE_PATTERN) {
        case 1:
          pattern = Pattern::THEATER_CHASE;
          break;
        case 2:
          pattern = Pattern::CYLON;
          break;
        case 3:
          pattern = Pattern::RAINBOW_CYCLE;
          break;
        case 4:
          pattern = Pattern::PULSE;
          break;
        case 5:
          pattern = Pattern::BATTERY_INDICATOR;
          break;
        default:
            pattern = Pattern::PULSE;
    }
    changePattern(pattern, true, true);
}

void Ws28xxController::startSequence() {
    Logger::notice(LOG_TAG_WS28XX, "run startSequence");
    blockChange = true;
    isStartSequence = true;
    int timeinterval = 0;
    switch (LIGHT_START_PATTERN) {
        case Pattern::THEATER_CHASE:
            timeinterval = LIGHT_START_TIMEOUT / numPixels();
            theaterChase(
                    Color((LIGHT_PRIMARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8),
                    Color((LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8), timeinterval);
            break;
        case Pattern::CYLON:
            timeinterval = LIGHT_START_TIMEOUT / numPixels();
            cylon(
                    Color((LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8), timeinterval);
            break;
        case Pattern::RAINBOW_CYCLE:
            timeinterval = LIGHT_START_TIMEOUT / numPixels();
            rainbowCycle(10, Direction::FORWARD);
            break;
        case Pattern::SLIDE:
            timeinterval = LIGHT_START_TIMEOUT / (numPixels() / 4);
            slidingLight(Color((LIGHT_PRIMARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_PRIMARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8),
                    Color((LIGHT_SECONDARY_COLOR_RED * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_GREEN * LIGHT_MAX_BRIGHTNESS) >> 8,
                          (LIGHT_SECONDARY_COLOR_BLUE * LIGHT_MAX_BRIGHTNESS) >> 8), timeinterval);
            break;
    }
}

uint32_t Ws28xxController::dimColor(uint32_t color, uint8_t width) {
    return (((color & 0xFF0000) / width) & 0xFF0000) + (((color & 0x00FF00) / width) & 0x00FF00) +
           (((color & 0x0000FF) / width) & 0x0000FF);
}

// map the remaining value to a value between 0 and LIGHT_MAX_BRIGHTNESS
int Ws28xxController::calcVal(int value) {
    return map(value, 0, 100, 0, LIGHT_MAX_BRIGHTNESS);
}