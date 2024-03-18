#include "key/key.h"
#include "gpio.h"

key_t keys[4];

void KEY_Init(void){
	keys[0].GPIOx = GPIOB;
	keys[0].GPIO_Pinx = GPIO_PIN_0;
	keys[1].GPIOx = GPIOB;
	keys[1].GPIO_Pinx = GPIO_PIN_1;
	keys[2].GPIOx = GPIOB;
	keys[2].GPIO_Pinx = GPIO_PIN_2;
	keys[3].GPIOx = GPIOA;
	keys[3].GPIO_Pinx = GPIO_PIN_0;
	for(int i = 0;i < 4; i++){
		keys[i].to_deal = 0;
		keys[i].state = KEY_NO_PRESS;
	}
}

void key_do_handler(){
	for(int i = 0;i < 4; i++){
		keys[i].input = HAL_GPIO_ReadPin(keys[i].GPIOx, keys[i].GPIO_Pinx);
		if(keys[i].input == GPIO_PIN_RESET){
			switch(keys[i].state){
				case KEY_NO_PRESS:
					keys[i].state = KEY_PRESS_UNAFFIRM;
					keys[i].time++;
					break;
				case KEY_PRESS_UNAFFIRM:
					keys[i].state = KEY_PRESS_AFFIRM;
					keys[i].time++;
					keys[i].to_deal = 1;
					break;
				case KEY_PRESS_AFFIRM:
					if(keys[i].time++ > 20){
						keys[i].long_press = 1;
						keys[i].to_deal = 1;
					}
					break;
			}
		}else{
			keys[i].state = KEY_NO_PRESS;
			keys[i].time = 0;

		}
		
	}

}


uint8_t key_get_state(void){
	uint8_t key_state = 0;
	for(int i = 0;i < 4; i++){
		if(keys[i].long_press && keys[i].to_deal)
		{
			key_state |= 1 << i + 4;
			keys[i].to_deal = 0;
			keys[i].long_press = 0;
		}
		if(keys[i].state == KEY_PRESS_AFFIRM && keys[i].to_deal)
		{
			key_state |= 1 << i;
			keys[i].to_deal = 0;
			
		}


	}
	return key_state;
}
