#include "menu/menu.h"
#include "main.h"
#include "lcd/lcd.h"
#include "key/key.h"
#include "stdio.h"
#include "tim.h"
#include "led/led.h"
#include "dealstring/dealstring.h"
int CNBR_num = 2, VNBR_num = 4;
double CNBR_price = 3.5, VNBR_price = 2.0;

void menu_init(void){
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
}
//第一个界面
void menu_deal0(uint8_t key_state){
    uint8_t buf[30];

    LCD_DisplayStringLine(Line1, "       DATA");
    sprintf((char *)buf, "   CNBR:%d", CNBR_num);
    LCD_DisplayStringLine(Line3, buf); 
    sprintf((char *)buf, "   VNBR:%d", VNBR_num); 
    LCD_DisplayStringLine(Line5, buf);
    sprintf((char *)buf, "   IDLE:%d", CNBR_num + VNBR_num);
    LCD_DisplayStringLine(Line7, buf);

}
//第二个界面
void menu_deal1(uint8_t key_state){
	if(key_state & KEY2_PRESS)
	{
		CNBR_price += 0.5;
		VNBR_price += 0.5;
	}
	if(key_state & KEY3_PRESS)
	{
		
		CNBR_price -= 0.5;
		VNBR_price -= 0.5;
		if(CNBR_price < 0)
			CNBR_price = 0;
		if(VNBR_price < 0)
			VNBR_price = 0;
	}
    uint8_t buf[30];
    LCD_DisplayStringLine(Line1, "       Para");
    sprintf((char *)buf, "   CNBR:%.2f", CNBR_price); 
    LCD_DisplayStringLine(Line3, buf); 
    sprintf((char *)buf, "   CNBR:%.2f", VNBR_price); 
    LCD_DisplayStringLine(Line5, buf);

}
extern uint32_t frq_pwm;
extern int to_deal_string;
void menu_main(void){
    static int menu_num;
	static char pwm_begin = 0;
    uint8_t key_state;
    key_state = key_get_state();
    if(key_state & KEY1_PRESS){
        menu_num++;
        if(menu_num > 1){
            menu_num = 0;
        }
		LCD_Clear(Black);
    }
    if(key_state & KEY4_PRESS){
		uint8_t led_state = led_read();
		pwm_begin = !pwm_begin;
		if(pwm_begin){
			led_state |= 1 << 1;
			HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
		}else{
			led_state &= ~(1 << 1);
			HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
		}
		led_write(led_state);	
    }
    switch (menu_num){
        case 0:
            menu_deal0(key_state);
            break;
        case 1:
            menu_deal1(key_state);
            break;
    }
	if(to_deal_string){
		deal_string();
		to_deal_string = 0;
	}
//    uint8_t buf[30];
//    sprintf((char *)buf, "   frq:%d      ", frq_pwm); 
//    LCD_DisplayStringLine(Line8, buf); 
}



