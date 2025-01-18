// See LICENSE for license details.

#ifndef _RISCV_UART_ROOMSOC_H
#define _RISCV_UART_ROOMSOC_H

#include <stdint.h>

extern volatile unsigned int *uart_roomsoc;

void uart_roomsoc_putchar(uint8_t ch);
int uart_roomsoc_getchar();
void query_uart_roomsoc(uintptr_t dtb);

#endif
