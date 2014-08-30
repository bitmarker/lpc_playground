#include "chip.h"
#include <stdio.h>


#include "string.h"

#include "i2c_helper.h"
#include "MCP9808.h"

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

STATIC RINGBUFF_T txring, rxring;
#define UART_SRB_SIZE 128 /* Send */
#define UART_RRB_SIZE 32 /* Receive */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


static void Init_UART_PinMux()
{
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); // RXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); // TXD
}

void UART_IRQHandler(void)
{
  Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring);
}




#define BP asm("mov r0,r0")


#define I2C_SLAVE_TEMPSENS_ADDR       0x18


I2C_IRQ_HANDLER(DEFAULT_I2C);

FILE __stdout;

int fputc(int ch, FILE *stream)
{
   Chip_UART_SendRB(LPC_USART, &txring, &ch, 1);
   return ch;
}

int main()
{
  uint16_t raw_temp;
  
  float temp = 0;
  
  Init_UART_PinMux();
  
  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, 9600);
  Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

  RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
  RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

  Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));

  NVIC_SetPriority(UART0_IRQn, 1);
  NVIC_EnableIRQ(UART0_IRQn);

  
  i2c_app_init(DEFAULT_I2C, I2C_DEFAULT_SPEED);
  
  while(1)
  {
    
    if(i2c_read16(DEFAULT_I2C, I2C_SLAVE_TEMPSENS_ADDR, 0x06, &raw_temp) != 0)
    {
      continue;
    }
    
    get_temperature_from_mcp9808(I2C0, I2C_SLAVE_TEMPSENS_ADDR, &temp);

    printf("Temp: %f\n", temp);
    
    BP;
  }
}
