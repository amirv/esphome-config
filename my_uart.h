#include "esphome.h"

static const char *const TAG = "ModbusController";

class MyUartComponent : public Component, public UARTDevice {
 public:
  MyUartComponent(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // nothing to do here
  }
  void loop() override {
    while (available()) {
      char c = read();
      ESP_LOGW(TAG, "Got char: %c", c);

    }
    // etc
  }
};
