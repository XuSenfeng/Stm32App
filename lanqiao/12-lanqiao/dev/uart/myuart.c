#include "uart/myuart.h"
#include "ring/ring.h"
ring_t uart_ring;
uint8_t uart_temp;
uint8_t uart_to_deal;
int to_deal_string = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    ring_buf_write(&uart_ring, &uart_temp, 1);
	if(uart_ring.size == 22)
		to_deal_string = 1;
    HAL_UART_Receive_IT(huart, &uart_temp, 1);
}

