#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	 
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//2.8寸/3.5寸/4.3寸/7寸 TFT液晶驱动
//支持驱动IC型号包括:ILI9341/NT35310/NT35510/SSD1963/ST7789等
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/9/17
//版本：V4.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//V4.0 20211110
//1，去掉大部分不常用驱动IC的支持
//2，新增对ST7789驱动IC的支持
//3，优化代码结构（精简源码）
//V4.1 20211208
//修改NT5510 ID读取方式,改为先发送秘钥,然后读取C500和C501,从而获取正确的ID(0X5510)
//V4.2 20211222
//解决因NT5510 ID读取（发送C501指令）导致SSD1963误触发软件复位进而读取不到ID问题，加延时解决
//////////////////////////////////////////////////////////////////////////////////

SRAM_HandleTypeDef TFTSRAM_Handler; //SRAM句柄(用于控制LCD)

//LCD的画笔颜色和背景色
u32 POINT_COLOR=0xFF000000; //画笔颜色
u32 BACK_COLOR =0xFFFFFFFF; //背景色

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(vu16 regval)
{
    regval = regval;        //使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG = regval;  //写入要写的寄存器序号
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(vu16 data)
{
    data = data;            //使用-O2优化的时候,必须插入的延时
    LCD->LCD_RAM = data;
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram;               //防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;         //写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;    //写入数据
}

//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);            //写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA();           //返回读到的值
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;        //写十六位GRAM
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
    while (i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 r, g, b;

    if (x >= lcddev.width || y >= lcddev.height)return 0;   //超过了范围,直接返回

    LCD_SetCursor(x, y);

    if (lcddev.id == 0X5510)    //5510 发送读GRAM指令
    {
        LCD_WR_REG(0X2E00);
    }
    else                        //其他IC(9341/5310/1963/7789)发送读GRAM指令
    {
        LCD_WR_REG(0X2E);
    }

    r = LCD_RD_DATA();          //假读

    if (lcddev.id == 0X1963)    //对1963来说,是真读
    {
        return r;               //1963直接读就可以
    }

    r = LCD_RD_DATA();          //实际坐标颜色

    //9341/5310/5510/7789 要分2次读出
    b = LCD_RD_DATA();
    g = r & 0XFF;               //对于 9341/5310/5510/7789, 第一次读取的是RG的值,R在前,G在后,各占8位
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  // 9341/5310/5510/7789 需要公式转换一下
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X5510)    //5510开启显示指令
    {
        LCD_WR_REG(0X2900);     //开启显示
    }
    else                        //9341/5310/1963/7789 等发送开启显示指令
    {
        LCD_WR_REG(0X29);       //开启显示
    }
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X5510)    //5510关闭显示指令
    {
        LCD_WR_REG(0X2800);     //关闭显示
    }
    else                        //9341/5310/1963/7789 等发送关闭显示指令
    {
        LCD_WR_REG(0X28);       //关闭显示
    }
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)   //x坐标需要变换
        {
            Xpos = lcddev.width - 1 - Xpos;
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
        }
        else
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        }

        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);

    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else     //9341/5310/7789等设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
}

//设置LCD的自动扫描方向
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9341/5310/5510/1963/7789等IC已经实际测试
//注意:其他函数可能会受到此函数设置的影响(尤其是9341),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;

    switch (dir)
    {
        case L2R_U2D://从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U://从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D://从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U://从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R://从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L://从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R://从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L://从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    if (lcddev.id == 0X5510)dirreg = 0X3600;
    else dirreg = 0X36;

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789)   //9341 & 7789 要设置BGR位
    {
        regval |= 0X08;
    }

    LCD_WriteReg(dirreg, regval);

    if (lcddev.id != 0X1963)   //1963不做坐标处理
    {
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    //设置显示区域(开窗)大小
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);        //设置光标位置
    LCD_WriteRAM_Prepare();     //开始写入GRAM
    LCD->LCD_RAM=POINT_COLOR; 
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)x = lcddev.width - 1 - x;

        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else     //9341/5310/7789等设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }

    LCD->LCD_REG=lcddev.wramcmd; 
    LCD->LCD_RAM=color; 
}

//SSD1963 背光设置
//pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(u8 pwm)
{
    LCD_WR_REG(0xBE);           //配置PWM输出
    LCD_WR_DATA(0x05);          //1设置PWM频率
    LCD_WR_DATA(pwm * 2.55);    //2设置PWM占空比
    LCD_WR_DATA(0x01);          //3设置C
    LCD_WR_DATA(0xFF);          //4设置D
    LCD_WR_DATA(0x00);          //5设置E
    LCD_WR_DATA(0x00);          //6设置F
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    lcddev.dir = dir;       //竖屏/横屏

    if (dir == 0)           //竖屏
    {
        lcddev.width = 240;
        lcddev.height = 320;
		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;

    }
    else     //横屏
    {
        lcddev.width = 320;
        lcddev.height = 240;
		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;

    }

    LCD_Scan_Dir(R2L_D2U);     //默认扫描方向
}

//设置窗口(对RGB屏无效),并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
    u16 twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    if (lcddev.id == 0X1963 && lcddev.dir != 1)     //1963竖屏特殊处理
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA((sx + width - 1) >> 8);
        LCD_WR_DATA((sx + width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(height >> 8);
        LCD_WR_DATA(height & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
    else     //9341/5310/7789/1963横屏 等 设置窗口
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_DATA(theight & 0XFF);
    }
}

//SRAM底层驱动，时钟使能，引脚分配
//此函数会被HAL_SRAM_Init()调用
//hsram:SRAM句柄
//jiao: 修改把FSMC_A10换为A16(PG0->PD11)
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
	GPIO_InitTypeDef GPIO_Initure;
  
      /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_FSMC_CLK_ENABLE();			//使能FSMC时钟
  
    /* Common GPIO configuration 背光引脚*/
  GPIO_Initure.Mode      = GPIO_MODE_OUTPUT_PP;  //推挽输出
  GPIO_Initure.Pull      = GPIO_PULLUP;
  GPIO_Initure.Speed     = GPIO_SPEED_FREQ_HIGH;

  
  GPIO_Initure.Pin=GPIO_PIN_12;
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
  
  //初始化复位引脚G11
	GPIO_Initure.Pin=GPIO_PIN_1;
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
  
  GPIO_Initure.Mode=GPIO_MODE_AF_PP; 
//  GPIO_Initure.Alternate=GPIO_AF12_FSMC;	//复用为FSMC
  
	//初始化PD0,1,4,5,8,9,10,14,15
	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|\
					         GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_15; 
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);
  
  	//初始化PE2,7,8,9,10,11,12,13,14,15
	GPIO_Initure.Pin=  GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//输出低电平
#define	digitalH(p,i)			{p->BSRR=i;}			  //设置为高电平		
void ILI9341_Rst( void )
{			
	digitalL( GPIOE,GPIO_PIN_1);	 //低电平复位

	HAL_Delay (0x1); 						   

	digitalH( GPIOE,GPIO_PIN_1);		 	 

	HAL_Delay (0x1);  	 	
	
}

//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
//    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;

    __HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
    GPIO_Initure.Pin=GPIO_PIN_12;            //PB0,背光控制
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure); 
	LCD_LED=1;
    TFTSRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;                
    TFTSRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    

    TFTSRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK1;                         //使用NE1
    TFTSRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;      //地址/数据线不复用
    TFTSRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_NOR;                  //NOR
    TFTSRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;     //16位数据宽度
    TFTSRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE;    //是否使能突发访问,仅对同步突发存储器有效,此处未用到
    TFTSRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;  //等待信号的极性,仅在突发模式访问下有用
    TFTSRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;       //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    TFTSRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;        //存储器写使能
    TFTSRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;               //等待使能位,此处未用到
    TFTSRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;            //读写使用不同的时序
    TFTSRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;   //是否使能同步传输模式下的等待信号,此处未用到
    TFTSRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;               //禁止突发写
    
	FSMC_ReadWriteTim.AddressSetupTime      = 0x00;
	FSMC_ReadWriteTim.AddressHoldTime       = 0x00;
	FSMC_ReadWriteTim.DataSetupTime         = 0x08;
	FSMC_ReadWriteTim.BusTurnAroundDuration = 0x00;
	FSMC_ReadWriteTim.CLKDivision           = 0x00;
	FSMC_ReadWriteTim.DataLatency           = 0x00;
	FSMC_ReadWriteTim.AccessMode            = FSMC_ACCESS_MODE_B;
    HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_ReadWriteTim);	
    delay_ms(50); // delay 50 ms 
	ILI9341_Rst();
    //尝试9341 ID的读取
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();          //dummy read
    lcddev.id = LCD_RD_DATA();          //读到0X00
    lcddev.id = LCD_RD_DATA();          //读取0X93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA();         //读取0X41
	
    if (lcddev.id != 0X9341)            //不是 9341 , 尝试看看是不是 ST7789
    {
        LCD_WR_REG(0X04);
        lcddev.id = LCD_RD_DATA();      //dummy read
        lcddev.id = LCD_RD_DATA();      //读到0X85
        lcddev.id = LCD_RD_DATA();      //读取0X85
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA();     //读取0X52
		//printf("id = %#x\r\n", lcddev.id);
        if (lcddev.id == 0X8552)        //将8552的ID转换成7789
        {
            lcddev.id = 0x7789;
			
        }

        if (lcddev.id != 0x7789)        //也不是ST7789, 尝试是不是 NT35310
        {
            LCD_WR_REG(0XD4);
            lcddev.id = LCD_RD_DATA();  //dummy read
            lcddev.id = LCD_RD_DATA();  //读回0X01
            lcddev.id = LCD_RD_DATA();  //读回0X53
            lcddev.id <<= 8;
            lcddev.id |= LCD_RD_DATA(); //这里读回0X10

            if (lcddev.id != 0X5310)    //也不是NT35310,尝试看看是不是NT35510
            {
                //发送秘钥（厂家提供,照搬即可）
                LCD_WriteReg(0xF000, 0x0055);
                LCD_WriteReg(0xF001, 0x00AA);
                LCD_WriteReg(0xF002, 0x0052);
                LCD_WriteReg(0xF003, 0x0008);
                LCD_WriteReg(0xF004, 0x0001);

                LCD_WR_REG(0xC500);             //读取ID高8位
                lcddev.id = LCD_RD_DATA();      //读回0X55
                lcddev.id <<= 8;

                LCD_WR_REG(0xC501);             //读取ID低8位
                lcddev.id |= LCD_RD_DATA();     //读回0X10
                delay_ms(50);

                if (lcddev.id != 0X5510)        //也不是NT5510,尝试看看是不是SSD1963
                {
                    LCD_WR_REG(0XA1);
                    lcddev.id = LCD_RD_DATA();
                    lcddev.id = LCD_RD_DATA();  //读回0X57
                    lcddev.id <<= 8;
                    lcddev.id |= LCD_RD_DATA(); //读回0X61

                    if (lcddev.id == 0X5761)lcddev.id = 0X1963; //SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
                }
            }
        }
    }

    printf(" LCD ID:%x\r\n", lcddev.id); //打印LCD ID

    if (lcddev.id == 0x7789)   //7789初始化
    {
        LCD_WR_REG(0x11);

        delay_ms(120);

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);


        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0X05);

        LCD_WR_REG(0xB2);
        LCD_WR_DATA(0x0C);
        LCD_WR_DATA(0x0C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x33);
        LCD_WR_DATA(0x33);

        LCD_WR_REG(0xB7);
        LCD_WR_DATA(0x35);

        LCD_WR_REG(0xBB);       //vcom
        LCD_WR_DATA(0x32);      //30

        LCD_WR_REG(0xC0);
        LCD_WR_DATA(0x0C);

        LCD_WR_REG(0xC2);
        LCD_WR_DATA(0x01);

        LCD_WR_REG(0xC3);       //vrh
        LCD_WR_DATA(0x10);      //17 0D

        LCD_WR_REG(0xC4);       //vdv
        LCD_WR_DATA(0x20);      //20

        LCD_WR_REG(0xC6);
        LCD_WR_DATA(0x0f);

        LCD_WR_REG(0xD0);
        LCD_WR_DATA(0xA4);
        LCD_WR_DATA(0xA1);

        LCD_WR_REG(0xE0);       //Set Gamma
        LCD_WR_DATA(0xd0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0a);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0X44);
        LCD_WR_DATA(0x42);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0e);
        LCD_WR_DATA(0x12);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x17);


        LCD_WR_REG(0XE1);       //Set Gamma
        LCD_WR_DATA(0xd0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0a);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x31);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0x47);
        LCD_WR_DATA(0x0e);
        LCD_WR_DATA(0x1c);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x1b);
        LCD_WR_DATA(0x1e);


        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xef);

        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);

        LCD_WR_REG(0x29);       //display on
    }
    
    LCD_Display_Dir(0);         //默认为竖屏
	//ILI9341_GramScan(6);
    LCD_LED = 0;                //点亮背光
    LCD_Clear(WHITE);
}  
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height;    //得到总点数

    LCD_SetCursor(0x00, 0x0000);    //设置光标位置
    LCD_WriteRAM_Prepare();         //开始写入GRAM

    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标
//区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);       //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;   //设置光标位置
        }
    }
}  

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
    width = ex - sx + 1;            //得到填充的宽度
    height = ey - sy + 1;           //高度

    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);  //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM

        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM=color[i * width + j];  //写入数据
        }
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;              //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1;       //设置单步方向
    else if (delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    //画线输出
    {
        LCD_DrawPoint(uRow, uCol); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //判断下个点位置的标志

    while (a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);        //5
        LCD_DrawPoint(x0 + b, y0 - a);        //0
        LCD_DrawPoint(x0 + b, y0 + a);        //4
        LCD_DrawPoint(x0 + a, y0 + b);        //6
        LCD_DrawPoint(x0 - a, y0 + b);        //1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b);        //2
        LCD_DrawPoint(x0 - b, y0 - a);        //7
        a++;

        //使用Bresenham算法画圆
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);  //得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';    //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t];        //调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];   //调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];   //调用2412字体
        else return;                                    //没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);

            temp <<= 1;
            y++;

            if (y >= lcddev.height)return;      //超区域了

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)return;   //超区域了

                break;
            }
        }
    }
}

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;

    while (n--)result *= m;

    return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
}

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);

                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
































