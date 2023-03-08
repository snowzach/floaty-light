#ifndef __CONFIG_H__
#define __CONFIG_H__

// ###################################################################### //
// ## For board specific GPIO-PIN definition please see platformio.ini ## //
// ###################################################################### //

#define VESC_CAN_ID 25 // VESC-ID as configured in VESC as decimal
// #define FAKE_VESC_ENABLED

#define LIGHT_NUMPIXELS 42 // the number of LEDs (first half front, second half back, needs to be even number)
#define LIGHT_LED_TYPE NEO_GRBW + NEO_KHZ800 // Update to whatever LED types you have.

#define LIGHT_FRONT_COLOR_RED 255
#define LIGHT_FRONT_COLOR_GREEN 255
#define LIGHT_FRONT_COLOR_BLUE 255
#define LIGHT_FRONT_COLOR_WHITE 255
#define LIGHT_REAR_COLOR_RED 255
#define LIGHT_REAR_COLOR_GREEN 0
#define LIGHT_REAR_COLOR_BLUE 0
#define LIGHT_REAR_COLOR_WHITE 0

#define LIGHT_BAR_ENABLED      // activates a light bar, comment if you don't have one
#define LIGHT_BAR_NUMPIXELS 10 // the number of LEDS in the light bar
// Which LEDs represent which values
#define LIGHT_BAR_LED_INDEX_ADC1 {8,9}
#define LIGHT_BAR_LED_INDEX_ADC2 {0,1}
#define LIGHT_BAR_LED_INDEX_BATTERY_CHARGE {2,3,4,5,6,7}
#define LIGHT_BAR_LED_INDEX_DUTY_CYCLE {}
// Config options
#define LIGHT_BAR_OFF_ERPM 10000
// Color Options
#define LIGHT_BAR_ADC_ON_COLOR_RED 0
#define LIGHT_BAR_ADC_ON_COLOR_GREEN 0
#define LIGHT_BAR_ADC_ON_COLOR_BLUE 100
#define LIGHT_BAR_ADC_ON_COLOR_WHITE 0
#define LIGHT_BAR_ADC_OFF_COLOR_RED 30
#define LIGHT_BAR_ADC_OFF_COLOR_GREEN 0
#define LIGHT_BAR_ADC_OFF_COLOR_BLUE 100
#define LIGHT_BAR_ADC_OFF_COLOR_WHITE 0
#define LIGHT_BAR_BATTERY_BRIGHTNESS 30
#define LIGHT_BAR_DUTY_CYCLE_BRIGHTNESS 30

#endif //__CONFIG_H__
