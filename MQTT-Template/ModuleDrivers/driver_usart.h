/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.h
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2020.6.6      v01        ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "stm32f1xx_hal.h"
#include "ring_buffer.h"

/*
 *  ��������void EnableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʹ��USART1���ж�
*/
void EnableDebugIRQ(void);
extern ring_buffer test_buffer;
extern ring_buffer uart3_buffer;
/*
 *  ��������void DisableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʧ��USART1���ж�
*/
extern void DisableDebugIRQ(void);
void EnableUART3IRQ(void);
void USART3_Write(char *buf , int len);
int USART3_Read(char *c, int timeout);
void UART3_Lock_Init(void);
void DisableUART3IRQ(void);
#endif /* __DRIVER_USART_H */
