#ifndef _UART_HELPER__H
#define _UART_HELPER__H

#include "chip.h"

void init_uart_pinmux();
void uart_helper_init();
void uart_helper_irq_handler();

#define IRQ_UART_HANDLER void UART_IRQHandler(void) { uart_helper_irq_handler(); }

uint32_t uart_send_rb(void *data, int len);

#endif