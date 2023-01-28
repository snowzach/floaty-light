#ifndef RESCUE_CANDEVICE_H
#define RESCUE_CANDEVICE_H

#include "Arduino.h"
#include "config.h"
#include <Logger.h>
#include "driver/twai.h"

#define LOG_TAG_CANDEVICE "CanDevice"

class CanDevice {
  private:
    SemaphoreHandle_t mutex_v = xSemaphoreCreateMutex();
  public:
    void init();
    void sendCanFrame(const twai_message_t *p_frame);
};
#endif //RESCUE_CANDEVICE_H
