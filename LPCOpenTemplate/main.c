#include "chip.h"
#include <stdio.h>


#include "string.h"
#include "i2c_helper.h"
#include "MCP9808.h"
#include "uart_helper.h"
#include "utils.h"

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;




#define I2C_SLAVE_TEMPSENS_ADDR       0x18

I2C_IRQ_HANDLER(DEFAULT_I2C);

FILE __stdout;

int fputc(int ch, FILE *stream)
{
  uart_send_rb(&ch, 1);
  return ch;
}


void SysTick_Handler(void)
{
  static int led_on = 0;
  float temp = 0;

  Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, led_on);
  
  get_temperature_from_mcp9808(I2C0, I2C_SLAVE_TEMPSENS_ADDR, &temp);

  printf("Temp: %f\n", temp);
  
  led_on = !led_on;
}

int main()
{
  
  uart_helper_init();
  
  i2c_app_init(DEFAULT_I2C, I2C_DEFAULT_SPEED);
  
  
  SystemCoreClockUpdate();
  
  
  Chip_GPIO_Init(LPC_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, false);
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 7);
  

  // Enable and setup SysTick Timer at a periodic rate
  SysTick_Config(SystemCoreClock);
  
  while(1)
  {
    /*
    if(i2c_read16(DEFAULT_I2C, I2C_SLAVE_TEMPSENS_ADDR, 0x06, &raw_temp) != 0)
    {
      continue;
    }
    */
    
    BP;
  }
}
