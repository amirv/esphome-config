#include "esphome.h"

using namespace esphome;

class GroveSwitch : public Switch {
 public:
   explicit GroveSwitch() {
   };

   void start(Multi_Channel_Relay *_mcr, uint8_t _channel) {
     mcr = _mcr;
     channel = _channel;
   };

 protected:
  virtual void write_state(bool state) {
    if (!mcr)
      return;

    if (state) {
      mcr->turn_on_channel(channel);
    } else {
      mcr->turn_off_channel(channel);
    }
    publish_state(state);
  };

  Multi_Channel_Relay *mcr;
  uint8_t channel;
};

class GroveRelays : public Component {
 public:

  GroveSwitch relay1;
  GroveSwitch relay2;
  GroveSwitch relay3;
  GroveSwitch relay4;
  
  Multi_Channel_Relay mcr;

  uint8_t addr;

  explicit GroveRelays(uint8_t addr) : addr(addr) {
  };

  void setup() override {
    mcr.begin(addr);

    relay1.start(&mcr, 1);
    relay2.start(&mcr, 2);
    relay3.start(&mcr, 3);
    relay4.start(&mcr, 4);
  }

  void update() {
    uint8_t state = mcr.getChannelState();

    relay1.publish_state(state & CHANNLE1_BIT);
    relay2.publish_state(state & CHANNLE2_BIT);
    relay3.publish_state(state & CHANNLE3_BIT);
    relay4.publish_state(state & CHANNLE4_BIT);
  };
};
