#ifndef __AT_UART_HAL_H
#define __AT_UART_HAL_H

void HAL_AT_Send(char *buf, int len);
int HAL_AT_Secv(char *c, int timeout);
#endif /*__AT_UART_HAL_H*/
