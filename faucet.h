#include "multi_channel_relay.h"

#include <string>

void faucet_update_status(esphome::binary_sensor::BinarySensor *opened,
			  esphome::binary_sensor::BinarySensor *closed,
			  esphome::text_sensor::TextSensor *state,
			  esphome::switch_::Switch *valve = nullptr,
			  esphome::script::SingleScript<> *do_open = nullptr,
			  esphome::script::SingleScript<> *do_close = nullptr)
{
	if (!opened->state && !closed->state) {
		state->publish_state("Transit");
		if (valve && do_open && do_close) {
			if (valve->state)
				do_open->execute();
			else
				do_close->execute();
		}

	} else if (opened->state && !closed->state)
		state->publish_state("Open");
	else if (!opened->state && closed->state)
		state->publish_state("Closed");
	else
		state->publish_state("Error");
}
