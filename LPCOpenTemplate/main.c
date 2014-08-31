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

IRQ_UART_HANDLER;


uint8_t has_error;

void SysTick_Handler()
{
  static int led_on = 0;
  float temp = 0;

  if(get_temperature_from_mcp9808(I2C0, I2C_SLAVE_TEMPSENS_ADDR, &temp) == 0)
  {
    has_error = 0;
    Chip_GPIO_SetPinState(LPC_GPIO, 1, 11, led_on);
    Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, true);
    printf("%.2f\n\r", temp);
  }
  else
  {
    has_error = 1;
    Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, led_on);
    Chip_GPIO_SetPinState(LPC_GPIO, 1, 11, true);
  }
  
  led_on = !led_on;
}

int main()
{
  has_error = 0;
  
  SystemCoreClockUpdate();
  
  // Enable and setup SysTick Timer at a periodic rate
  SysTick_Config(SystemCoreClock / 5);
  
  uart_helper_init();
  
  i2c_app_init(DEFAULT_I2C, I2C_DEFAULT_SPEED);
  
  
  
  
  
  Chip_GPIO_Init(LPC_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 11);
  
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, true);
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 11, false);
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 7);
  


  
  while(1)
  {
  }
}
