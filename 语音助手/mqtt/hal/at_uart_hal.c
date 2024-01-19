#include "at_uart_hal.h"
#include "driver_usart.h"
void HAL_AT_Send(char *buf, int len)
{
	USART3_Write(buf, len);
}
int HAL_AT_Secv(char *c, int timeout)
{
	//从环形缓冲区中得到数据, 没有的话进行阻塞
	return USART3_Read(c, timeout);
}
int HAL_USRT2_Secv(char *c, int timeout)
{
	//从环形缓冲区中得到数据, 没有的话进行阻塞
	return USART2_Read(c, timeout);
}


