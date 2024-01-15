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
 ALIENTEK 精英STM32F103开发板 实验41
 手写识别实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/  


int main(void)
{
    uint8_t key;
	FRESULT ERR_fs;
    HAL_Init();                                 /* 初始化HAL库 */
    Stm32_Clock_Init(RCC_PLL_MUL9);         /* 设置时钟, 72Mhz */
	W25QXX_Init();
    delay_init(72);                             /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    LCD_Init();                                 /* 初始化LCD */
    KEY_Init();                                 /* 初始化按键 */
	my_mem_init(SRAMIN);				//初始化内部内存池
	exfuns_init();						//为fatfs相关变量申请内存  

	key = KEY_Scan(0);
	if (key != KEY2_PRES){
		LCD_LED=1;
		iap_run();
	}
	ERR_fs = f_mount(fs[0],"0:",1); 				//挂载SD卡 
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

