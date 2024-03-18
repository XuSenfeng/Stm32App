#include "main.h"
#include "led/led.h"
uint8_t led_pins;
void led_write(uint8_t led_pin){
	led_pins = led_pin;
	HAL_GPIO_WritePin(GPIOC, 0xff00, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, led_pin << 8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t led_read(void){
	return led_pins;
}
