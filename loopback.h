#include "esphome.h"

static const char *const TAG = "SerialLoopback";

class LoopbackComponent : public Component, public UARTDevice {
 public:
  LoopbackComponent(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
  }

  void loop() override {
    while (available()) {
      char c = read();

      write(c);

      ESP_LOGW(TAG, "Got char: %c", c);
    }
  }
};
