#include "lcd/menu.h"
#include "lcd/lcd.h"
#include "stdio.h"
#include "key/key.h"
#include "adc/bsp_adc.h"
#include "i2c/eeprom.h"
#include "time/time.h"

extern uint32_t frq, duty;
uint32_t data_K = 1, data_R = 1;
uint32_t data_nK = 1, data_nR = 1;
extern int pwm_change_time;
double max_H, max_L;


//这一个是菜单处理函数
void menu_init(){
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
}
uint8_t menu_num;//记录这时候的menu
void deal_menu0(uint8_t key_state){
	int direction = PWM_GetDirection();
	if(key_state & KEY2_Press){
		if(direction == 0)
			PWM_SetDirection(1);
		else
			PWM_SetDirection(0);
	}
	extern int pwm_duty;
	int duty = pwm_duty;
	LCD_DisplayStringLine(Line1, "        DATA");
	uint8_t buf[30];
	sprintf((char *)buf, "     M=%c     ", direction? 'L' : 'H');
	LCD_DisplayStringLine(Line3, buf);
	sprintf((char *)buf, "     P=%d%%     ", duty);
	LCD_DisplayStringLine(Line4, buf);
	double v = frq * 2 *3.14 *data_R / 100 /data_K;
	
	if(direction){
		if(max_L < v)
			max_L = v;
	}else{
		if(max_H < v)
			max_H = v;
	}
	sprintf((char *)buf, "     V=%.1f     ", v);
	LCD_DisplayStringLine(Line5, buf);
	
}
void deal_menu1(uint8_t key_state){
	static char which = 0;
	if(key_state & KEY2_Press){
		which = ~which;
	}
	if(key_state & KEY3_Press){
		//这一个是增加的处理位置
		if(which){
			data_nK==10? data_nK = 1: data_nK++;
		}else{
			data_nR==10? data_nR = 1: data_nR++;
		}
	}
	if(key_state & KEY4_Press){
		//这一个是增加的处理位置
		if(which){
			data_nK==1? data_nK = 10: data_nK--;
		}else{
			data_nR==1? data_nR = 10: data_nR--;
		}
	}
	uint8_t buf[30];

	LCD_DisplayStringLine(Line1, "        PARA");
	sprintf((char *)buf, "     R=%d    ", data_nR);
	LCD_DisplayStringLine(Line3, buf);
	sprintf((char *)buf, "     K=%d    ", data_nK);
	LCD_DisplayStringLine(Line4, buf);

}
extern int deal_pwm;
void deal_menu2(uint8_t key_state){
	if(key_state & KEY4_LONG_Press){
		deal_pwm = 0;
	}
	if(key_state & KEY4_Press)
		deal_pwm = 1;
	uint8_t buf[30];
	LCD_DisplayStringLine(Line1, "        RECD");
	sprintf((char *)buf, "     N=%d    ", pwm_change_time);
	LCD_DisplayStringLine(Line3, buf);
	sprintf((char *)buf, "     MH=%.1f     ", max_H);
	LCD_DisplayStringLine(Line4, buf);
	sprintf((char *)buf, "     ML=%.1f     ", max_L);
	LCD_DisplayStringLine(Line5, buf);
}

void menu_main(){
	uint8_t key_state;
	key_state = key_get_state();  
	if(key_state & KEY1_Press){
		if(menu_num == 1){
			data_K = data_nK;
			data_R = data_nR;
		}
		menu_num++;
		if(menu_num >= 3){
			menu_num = 0;
		}
		LCD_Clear(Black);
	}
	if(menu_num == 0){
		deal_menu0(key_state);
	}else if(menu_num == 1){
		deal_menu1(key_state);
	}else if(menu_num == 2){
		deal_menu2(key_state);
	}

	uint8_t buf[30];
	static char i = 0;
	eeprom_write(22, i++);
	HAL_Delay(1);
	sprintf((char *)buf, " eep=%d  ", eeprom_read(22));
    LCD_DisplayStringLine(Line6, buf);

}



