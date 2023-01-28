#include <Arduino.h>
#include <Logger.h>
#include "config.h"
#include "ILedController.h"
#include "LightBarController.h"
#include "Ws28xxController.h"
#include "CanBus.h"

int new_forward  = LOW;
int new_backward = LOW;
int new_brake    = LOW;
int idle         = LOW;
double idle_erpm = 10.0;

int lastFake = 4000;
int lastFakeCount = 0;
VescData vescData;
ILedController *ledController;
LightBarController *lightBarController;
CanBus * canbus = new CanBus(&vescData);

// Declare the local logger function before it is called.
void localLogger(Logger::Level level, const char* module, const char* message);

void fakeCanbusValues() {
    if(millis() - lastFake > 3000) {
        vescData.tachometer= random(0, 30);
        vescData.inputVoltage = random(43, 50);
        vescData.dutyCycle = random(0, 100);
        if(lastFakeCount > 10) {
            vescData.erpm = random(-100, 200);
        } else {
            vescData.erpm = 0;//random(-100, 200);
        }
        vescData.current = random(0, 10);
        vescData.ampHours = random(0, 100);
        vescData.mosfetTemp = random(20, 60);
        vescData.motorTemp = random(20, 40);
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
  ledController = new Ws28xxController(LIGHT_NUMPIXELS, LIGHT_PIN, LIGHT_LED_TYPE, &vescData);
  lightBarController = new LightBarController(LIGHT_BAR_NUMPIXELS, LIGHT_BAR_PIN, LIGHT_LED_TYPE, &vescData);
  
  delay(50);

  canbus->init();
  ledController->init();
  #ifdef LIGHT_BAR_ENABLED
  lightBarController->init();
  #endif
  ledController->startSequence();

  char buf[128];
  snprintf(buf, 128, " sw-version %d.%d.%d started on hw-version %d.%d",
    SOFTWARE_VERSION_MAJOR, SOFTWARE_VERSION_MINOR, SOFTWARE_VERSION_PATCH,
    HARDWARE_VERSION_MAJOR, HARDWARE_VERSION_MINOR);
  Logger::notice("floaty", buf);
}

void loop() {
  #ifdef FAKE_VESC_ENABLED
    fakeCanbusValues();
  #endif
  new_forward  = vescData.erpm > idle_erpm ? HIGH : LOW;
  new_backward = vescData.erpm < -idle_erpm ? HIGH : LOW;
  idle         = (abs(vescData.erpm) < idle_erpm && vescData.switchState == 0) ? HIGH : LOW;
  new_brake    = (abs(vescData.erpm) > idle_erpm && vescData.current < -4.0) ? HIGH : LOW;

  #ifndef FAKE_VESC_ENABLED
    canbus->loop();
  #endif

  // is motor brake active?
  if(new_brake == HIGH) {
    // flash backlights
    ledController->changePattern(Pattern::RESCUE_FLASH_LIGHT, new_forward == HIGH, false);
  }

  // call the led loops
  ledController->loop(&new_forward, &new_backward, &idle);
  #ifdef LIGHT_BAR_ENABLED
  lightBarController->loop();
  #endif

}

void localLogger(Logger::Level level, const char* module, const char* message) {
  Serial.print(F("FWC: ["));
  Serial.print(Logger::asString(level));
  Serial.print(F("] "));
  if (strlen(module) > 0) {
      Serial.print(F(": "));
      Serial.print(module);
      Serial.print(" ");
  }
  Serial.println(message);
}