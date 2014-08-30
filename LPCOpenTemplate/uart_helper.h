#ifndef _UART_HELPER__H
#define _UART_HELPER__H

#include "chip.h"

void init_uart_pinmux();
void uart_helper_init();

#define IRQ_UART_HANDLER void UART_IRQHandler(void) { Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring); }

uint32_t uart_send_rb(void *data, int len);

#endif