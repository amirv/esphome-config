#include "esphome.h"

using namespace esphome;
using namespace modbus_controller;

void queue_write_cmd(ModbusController *hub, uint16_t reg, uint16_t val)
{
    ESP_LOGW("XXXXX", "Setting %#x to %d", reg, val);
    auto cmd = modbus_controller::ModbusCommandItem::create_write_single_command(hub, reg, val);
    hub->queue_command(cmd);
};

enum motor_status {
    MOTOR_STATUS_UNKNOWN = 0,
    MOTOR_STATUS_FORWARD = 1,
    MOTOR_STATUS_STOPPED = 2,
    MOTOR_STATUS_BACKWARD = 3,
    MOTOR_STATUS_TRANSITION = 4,
};

enum motor_status motor_status_code_a(uint16_t status)
{
    if (status == 0xFFFF) {
        return MOTOR_STATUS_UNKNOWN;
    }

    if ((status & 0x3) == 0x0) {
        return MOTOR_STATUS_STOPPED;
    }

    status &= 0x1b;

    if (status == 0x3) {
        return MOTOR_STATUS_FORWARD;
    } else if (status == 0x1b) {
        return MOTOR_STATUS_BACKWARD;
    }

    return MOTOR_STATUS_TRANSITION;
}

//#define BIT(x) (1<<x)

enum motor_status motor_status_code_b(uint16_t status)
{
    if (status == 0xFFFF) {
        return MOTOR_STATUS_UNKNOWN;
    }

    if ((status & BIT(12)) == 0) {
        return MOTOR_STATUS_STOPPED;
    } else if ((status & BIT(11)) == 0) {
        return MOTOR_STATUS_FORWARD;
    } else if ((status & BIT(11)) == BIT(11)) {
        return MOTOR_STATUS_BACKWARD;
    }

    return MOTOR_STATUS_UNKNOWN;
}

std::string motor_status_str(enum motor_status status)
{
    switch (status) {
    case MOTOR_STATUS_FORWARD: return "FORWARD";
    case MOTOR_STATUS_STOPPED: return "STOPPED";
    case MOTOR_STATUS_BACKWARD: return "BACKWARD";
    case MOTOR_STATUS_TRANSITION: return "TRANSITION";
    default:
        break;
    }
    
    return {"UNKNOWN"};
}
