#ifndef __CONFIG_H__
#define __CONFIG_H__

// ###################################################################### //
// ## For board specific GPIO-PIN definition please see platformio.ini ## //
// ###################################################################### //

#define SOFTWARE_VERSION_MAJOR 1
#define SOFTWARE_VERSION_MINOR 0
#define SOFTWARE_VERSION_PATCH 0
#define HARDWARE_VERSION_MAJOR 3
#define HARDWARE_VERSION_MINOR 1

#define VESC_CAN_ID 25 // VESC-ID as configured in VESC as decimal
#define MAX_BATTERY_VOLTAGE 84
#define MIN_BATTERY_VOLTAGE 60
//#define FAKE_VESC_ENABLED

#define LIGHT_NUMPIXELS 42 // the number of LEDs if WS28xx is used
#define LIGHT_LED_TYPE NEO_GRBW + NEO_KHZ800

#define LIGHT_MAX_BRIGHTNESS 255
#define LIGHT_MAX_BRIGHTNESS_BRAKE 50
#define LIGHT_PRIMARY_COLOR_RED 255
#define LIGHT_PRIMARY_COLOR_GREEN 255
#define LIGHT_PRIMARY_COLOR_BLUE 255
#define LIGHT_PRIMARY_COLOR_WHITE 255
#define LIGHT_SECONDARY_COLOR_RED 255
#define LIGHT_SECONDARY_COLOR_GREEN 0
#define LIGHT_SECONDARY_COLOR_BLUE 0
#define LIGHT_SECONDARY_COLOR_WHITE 0
#define LIGHT_IDLE_TIMEOUT 120000
#define LIGHT_IDLE_PATTERN Pattern::PULSE
#define LIGHT_START_TIMEOUT 5000
#define LIGHT_START_PATTERN Pattern::SLIDE

#define LIGHT_BAR_ENABLED      // activates a visual WS28xx battery bar, if connected
#define LIGHT_BAR_NUMPIXELS 10 // the number of LEDS of the battery bar

#define LIGHT_BAR_OFF_ERPM 10000
#define LIGHT_BAR_BRIGHTNESS 50
#define LIGHT_BAR_ADC_ENABLED // Enables the lightbar support for ADC-Footpad

#endif //__CONFIG_H__
