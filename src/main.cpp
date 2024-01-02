#include <Arduino.h>
#include <Logger.h>
#include "config.h"
#include "LightController.h"
#include "LightBarController.h"
#include <Adafruit_NeoPixel.h>
#include "CanBus.h"

int lastFake = 4000;
int lastFakeCount = 0;
VescData vescData;
LightController *lightController;
LightBarController *lightBarController;
CanBus * canbus = new CanBus(&vescData);

// Declare the local logger function before it is called.
void localLogger(Logger::Level level, const char* module, const char* message);

void fakeCanbusValues() {
    if(millis() - lastFake > 3000) {
        vescData.dutyCycle = random(0, 100);
        if(lastFakeCount > 10) {
            vescData.erpm = random(-100, 200);
        } else {
            vescData.erpm = 0;//random(-100, 200);
        }
        vescData.adc1 = 0.5;
        vescData.adc2 = 0.5;
        vescData.switchState = 0;
        lastFake = millis();
        lastFakeCount++;
    }
}

void setup() {
  Logger::setOutputFunction(localLogger);

  Logger::setLogLevel(Logger::NOTICE);
  if(Logger::getLogLevel() != Logger::SILENT) {
      Serial.begin(115200);
  }
  lightController = new LightController(LIGHT_NUMPIXELS, LIGHT_PIN, LIGHT_LED_TYPE, &vescData);
  lightBarController = new LightBarController(LIGHT_BAR_NUMPIXELS, LIGHT_BAR_PIN, LIGHT_BAR_LED_TYPE, &vescData);
  
  delay(50);

  canbus->init();
  lightController->init();
  #ifdef LIGHT_BAR_ENABLED
  lightBarController->init();
  #endif

  Logger::notice("Floaty", "Startup...");
}

void loop() {

  canbus->loop();
  // call the led loops
  lightController->loop();
  #ifdef LIGHT_BAR_ENABLED
  lightBarController->loop();
  #endif

}

void localLogger(Logger::Level level, const char* module, const char* message) {
  Serial.print(F("FLOATY: ["));
  Serial.print(Logger::asString(level));
  Serial.print(F("] "));
  if (strlen(module) > 0) {
      Serial.print(F(": "));
      Serial.print(module);
      Serial.print(" ");
  }
  Serial.println(message);
}
