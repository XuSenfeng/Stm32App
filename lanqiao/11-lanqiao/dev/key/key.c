#include "key/key.h"
key_t keys[4];

void key_init(void){
	keys[0].GPIOx = GPIOB;
	keys[0].GPIO_Pinx = GPIO_PIN_0;
	keys[1].GPIOx = GPIOB;
	keys[1].GPIO_Pinx = GPIO_PIN_1;
	keys[2].GPIOx = GPIOB;
	keys[2].GPIO_Pinx = GPIO_PIN_2;
	keys[3].GPIOx = GPIOA;
	keys[3].GPIO_Pinx = GPIO_PIN_0;
    for(int i = 0; i < 4; i++){
        keys[i].state = KEY_NOPRESS;
        keys[i].count = 0;
        keys[i].to_deal = 0;
    }
}

void key_deal_time(void){
    for(int i = 0; i < 4; i++){
        if(HAL_GPIO_ReadPin(keys[i].GPIOx, keys[i].GPIO_Pinx) == GPIO_PIN_RESET){
            if(keys[i].state == KEY_NOPRESS){
                keys[i].state = KEY_PRESS;
                keys[i].count = 0;
            }
            else if(keys[i].state == KEY_PRESS){
                keys[i].count++;
                keys[i].to_deal = 1;
				keys[i].state = KEY_LONG_PRESS;
            }else if(keys[i].state == KEY_LONG_PRESS){
                if(keys[i].count++ >= 40){
                    keys[i].to_deal_long = 1;
                }
            }
            
        }else{
			keys[i].state = KEY_NOPRESS;
			keys[i].count = 0;
		}
    }
}
uint8_t key_scan(void){
    int ret = 0;
    for(int i = 0; i < 4; i++){
        if(keys[i].to_deal){
            keys[i].to_deal = 0;
            ret |= 1<<i;
        }
        if(keys[i].to_deal_long){
            keys[i].to_deal_long = 0;
            ret |= 1<<(i+4);
        }
    }
    return ret;
}
#include "led/led.h"
extern int menu_num;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM16){
        key_deal_time();
    }
	static int i = 0;
	i++;
	if(i == 40){
		if(menu_num == 0){
			int led_state;
			led_state = led_read();
			led_state ^= 1;
			led_write(led_state);
		}
		i = 0;
	}

}


