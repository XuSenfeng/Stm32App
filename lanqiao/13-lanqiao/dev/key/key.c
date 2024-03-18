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
        keys[i].key_state = KEY_NOPRESS;
        keys[i].count = 0;
        keys[i].to_deal = 0;
    }
}

void key_scan(void){
    for(int i = 0; i < 4 ; i++){
        if(HAL_GPIO_ReadPin(keys[i].GPIOx, keys[i].GPIO_Pinx) == GPIO_PIN_RESET){
            if(keys[i].key_state == KEY_NOPRESS){
                keys[i].key_state = KEY_PRESS_PENDING;
                keys[i].count = 0;
            }
            else if(keys[i].key_state == KEY_PRESS_PENDING){

                keys[i].key_state = KEY_PRESS;
                keys[i].to_deal = 1;
            }
            else if(keys[i].key_state == KEY_PRESS){
                keys[i].count++;
                if(keys[i].count > 20){
                    keys[i].key_state = KEY_LONGPRESS;
                    keys[i].to_deal_long = 1;
                }
            }
        }
        else{
            keys[i].key_state = KEY_NOPRESS;
            keys[i].count = 0;
        }
    }
}

//获取按键的状态
uint8_t key_get_state(){
    uint8_t ret = 0;
    for(int i = 0;i < 4;i++){
        if(keys[i].to_deal){
            keys[i].to_deal = 0;
			ret |= 1 << i;
        }
        if(keys[i].to_deal_long){
            keys[i].to_deal_long = 0;
			ret |= 1 << i+4;

        }
    }
    return  ret;
}
