/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200512
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./IAP/iap.h"
#include "usart.h"
#include "delay.h"
#include "stmflash.h"
#include "ff.h"
#include "key.h"
#include "lcd.h"
#include "string.h"
iapfun jump2app;
uint16_t g_iapbuf[1024];       /* 2K字节缓存 */

/**
 * @brief       IAP写入APP BIN
 * @param       appxaddr : 应用程序的起始地址
 * @param       appbuf   : 应用程序CODE
 * @param       appsize  : 应用程序大小(字节)
 * @retval      无
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* 当前写入的地址 */
    uint8_t *dfu = appbuf;
	
    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* 偏移2个字节 */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
            fwaddr += 2048;     /* 偏移2048  16 = 2 * 8  所以要乘以2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* 将最后的一些内容字节写进去 */
    }
}

void iap_get_app_path(char *path)
{
	uint8_t key;
    FRESULT res;
    DIR dir;
    FILINFO fno;
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Error or end of dir */
            if (fno.fattrib & AM_DIR) {            /* Directory */
				sprintf((char *)g_iapbuf, "%s/%s", path, fno.fname);
				iap_get_app_path((char *)g_iapbuf);
            } else {                               /* File */
				sprintf((char *)g_iapbuf, "%s/%s", path, fno.fname);
				printf((char *)g_iapbuf);
				printf("\r\n");				
				if(strstr(fno.fname, ".bin") || strstr(fno.fname, ".BIN"))
				{

					LCD_ShowString(30, 190, 200, 16, 16, (unsigned char*)fno.fname);
					LCD_ShowString(30, 210, 200, 16, 16, (unsigned char*)"Key1: Run Key2: Next");
					while (1)
					{
						delay_ms(100);

						key = KEY_Scan(0);
						 if (key == KEY1_PRES)   /* WKUP按下,更新固件到FLASH */
						{
							iap_write_appbin_name(FLASH_APP1_ADDR, (char *)g_iapbuf, (int)fno.fsize);
							LCD_LED=1;
							iap_run();
							break;
						}else if (key == KEY2_PRES)
						{
							break;
						}
					}
				}
            }
        }
        f_closedir(&dir);
    } else {
        printf("Failed to open \"%s\". (%u)\n", path, res);
    }
}
void iap_run(void)
{
	if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) /* 判断FLASH里面是否有APP,有的话执行 */
	{
		printf("开始执行FLASH用户代码!!\r\n\r\n");
		delay_ms(10);
		iap_load_app(FLASH_APP1_ADDR);/* 执行FLASH APP代码 */

	}
	else
	{
		printf("没有可以运行的固件!\r\n");
		LCD_ShowString(30, 190, 200, 16, 16, "No APP!");
	}
}

FRESULT iap_write_appbin_name(uint32_t appxaddr, char *file_name, int size)
{
    uint32_t temp;
    uint32_t fwaddr = appxaddr; /* 当前写入的地址 */
    FIL fil;        /* File object */
    FRESULT fr;     /* FatFs return code */
	int i=0;
    /* Open a text file */
	printf("begin to update\r\n");
    fr = f_open(&fil, file_name, FA_READ);
    if (fr){ 
		printf("open fail %d\r\n", fr);
		return fr;
	}
	do{
		f_read(&fil, g_iapbuf, 2048, &temp);
		stmflash_write(fwaddr, g_iapbuf, temp/2 + temp%2);  /* 将最后的一些内容字节写进去 */
		fwaddr += 2048;
		i+=temp;
		LCD_ShowxNum(30, 230, i*100/size, 7, 16, 0);
	}while(temp==2048);
	LCD_ShowString(30, 230, 200, 16, 16, (unsigned char*)"Success");
	return FR_OK;
}
/**
 * @brief       跳转到应用程序段(执行APP)
 * @param       appxaddr : 应用程序的起始地址

 * @retval      无
 */
void iap_load_app(uint32_t appxaddr)
{
    if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* 检查栈顶地址是否合法.可以放在内部SRAM共64KB(0x20000000) */
    {
        /* 用户代码区第二个字为程序开始地址(复位地址) */
        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
        
        /* 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址) */
        MSR_MSP(*(volatile uint32_t *)appxaddr);
        
        /* 跳转到APP */
        jump2app();
    }
}












