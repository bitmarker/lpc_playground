#include "uart_helper.h"


STATIC RINGBUFF_T txring, rxring;
#define UART_SRB_SIZE 128 /* Send */
#define UART_RRB_SIZE 32 /* Receive */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


void init_uart_pinmux()
{
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); // RXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); // TXD
}

void uart_helper_init()
{
  init_uart_pinmux();
  
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
}

uint32_t uart_send_rb(void *data, int len)
{
  return Chip_UART_SendRB(LPC_USART, &txring, data, len);
}

void uart_helper_irq_handler()
{
  Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring);
}
