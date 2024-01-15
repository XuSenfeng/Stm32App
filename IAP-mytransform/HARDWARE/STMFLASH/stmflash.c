/**
 ****************************************************************************************************
 * @file        stmflash.c
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

#include "usart.h"
#include "delay.h"
#include "stmflash.h"

/**
 * @brief       ��ָ����ַ��ȡһ������ (16λ����)
 * @param       faddr   : ��ȡ��ַ (�˵�ַ����Ϊ2�ı���!!)
 * @retval      ��ȡ�������� (16λ)
 */
uint16_t stmflash_read_halfword(uint32_t faddr)
{
    return *(volatile uint16_t *)faddr;
}

/**
 * @brief       ��ָ����ַ��ʼ����ָ�����ȵ�����
 * @param       raddr : ��ʼ��ַ
 * @param       pbuf  : ����ָ��
 * @param       length: Ҫ��ȡ�İ���(16λ)��,��2���ֽڵ�������
 * @retval      ��
 */
void stmflash_read(uint32_t raddr, uint16_t *pbuf, uint16_t length)
{
    uint16_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_halfword(raddr);    /* ��ȡ2���ֽ� */
        raddr += 2; /* ƫ��2���ֽ� */
    }
}

/**
 * @brief       ������д��
                ��������ļ����Ѿ���ԭ����������������д��
 * @param       waddr   : ��ʼ��ַ (�˵�ַ����Ϊ2�ı���!!,����д�����!)
 * @param       pbuf    : ����ָ��
 * @param       length  : Ҫд��� ����(16λ)��
 * @retval      ��
 */
void stmflash_write_nocheck(uint32_t waddr, uint16_t *pbuf, uint16_t length)
{
    uint16_t i;
    for (i = 0; i < length; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, waddr, pbuf[i]);
        waddr += 2;     /* ָ����һ������ */
    }
}

/**
 * @brief       ��FLASH ָ��λ��, д��ָ�����ȵ�����(�Զ�����)
 *   @note      �ú����� STM32 �ڲ� FLASH ָ��λ��д��ָ�����ȵ�����
 *              �ú������ȼ��Ҫд��������Ƿ��ǿ�(ȫ0XFFFF)��?, ���
 *              ����, ���Ȳ���, �����, ��ֱ������������д������.
 *              ���ݳ��Ȳ�������ʱ���Զ����ز���ǰ������
 * @param       waddr   : ��ʼ��ַ (�˵�ַ����Ϊ2�ı���!!,����д�����!)
 * @param       pbuf    : ����ָ��
 * @param       length  : Ҫд��� ����(16λ)��
 * @retval      ��
 */
uint16_t g_flashbuf[STM32_SECTOR_SIZE / 2]; /* �����2K�ֽ� */
void stmflash_write(uint32_t waddr, uint16_t *pbuf, uint16_t length)
{
    uint32_t secpos;        /* ������ַ */
    uint16_t secoff;        /* ������ƫ�Ƶ�ַ(16λ�ּ���) */
    uint16_t secremain;     /* ������ʣ���ַ(16λ�ּ���) */
    uint16_t i;
    uint32_t offaddr;       /* ȥ��0X08000000��ĵ�ַ */
    FLASH_EraseInitTypeDef flash_eraseop;
    uint32_t erase_addr;    /* �����������ֵΪ���������������ַ */

    if (waddr < STM32_FLASH_BASE || (waddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return;     /* �Ƿ���ַ */
    }

    HAL_FLASH_Unlock();     /* FLASH���� */

    offaddr = waddr - STM32_FLASH_BASE;             /* ʵ��ƫ�Ƶ�ַ. */
    secpos = offaddr / STM32_SECTOR_SIZE;           /* ������ַ  0~255 for STM32F103ZET6 */
    secoff = (offaddr % STM32_SECTOR_SIZE) / 2;     /* �������ڵ�ƫ��(2���ֽ�Ϊ������λ.) */
    secremain = STM32_SECTOR_SIZE / 2 - secoff;     /* ����ʣ��ռ��С */
    if (length <= secremain)
    {
        secremain = length; /* �����ڸ�������Χ */
    }

    while (1)
    {
        stmflash_read(secpos * STM32_SECTOR_SIZE + STM32_FLASH_BASE, g_flashbuf, STM32_SECTOR_SIZE / 2); /* ������������������ */
        for (i = 0; i < secremain; i++)                                                              /* У������ */
        {
            if (g_flashbuf[secoff + i] != 0XFFFF)
            {
                break;      /* ��Ҫ���� */
            }
        }
        if (i < secremain)  /* ��Ҫ���� */
        { 
            flash_eraseop.TypeErase = FLASH_TYPEERASE_PAGES;     /* ѡ��ҳ���� */
            flash_eraseop.NbPages = 1;
            flash_eraseop.PageAddress = secpos * STM32_SECTOR_SIZE + STM32_FLASH_BASE;  /* Ҫ���������� */
            HAL_FLASHEx_Erase( &flash_eraseop, &erase_addr);

            for (i = 0; i < secremain; i++)         /* ���� */
            {
                g_flashbuf[i + secoff] = pbuf[i];
            }
            stmflash_write_nocheck(secpos * STM32_SECTOR_SIZE + STM32_FLASH_BASE, g_flashbuf, STM32_SECTOR_SIZE / 2); /* д���������� */
        }
        else
        {
            stmflash_write_nocheck(waddr, pbuf, secremain); /* д�Ѿ������˵�,ֱ��д������ʣ������. */
        }
        if (length == secremain)
        {
            break; /* д������� */
        }
        else       /* д��δ���� */
        {
            secpos++;               /* ������ַ��1 */
            secoff = 0;             /* ƫ��λ��Ϊ0 */
            pbuf += secremain;      /* ָ��ƫ�� */
            waddr += secremain * 2; /* д��ַƫ��(16λ���ݵ�ַ,��Ҫ*2) */
            length -= secremain;    /* �ֽ�(16λ)���ݼ� */
            
            if (length > (STM32_SECTOR_SIZE / 2))
            {
                secremain = STM32_SECTOR_SIZE / 2; /* ��һ����������д���� */
            }
            else
            {
                secremain = length; /* ��һ����������д���� */
            }
        }
    }

    HAL_FLASH_Lock(); /* ���� */
}

/******************************************************************************************/
/* �����ô��� */

/**
 * @brief       ����д����(д1����)
 * @param       waddr : ��ʼ��ַ
 * @param       wdata : Ҫд�������
 * @retval      ��ȡ��������
 */
void test_write(uint32_t waddr, uint16_t wdata)
{
    stmflash_write(waddr, &wdata, 1); /* д��һ������ */
}
