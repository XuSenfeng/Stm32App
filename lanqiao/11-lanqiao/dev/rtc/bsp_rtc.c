#include "rtc/bsp_rtc.h"
#include "lcd/lcd.h"
#include "stdio.h"
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
    static int i;
    i++;
    char buf[30];
    sprintf(buf, "AlarmA %d", i);
    LCD_DisplayStringLine(Line5, (uint8_t *)buf);
}


