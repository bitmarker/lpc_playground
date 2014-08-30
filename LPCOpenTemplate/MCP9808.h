#ifndef __MCP9808_H__
#define __MCP9808_H__

#include "chip.h"

uint8_t get_temperature_from_mcp9808(I2C_ID_T i2c_id, uint8_t slave_addr, float * temperature);

#define MCP9808_I2C_CMD_TEMPERATURE     0x05

#define MCP9808_I2C_CMD_SIGNATURE       0x07


#endif