/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_usart.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01        百问科技      创建文件
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "stm32f1xx_hal.h"
#include "ring_buffer.h"

/*
 *  函数名：void EnableDebugIRQ(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：使能USART1的中断
*/
void EnableDebugIRQ(void);
extern ring_buffer test_buffer;
extern ring_buffer uart3_buffer;
extern ring_buffer uart2_buffer;
/*
 *  函数名：void DisableDebugIRQ(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：失能USART1的中断
*/
extern void DisableDebugIRQ(void);
void EnableUART3IRQ(void);
void USART3_Write(char *buf , int len);
int USART3_Read(char *c, int timeout);
void UART3_Lock_Init(void);
void DisableUART3IRQ(void);
void EnableUART2IRQ(void);
int USART2_Read(char *c, int timeout);

#endif /* __DRIVER_USART_H */
