#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h" 
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "fontupd.h"
#include "text.h"
#include "touch.h"	 
#include "iap.h"
#include "fattester.h"	
/************************************************
 ALIENTEK ��ӢSTM32F103������ ʵ��41
 ��дʶ��ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/  


int main(void)
{
    uint8_t key;
	FRESULT ERR_fs;
    HAL_Init();                                 /* ��ʼ��HAL�� */
    Stm32_Clock_Init(RCC_PLL_MUL9);         /* ����ʱ��, 72Mhz */
	W25QXX_Init();
    delay_init(72);                             /* ��ʱ��ʼ�� */
    usart_init(115200);                         /* ���ڳ�ʼ��Ϊ115200 */
    LCD_Init();                                 /* ��ʼ��LCD */
    KEY_Init();                                 /* ��ʼ������ */
	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	exfuns_init();						//Ϊfatfs��ر��������ڴ�  

	key = KEY_Scan(0);
	if (key != KEY2_PRES){
		LCD_LED=1;
		iap_run();
	}
	ERR_fs = f_mount(fs[0],"0:",1); 				//����SD�� 
	if(ERR_fs != FR_OK)
	{
		LCD_ShowString(30,  50, 200, 16, 16, "SD Init ERR");
		LCD_ShowString(30,  70, 200, 16, 16, "KEY1 to RUNNING");

		while(1){
			key = KEY_Scan(0);
			if (key != KEY1_PRES){
				LCD_LED=0;
				iap_run();
			}
		}
	}
    LCD_ShowString(30,  50, 200, 16, 16, "STM32");
    LCD_ShowString(30,  70, 200, 16, 16, "IAP-XvSenfeng");
    LCD_ShowString(30,  90, 200, 16, 16, "2024-1-15");
    LCD_ShowString(30, 110, 200, 16, 16, "https://github.com/XuSenfeng");
	HAL_Delay(5);
    while (1)
    {
			iap_get_app_path("0:");

	}

}

