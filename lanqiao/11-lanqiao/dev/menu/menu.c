#include "menu/menu.h"
#include "lcd/lcd.h"
#include "rtc.h"
#include "stdio.h"
#include "key/key.h"
#include "led/led.h"
#include "eeprom/eeprom.h"


char RTC_set_time, Alarm_set_time;

void menu_init(void){
	RTC_set_time = eeprom_read(0);
	Alarm_set_time = eeprom_read(1);

	RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours = eeprom_read32(3);
	sAlarm.AlarmTime.Minutes = eeprom_read32(7);
	sAlarm.AlarmTime.Seconds = eeprom_read32(11);
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_A;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
	
	
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
}
HAL_StatusTypeDef rtc_statue;
RTC_TimeTypeDef Time;
RTC_DateTypeDef Data;

int menu_num = 0;
int which_to_deal = 0;
void menu_init_1(void){
	which_to_deal = 0;
}
//这一个是主页面, 显示时间
void menu_deal_0(uint8_t key_state){
	uint8_t led_state;
	uint8_t buf[30];
    LCD_DisplayStringLine(Line1, "      DATA");
    rtc_statue = HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
    if(rtc_statue == HAL_OK){
        sprintf((char*)buf, "   RTC:%02d:%02d:%02d", Time.Hours, Time.Minutes, Time.Seconds);
        LCD_DisplayStringLine(Line3, buf);
    }
    else{
        LCD_DisplayStringLine(Line3, "Error");
    }
    rtc_statue = HAL_RTC_GetDate(&hrtc, &Data, RTC_FORMAT_BIN);
	if(key_state & KEY1_PRESS){
        menu_num=1;
		led_state = led_read();
		led_state |= 1<<1;
		led_write(led_state);
		LCD_Clear(Black);
		menu_init_1();
    }
	if(key_state & KEY2_PRESS){
        menu_num=2;
		led_state = led_read();
		led_state |= 1<<2;
		led_write(led_state);
		menu_init_1();

		LCD_Clear(Black);
    }
}


#include "usart.h"
#include "string.h"
//设置时间的函数
void menu_deal_1(uint8_t key_state){
	uint8_t buf[30];
	//处理按键, 对数值进行加减
	if((key_state & KEY3_PRESS) ||(key_state & KEY3_LONG_PRESS)){
		switch (which_to_deal){
			case 0:
				Time.Hours >= 23 ?Time.Hours = 0: Time.Hours++;
				break;
			case 1:
				Time.Minutes >= 59 ?Time.Minutes = 0: Time.Minutes++;
				break;
			case 2:
				Time.Seconds >= 59 ?Time.Seconds = 0: Time.Seconds++;
				break;
		}
	}
	if((key_state & KEY4_PRESS) || (key_state & KEY4_LONG_PRESS)){
		switch (which_to_deal){
			case 0:
				Time.Hours <=0 ?Time.Hours = 23: Time.Hours--;
				break;
			case 1:
				Time.Minutes <= 0 ?Time.Minutes = 59: Time.Minutes--;
				break;
			case 2:
				Time.Seconds <= 0 ?Time.Seconds = 59: Time.Seconds--;
				break;
		}
	}
	if(key_state & KEY2_PRESS){
		which_to_deal++;
		if(which_to_deal > 2)
			which_to_deal = 0;
	}
    LCD_DisplayStringLine(Line1, "   RTC-SETTING");
	sprintf((char *)buf, "   RTC:%02d:%02d:%02d",Time.Hours, Time.Minutes, Time.Seconds);
	switch (which_to_deal){
		case 0:
			LCD_DisplayStringLineColor(Line3, buf, 7, 9, Red);
			break;
		case 1:
			LCD_DisplayStringLineColor(Line3, buf, 10, 12, Red);
			break;
		case 2:
			LCD_DisplayStringLineColor(Line3, buf, 13, 15, Red);
			break;
	}
	
	//退出, 把设置的时间写入
	if(key_state & KEY1_PRESS){
		sprintf((char *)buf, "New RTC:%02d:%02d:%02d\r\n",Time.Hours, Time.Minutes, Time.Seconds);
		HAL_UART_Transmit(&huart1, buf, strlen((char *)buf), 1000);
		menu_num = 0;
		HAL_RTC_SetTime(&hrtc, &Time, RTC_FORMAT_BIN);
		int led_state;
		led_state = led_read();
		led_state &= ~(1<<1);
		led_write(led_state);
		RTC_set_time++;//记录一下实际的次数
		eeprom_write(0, RTC_set_time);
	}
}
//这一个是定时器处理函数
void menu_deal_2(uint8_t key_state){
	uint8_t buf[30];
	if((key_state & KEY3_PRESS) ||(key_state & KEY3_LONG_PRESS)){
		switch (which_to_deal){
			case 0:
				Time.Hours >= 23 ?Time.Hours = 0: Time.Hours++;
				break;
			case 1:
				Time.Minutes >= 59 ?Time.Minutes = 0: Time.Minutes++;
				break;
			case 2:
				Time.Seconds >= 59 ?Time.Seconds = 0: Time.Seconds++;
				break;
		}
	}
	if((key_state & KEY4_PRESS) || (key_state & KEY4_LONG_PRESS)){
		switch (which_to_deal){
			case 0:
				Time.Hours <=0 ?Time.Hours = 23: Time.Hours--;
				break;
			case 1:
				Time.Minutes <= 0 ?Time.Minutes = 59: Time.Minutes--;
				break;
			case 2:
				Time.Seconds <= 0 ?Time.Seconds = 59: Time.Seconds--;
				break;
		}
	}
	if(key_state & KEY1_PRESS){
		which_to_deal++;
		if(which_to_deal > 2)
			which_to_deal = 0;
	}
	//实际的显示
    LCD_DisplayStringLine(Line1, "   RTC-SETTING");
	sprintf((char *)buf, "   RTC:%02d:%02d:%02d",Time.Hours, Time.Minutes, Time.Seconds);
	switch (which_to_deal){
		case 0:
			LCD_DisplayStringLineColor(Line3, buf, 7, 9, Red);
			break;
		case 1:
			LCD_DisplayStringLineColor(Line3, buf, 10, 12, Red);
			break;
		case 2:
			LCD_DisplayStringLineColor(Line3, buf, 13, 15, Red);
			break;
	}
	
	//退出, 进行设置闹钟
	if(key_state & KEY2_PRESS){
		sprintf((char *)buf, "New Alarm:%02d:%02d:%02d\r\n",Time.Hours, Time.Minutes, Time.Seconds);
		HAL_UART_Transmit(&huart1, buf, strlen((char *)buf), 1000);
		menu_num = 0;
  		RTC_AlarmTypeDef sAlarm = {0};
		sAlarm.AlarmTime.Hours = Time.Hours;
		sAlarm.AlarmTime.Minutes = Time.Minutes;
		sAlarm.AlarmTime.Seconds = Time.Seconds;
		sAlarm.AlarmTime.SubSeconds = 0x0;
		sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
		sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
		sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
		sAlarm.AlarmDateWeekDay = 0x1;
		sAlarm.Alarm = RTC_ALARM_A;
		HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
		int led_state;
		led_state = led_read();
		led_state &= ~(1<<2);
		led_write(led_state);
		//记录一下使用eeprom保存的信息
		eeprom_write32(3, Time.Hours);
		eeprom_write32(7, Time.Minutes);
		eeprom_write32(11,Time.Seconds);
		eeprom_write(1, ++Alarm_set_time);
	}
}

void menu_main(void){
	
    uint8_t key_state = key_scan();


	switch (menu_num){
		case 0:
			menu_deal_0(key_state);
			break;
		case 1:
			menu_deal_1(key_state);
			break;
		case 2:
			menu_deal_2(key_state);
			break;
	
	}


}
