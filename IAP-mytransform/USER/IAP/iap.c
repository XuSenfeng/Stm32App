/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200512
 * ��һ�η���
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
uint16_t g_iapbuf[1024];       /* 2K�ֽڻ��� */

/**
 * @brief       IAPд��APP BIN
 * @param       appxaddr : Ӧ�ó������ʼ��ַ
 * @param       appbuf   : Ӧ�ó���CODE
 * @param       appsize  : Ӧ�ó����С(�ֽ�)
 * @retval      ��
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* ��ǰд��ĵ�ַ */
    uint8_t *dfu = appbuf;
	
    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* ƫ��2���ֽ� */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
            fwaddr += 2048;     /* ƫ��2048  16 = 2 * 8  ����Ҫ����2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* ������һЩ�����ֽ�д��ȥ */
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
						 if (key == KEY1_PRES)   /* WKUP����,���¹̼���FLASH */
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
	if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) /* �ж�FLASH�����Ƿ���APP,�еĻ�ִ�� */
	{
		printf("��ʼִ��FLASH�û�����!!\r\n\r\n");
		delay_ms(10);
		iap_load_app(FLASH_APP1_ADDR);/* ִ��FLASH APP���� */

	}
	else
	{
		printf("û�п������еĹ̼�!\r\n");
		LCD_ShowString(30, 190, 200, 16, 16, "No APP!");
	}
}

FRESULT iap_write_appbin_name(uint32_t appxaddr, char *file_name, int size)
{
    uint32_t temp;
    uint32_t fwaddr = appxaddr; /* ��ǰд��ĵ�ַ */
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
		stmflash_write(fwaddr, g_iapbuf, temp/2 + temp%2);  /* ������һЩ�����ֽ�д��ȥ */
		fwaddr += 2048;
		i+=temp;
		LCD_ShowxNum(30, 230, i*100/size, 7, 16, 0);
	}while(temp==2048);
	LCD_ShowString(30, 230, 200, 16, 16, (unsigned char*)"Success");
	return FR_OK;
}
/**
 * @brief       ��ת��Ӧ�ó����(ִ��APP)
 * @param       appxaddr : Ӧ�ó������ʼ��ַ

 * @retval      ��
 */
void iap_load_app(uint32_t appxaddr)
{
    if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* ���ջ����ַ�Ƿ�Ϸ�.���Է����ڲ�SRAM��64KB(0x20000000) */
    {
        /* �û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ) */
        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
        
        /* ��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ) */
        MSR_MSP(*(volatile uint32_t *)appxaddr);
        
        /* ��ת��APP */
        jump2app();
    }
}












