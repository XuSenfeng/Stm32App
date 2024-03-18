#include "menu/menu.h"
#include "lcd/lcd.h"
#include "key/key.h"
#include "stdio.h"
#include "adc/myadc.h"
#include "eeprom/eeprom.h"
#include "uart/myuart.h"
#include "led/led.h"
double voltage_value[2][100];
char voltage_pos_1, voltage_pos_2;
double voltage_max_1, voltage_max_2;
double voltage_min_1 = 4, voltage_min_2 = 4;
double voltage_all_1, voltage_all_2;
extern int frq_pwm, test_frq;
int data_X = 0, data_Y = 0;
int pwm_mode = 0;
//初始化函数
void menu_init(void){
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
    data_X = eeprom_read(20);
	HAL_Delay(1);
    data_Y = eeprom_read(21);
	HAL_Delay(1);
}



//第一个界面
void menu_main0(uint8_t key_state){
	if(key_state & KEY4_PRESS)
	{
		read_adc();
		voltage_value[0][voltage_pos_1++] = adcs[0].adc_voltage;
		voltage_value[1][voltage_pos_2++] = adcs[1].adc_voltage;
		if(adcs[0].adc_voltage > voltage_max_1)
			voltage_max_1 = adcs[0].adc_voltage;
		if(adcs[1].adc_voltage > voltage_max_2)
			voltage_max_2 = adcs[1].adc_voltage;
		if(adcs[0].adc_voltage < voltage_min_1)
			voltage_min_1 = adcs[0].adc_voltage;
		if(adcs[1].adc_voltage < voltage_min_2)
			voltage_min_2 = adcs[1].adc_voltage;
        voltage_all_1 += adcs[0].adc_voltage;
        voltage_all_2 += adcs[1].adc_voltage; 
	}
	LCD_DisplayStringLine(Line1, "        DATA");
	uint8_t buf[30];
	sprintf((char *)buf, "     PA4:%.2f     ", adcs[0].adc_voltage);
    LCD_DisplayStringLine(Line3, buf);
    sprintf((char *)buf, "     PA5:%.2f     ", adcs[1].adc_voltage);
    LCD_DisplayStringLine(Line4, buf);
    sprintf((char *)buf, "     PA1:%d     ", frq_pwm);
    LCD_DisplayStringLine(Line5, buf);

}
//第二个界面
void menu_main1(uint8_t key_state){
    uint8_t buf[30];
    LCD_DisplayStringLine(Line1, "        PARA");
    if(key_state & KEY2_PRESS){
        data_X ++;
        if(data_X > 4){
            data_X = 1;
        }
        eeprom_write(20, data_X);
    }
    if(key_state & KEY3_PRESS){
        data_Y ++;
        if(data_Y > 4){
            data_Y = 1;
        }
        eeprom_write(21, data_Y);
    }
    if(key_state & KEY4_PRESS){
		pwm_mode = !pwm_mode;
		uint8_t led_state = led_get_pin();
		if(pwm_mode){
			led_state &= ~(1<<0);
			led_state |= (1<<1);
		}
		else{
			led_state |= (1<<0);
			led_state &= ~(1<<1);
		}
		led_set_pin(led_state);
		
    }
    sprintf((char *)buf, "     X:%d     ", data_X);
    LCD_DisplayStringLine(Line3, buf);
    sprintf((char *)buf, "     Y:%d     ", data_Y);
    LCD_DisplayStringLine(Line4, buf);
}

void menu_main2(uint8_t key_state){
	static char which_channel;
    if(key_state & KEY4_PRESS){
        which_channel = !which_channel;
    }
	if(key_state & KEY4_LOG_PRESS)
	{
		voltage_pos_1 = 0;
		voltage_max_1 = 0;
		voltage_min_1 = 4;
		voltage_all_1 = 0;
		voltage_pos_2 = 0;
		voltage_max_2 = 0;
		voltage_min_2 = 4;
		voltage_all_2 = 0;
	}
	uint8_t buf[30];
	if(which_channel == 0){
		LCD_DisplayStringLine(Line1, "        REC-PA4");
		sprintf((char *)buf, "     N=%d  ", voltage_pos_1);
		LCD_DisplayStringLine(Line3, buf);
		sprintf((char *)buf, "     A=%.2f  ", voltage_max_1);
		LCD_DisplayStringLine(Line4, buf);
		sprintf((char *)buf, "     T=%.2f  ", voltage_min_1 > 3.3 ? 0 : voltage_min_1);
		LCD_DisplayStringLine(Line5, buf);
		sprintf((char *)buf, "     H=%.2f  ", voltage_pos_1 ? voltage_all_1 / voltage_pos_1 : 0);
		LCD_DisplayStringLine(Line6, buf);
	}else{
		LCD_DisplayStringLine(Line1, "        REC-PA5");
        sprintf((char *)buf, "     N=%d  ", voltage_pos_2);
		LCD_DisplayStringLine(Line3, buf);
        sprintf((char *)buf, "     A=%.2f  ", voltage_max_2);
        LCD_DisplayStringLine(Line4, buf);
        sprintf((char *)buf, "     T=%.2f  ", voltage_min_2 > 3.3 ? 0 : voltage_min_2);
        LCD_DisplayStringLine(Line5, buf);
        sprintf((char *)buf, "     H=%.2f  ", voltage_pos_2 ? voltage_all_2 / voltage_pos_2 : 0);
        LCD_DisplayStringLine(Line6, buf);

	}
	
}


extern int usart_to_deal;
void menu_main(void){
    uint8_t key_state = key_get_state();
    static int menu_now = 0;   
    if(key_state & KEY1_PRESS){
        menu_now ++;
        if(menu_now >= 3){
            menu_now = 0;
        }
		LCD_Clear(Black);
    }
    switch (menu_now)
    {
    case 0:
        menu_main0(key_state);
        break;
    case 1:
        menu_main1(key_state);
        break;
    case 2:
        menu_main2(key_state);
        break;
    }
//	uint8_t buf[30];
//	sprintf((char *)buf, " frq = %d  ", test_frq);
//    LCD_DisplayStringLine(Line7, buf);
//	if(usart_to_deal){
//		usart_deal_message();
//		LCD_Clear(Black);
//		LCD_DisplayStringLine(Line0, "display test");
//	}
	
}

