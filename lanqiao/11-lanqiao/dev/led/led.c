#include "led/led.h"

uint8_t led_state = 0;
//��ȡһ�����ڵ�led
//��һ��led�����ĵ�ʱ��Ѷ�Ӧ��λ����Ϊ1
uint8_t led_read(void){
    return led_state;
}
//����һ��led
void led_write(uint8_t pins){
    led_state = pins;
    HAL_GPIO_WritePin(GPIOC, 0xff00, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, (pins << 8), GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}
