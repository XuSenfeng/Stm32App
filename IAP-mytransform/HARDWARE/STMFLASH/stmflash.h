/**
 ****************************************************************************************************
 * @file        stmflash.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-26
 * @brief       STM32�ڲ�FLASH��д ��������
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
 * V1.0 20200426
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __STMFLASH_H
#define __STMFLASH_H

#include "sys.h"


/* FLASH��ʼ��ַ */
#define STM32_FLASH_SIZE        0x80000         /* STM32 FLASH �ܴ�С */
#define STM32_FLASH_BASE        0x08000000      /* STM32 FLASH ��ʼ��ַ */

 /* STM32F103 ������С */
#if STM32_FLASH_SIZE < 256 * 1024
#define STM32_SECTOR_SIZE   1024                /* ����С��256K�� F103, ������СΪ1K�ֽ� */
#else
#define STM32_SECTOR_SIZE   2048                /* �������ڵ�����256K�� F103, ������СΪ2K�ֽ� */
#endif

/* FLASH������ֵ */
#define STM32_FLASH_KEY1        0X45670123
#define STM32_FLASH_KEY2        0XCDEF89AB

/* ��̬����(����stmflash.c����) */
static void stmflash_unlock(void);                  /* ����STM32 �ڲ�FLASH */
static void stmflash_lock(void);                    /* ����STM32 �ڲ�FLASH */
static uint8_t stmflash_get_error_status(void);     /* ��ȡFLASH����״̬ */
static uint8_t stmflash_wait_done(uint32_t time);   /* �ȴ�������� */
static uint8_t stmflash_erase_sector(uint32_t saddr);                   /* �������� */
static uint8_t stmflash_write_halfword(uint32_t faddr, uint16_t data);  /* FLASHд���� */

/* �ӿں���(�ⲿ�ɵ���) */
uint16_t stmflash_read_halfword(uint32_t faddr);                        /* FLASH������ */
void stmflash_read(uint32_t raddr, uint16_t *pbuf, uint16_t length);    /* ��ָ����ַ��ʼ����ָ�����ȵ����� */
void stmflash_write(uint32_t waddr, uint16_t *pbuf, uint16_t length);   /* ��FLASH ָ��λ��, д��ָ�����ȵ�����(�Զ�����) */

/* ���Ժ��� */
void test_write(uint32_t waddr, uint16_t wdata);

#endif


















