#include "led/led.h"
#include "main.h"

uint8_t led_gpio_state = 0xff;

void led_set_pin(uint8_t pins){
	HAL_GPIO_WritePin(GPIOC, 0xff00, GPIO_PIN_SET); 
	led_gpio_state = pins;
	HAL_GPIO_WritePin(GPIOC, (pins << 8), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t led_get_pin(void){

	return led_gpio_state;

}

