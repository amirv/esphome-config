#include "esphome.h"
#include "esphome/core/component.h"

#define TAG "ModbusSlave"

#define REGS_COUNT 10

static inline uint16_t crc16(const uint8_t *data, uint8_t len) {
	uint16_t crc = 0xFFFF;
	while (len--) {
		crc ^= *data++;
		for (uint8_t i = 0; i < 8; i++) {
			if ((crc & 0x01) != 0) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
};

class ModbusSlave : public UARTDevice, public Component  {
 public:
	 ModbusSlave(UARTComponent *parent) : UARTDevice(parent), address(0) {}

	 void set_address(int addr) {
		 this->address = addr;
	 };

	 void add_register(int addr, Component *comp) {
	 };

	 void loop() override {
		 const uint32_t now = millis();
		 if (now - this->last_modbus_byte_ > 50) {
			 this->rx_buffer_.clear();
			 this->last_modbus_byte_ = now;
		 }

		 while (this->available()) {
			 ESP_LOGW(TAG, "%s:%d -- ", __func__, __LINE__);
			 uint8_t byte;
			 this->read_byte(&byte);
			 if (this->parse_modbus_byte_(byte)) {
				 this->last_modbus_byte_ = now;
			 } else {
				 this->rx_buffer_.clear();
			 }
		 }
	 }

	 void dump_config() override {
		 ESP_LOGCONFIG(TAG, "ModbusSlave:");
		 this->check_uart_settings(9600, 2);
	 }

	 float get_setup_priority() const override {
		 // After UART bus
		 return setup_priority::BUS - 1.0f;
	 }

	 void send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t register_count) {
		 uint8_t frame[8];
		 frame[0] = address;
		 frame[1] = function;
		 frame[2] = start_address >> 8;
		 frame[3] = start_address >> 0;
		 frame[4] = register_count >> 8;
		 frame[5] = register_count >> 0;
		 auto crc = crc16(frame, 6);
		 frame[6] = crc >> 0;
		 frame[7] = crc >> 8;

		 this->write_array(frame, 8);
	 }

 protected:
	 bool parse_modbus_byte_(uint8_t byte) {
		 size_t at = this->rx_buffer_.size();
		 this->rx_buffer_.push_back(byte);
		 const uint8_t *raw = &this->rx_buffer_[0];

		 if (at < 7)
			 return true;

		 // Byte 0: modbus address (match all)
		 // Byte 1: Function
		 // Byte 2,3: reg address
		 // Byte 4,5: num regs (for read), value (for write)
		 // Byte 6,7: CRC

		 uint16_t computed_crc = crc16(raw, 6);
		 uint16_t remote_crc = uint16_t(raw[6]) | (uint16_t(raw[7]) << 8);
		 if (computed_crc != remote_crc) {
			 ESP_LOGW(TAG, "ModbusSlave CRC Check failed! %02X!=%02X", computed_crc, remote_crc);
			 return false;
		 }

		 uint8_t address = raw[0];
		 if (address != this->address)
			 return false;

		 uint8_t func = raw[1];
		 if (func != 4 && func != 6) {
			 ESP_LOGW(TAG, "function %d not supported. Only 3 or 6", func);
			 return false;
		 }

		 uint16_t reg_addr = uint16_t(raw[2]) | (uint16_t(raw[3]) << 8);

		 if (reg_addr >= REGS_COUNT || this->devices[reg_addr]) {
			 ESP_LOGW(TAG, "Bad register address: %#x\n", reg_addr);
			 return false;
		 }

		 Component *device = this->devices[reg_addr];

		 if (func == 6) {
			 // Read Input registers
			 uint16_t reg_count = uint16_t(raw[4]) | (uint16_t(raw[5]) << 8);

			 if (reg_count != 1) {
				 ESP_LOGW(TAG, "Requested %d registers. only reading one is supported", reg_count);
				 return false;
			 }

		 } else if (func == 6) {
			 // Write Single Holding Register
			 uint16_t reg_val = uint16_t(raw[4]) | (uint16_t(raw[5]) << 8);
		 }

		 return true;
	 }


	 std::vector<uint8_t> rx_buffer_;
	 uint32_t last_modbus_byte_{0};
	 int address;
	 Component *devices[REGS_COUNT];
};
#undef TAG
