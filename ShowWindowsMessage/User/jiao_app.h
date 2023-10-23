#ifndef __JIAO_APP_H
#define __JIAO_APP_H

#include "jiao_FIFO.h"
#define ETMP_OUT 		10
#define SHOW_STR_TIME		15
#define MAX_STR_SHOW		50
void LCD_Show_Init(void);
void jiao_App_Init(void);
void LCD_Test(void);
#include "main.h"
#include "stdio.h"
#include "bsp_ili9341_lcd.h"
#endif //__JIAO_APP_H
