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

void scanI2CDevice(esphome::template_::TemplateTextSensor *comp)
{
    Multi_Channel_Relay mcr;
	  std::vector<uint8_t> v;
	  std::string txt = "";

	  comp->publish_state("Scanning...");

    mcr.scanI2CDevice(v);
	  
	  if (v.empty()) {
		  txt = "No devices found";
	  } else {
		  txt = "addresses: ";
		  for (uint8_t a: v) {
			  txt += "0x" + format_hex(a) + ", ";
		  }
	  }

	  comp->publish_state(txt);

}


void do_scan(esphome::i2c::I2CBus *bus, std::vector<uint8_t> &v)
{
    for (uint8_t address = 8; address < 120; address++) {
      auto err = bus->writev(address, nullptr, 0);

      ESP_LOGD("grove", "address %d err: %d", address, err);

      if (err == ERROR_OK) {
    	    v.push_back(address);
      } else if (err == ERROR_UNKNOWN) {
      }
    }
}

void scanI2CDevice(esphome::i2c::I2CBus *bus, esphome::template_::TemplateTextSensor *comp) {
    byte error = 0, address = 0, result = 0;
	  std::vector<uint8_t> v;
	  std::string txt = "";
    int nDevices;

	  comp->publish_state("Scanning...");

    ESP_LOGD("grove", "Scanning...");

    do_scan(bus, v);

	  if (v.empty()) {
		  txt = "No devices found";
	  } else {
		  txt = "addresses: ";
		  for (uint8_t a: v) {
			  txt += "0x" + format_hex(a) + ", ";
		  }
	  }

	  comp->publish_state(txt);
}

void setI2CDevice(esphome::i2c::I2CBus *bus, uint8_t new_addr) {
  uint8_t data[] = { CMD_SAVE_I2C_ADDR, new_addr };
  std::vector<uint8_t> v;
  uint8_t old_addr;

  do_scan(bus, v);

  if (v.size() != 1) {
    ESP_LOGE("grove", "Don't know which old address to use");
    return;
  }

  bus->write(v[0], data, 2, true);
}
