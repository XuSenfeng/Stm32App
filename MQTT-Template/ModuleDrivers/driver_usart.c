/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.c
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2021.8.21      v01         ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#include "driver_usart.h"
#include "usart.h"
#include "main.h"
#include "ring_buffer.h"
#include <stdio.h>
#include "platform_mutex.h"
static volatile uint8_t txcplt_flag = 0;    // ������ɱ�־��1���0δ���
static volatile uint8_t rxcplt_flag = 0;    // ������ɱ�־��1���0δ���

static volatile uint8_t rx_data = 0;
//����AT�����һ��������
static platform_mutex_t at_recv_mutex;
/*
 *  ��������void EnableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʹ��USART1���ж�
*/
void EnableDebugIRQ(void)
{
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);    // ����USART1�жϵ����ȼ�
    HAL_NVIC_EnableIRQ(USART1_IRQn);            // ʹ��USART1���ж�
    
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);   // ʹ��USRAT1�ķ��ͺͽ����ж�
}
void EnableUART3IRQ(void)
{
    HAL_NVIC_SetPriority(USART3_IRQn, 7, 1);    // ����USART3�жϵ����ȼ�
    HAL_NVIC_EnableIRQ(USART3_IRQn);            // ʹ��USART3���ж�

	huart3.Instance->SR &= ~(USART_SR_RXNE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);   // ʹ��USRAT3�Ľ����ж�
}
/*
 *  ��������void DisableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʧ��USART1���ж�
*/
void DisableDebugIRQ(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);      // ʧ��USRAT1�ķ��ͺͽ����ж�
    
    HAL_NVIC_DisableIRQ(USART1_IRQn);   // ʧ��USART1���ж�
}
void DisableUART3IRQ(void)
{
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);      // ʧ��USRAT1�ķ��ͺͽ����ж�
    
    HAL_NVIC_DisableIRQ(USART3_IRQn);   // ʧ��USART1���ж�
}
/*
 *  ��������int fputc(int ch, FILE *f)
 *  ���������ch --> Ҫ���������
 *  �����������
 *  ����ֵ����
 *  �������ã�printf/putchar ��׼��������ĵײ��������
*/
int fputc(int ch, FILE *f)
{
    txcplt_flag = 0;
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)&ch, 1);
    while(txcplt_flag==0);
	return ch;
}

/*
 *  ��������int fgetc(FILE *f)
 *  ���������
 *  �����������
 *  ����ֵ�����յ�������
 *  �������ã�scanf/getchar ��׼��������ĵײ��������
*/
int fgetc(FILE *f)
{
    char c = 0;
    while(ring_buffer_read((unsigned char *)&c, &test_buffer) != 0);
    return c;
}

/*
 *  ��������void USART1_IRQHandler(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�USART1���жϷ�����
*/
void USART1_IRQHandler(void)
{
    unsigned char c = 0;
    if((USART1->SR &(1<<5)) != 0)   // �ж�USART1��״̬�Ĵ����ĵ���λ��RXNEλ�Ƿ���λ
    {
        c = USART1->DR; // RXNE=1������DR�Ĵ�����ֵ���ͽ������������浽��ʱ�����У�
        ring_buffer_write(c, &test_buffer); // �����ݱ��浽���λ�������
    }
    HAL_UART_IRQHandler(&huart1);   // HAL���е�UARTͳһ�жϷ�������ͨ���β��ж���Ҫ����˭���ж�
}

/*
 *  ��������void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *  ���������huart --> UART���豸���������ָ��UART�豸����һ��UART
 *  �����������
 *  ����ֵ����
 *  �������ã�HAL���е�UART������ɻص�����
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // �жϽ������Ƿ���USART1���UART�豸
    {
        rxcplt_flag = 1;    // ����˻ص�������������ָ�����ȵ������Ѿ���ɣ�����־��һ
    }
}

/*
 *  ��������void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 *  ���������huart --> UART���豸���������ָ��UART�豸����һ��UART
 *  �����������
 *  ����ֵ����
*  �������ã�HAL���е�UART������ɻص�����
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // �жϽ������Ƿ���USART1���UART�豸
    {
        txcplt_flag = 1;    // ����˻ص�������������ָ�����ȵ������Ѿ���ɣ�����־��һ
    }
}
volatile uint8_t ESP8266_FLOG = 0;
void USART3_IRQHandler(void)
{
	/* �����������RX�ж�
	 * �����ݶ�����, ���뻷��buffer
	 */

	uint32_t isrflags	= READ_REG(huart3.Instance->SR);
	char c;
	static char num=0, r_c;
	//printf("hi\n");
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET))
    {
		c = huart3.Instance->DR;
		if(num<5){
			if(c=='r' && num == 0)
			{
				num++;	
				r_c = c;
			}else if(c=='e' && r_c == 'r' && num == 1)
			{
				num++;
				r_c = c;
			}else if(c=='a' && r_c == 'e'&& num == 2)
			{
				num++;	
				r_c = c;
			}else if(c=='d' && r_c == 'a'&& num == 3)
			{
				num++;
				r_c = c;				
			}else if(c=='y' && r_c == 'd'&& num == 4)
			{
				num++;	
				r_c = c;
				ESP8266_FLOG = 1;
			}else
				num=0;
			return;
		}
		//д�뻷�λ�����
	  ring_buffer_write(c, &uart3_buffer);
		//����
	  platform_mutex_unlock_FromISR(&at_recv_mutex);
//	  while((huart1.Instance->SR & USART_SR_TXE)==0);
//	  huart1.Instance->DR = c;
    }
	HAL_UART_IRQHandler(&huart3);
}

void USART3_Write(char *buf , int len){
	int i=0;
	while(i<len)
	{
		while((huart3.Instance->SR & USART_SR_TXE)==0);
		huart3.Instance->DR = buf[i++];
	}
}
//�Ӵ���3�����ȡ������һ������
int USART3_Read(char *c, int timeout)
{
	int ret=1;
	while(ret){
		
		if(ring_buffer_read((unsigned char *)c, &uart3_buffer)==0)
		{
			return ret;
		}else{
			ret = platform_mutex_lock_timeout(&at_recv_mutex, timeout);
		}
	}
	return ret;
}

void UART3_Lock_Init()
{
		//��ʼ��������
	platform_mutex_init(&at_recv_mutex);
	platform_mutex_lock(&at_recv_mutex);
}
