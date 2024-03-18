#include "uart/myuart.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "adc/myadc.h"
#include "lcd/lcd.h"
#include "stdio.h"
#include "led/led.h"

uint8_t rxdata[30];
uint8_t rxdata_t;
uint8_t rx_pointer;
int lcd_show_mode;
int usart_to_deal = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	HAL_UART_Receive_IT(&huart1, &rxdata_t, 1);
	rxdata[rx_pointer ++] = rxdata_t;
	if(rxdata_t == ':' || rxdata_t == '#'){
		usart_to_deal = 1;
	}
}

extern int frq_pwm;
extern adc_t adcs[2];
void usart_deal_message(void){
	uint8_t buf[10];
	if(rxdata[0] == 'P' && rxdata[1] == 'A'){
		int port = rxdata[2] - '0';
		switch (port){
			case 1:
				sprintf((char *)buf, "%d\n", frq_pwm);
				HAL_UART_Transmit(&huart1, buf, strlen((const char *)buf), 500);
				break;
			case 4:
				sprintf((char *)buf, "%.2f\n", adcs[0].adc_voltage);
				HAL_UART_Transmit(&huart1, buf, strlen((const char *)buf), 500);
				break;
			case 5:
				sprintf((char *)buf, "%.2f\n", adcs[1].adc_voltage);
				HAL_UART_Transmit(&huart1, buf, strlen((const char *)buf), 500);
				break;
			default:
				break;
		}
		
	}else if(rxdata[0] == '#'){
		lcd_show_mode = !lcd_show_mode;
		uint8_t led_state = led_get_pin();
		if(lcd_show_mode)
			led_state &= ~(1<<3);
		else
			led_state |= (1<<3);
		led_set_pin(led_state);
	}
	usart_to_deal = 0;
	rx_pointer = 0;
}
