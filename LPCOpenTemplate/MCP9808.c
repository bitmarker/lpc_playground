#include "MCP9808.h"
#include "i2c_helper.h"

uint8_t get_temperature_from_mcp9808(I2C_ID_T i2c_id, uint8_t slave_addr, float *temperature)
{
  uint16_t raw_temp;
  
  if(i2c_read16(i2c_id, slave_addr, MCP9808_I2C_CMD_TEMPERATURE, &raw_temp) != 0)
  {
    // can not read the temperature
    return 1;
  }
  
  // xxx1 xxxx -> T < 0°C
  if((raw_temp & 0x1000) == 0x1000)
  {
    *temperature = 256 - (raw_temp & 0x0FFF) / 16.0;
  }
  else
  {
    *temperature = (raw_temp & 0x0FFF) / 16.0;
  }
  
  return 0;
}