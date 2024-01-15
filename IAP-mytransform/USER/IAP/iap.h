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

#ifndef __IAP_H
#define __IAP_H

#include "sys.h"
#include "ff.h"



typedef void (*iapfun)(void);                   /* ����һ���������͵Ĳ��� */

#define FLASH_APP1_ADDR         0x08010000      /* ��һ��Ӧ�ó�����ʼ��ַ(������ڲ�FLASH)
                                                 * ���� 0X08000000~0X0800FFFF(64KB) �Ŀռ�Ϊ Bootloader ʹ��
                                                 */

void iap_get_app_path(char *path);

void iap_load_app(uint32_t appxaddr);   /* ��ת��APP����ִ�� */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* ��ָ����ַ��ʼ,д��bin */
FRESULT iap_write_appbin_name(uint32_t appxaddr, char *file_name, int size);
void iap_run(void);

#endif







































