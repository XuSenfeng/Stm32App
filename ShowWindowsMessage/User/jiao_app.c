#include "jiao_app.h"
struct FIFO8 app_fifo;
unsigned char app_fifo_buf[100];
//外部的句子
extern char * jiao_strs[50];
char str_num = 0, str_time = SHOW_STR_TIME-1;	//设置为减一在第一次循环加一后可以正常显示第一行内容

extern struct FIFO8 app_fifo;
int temp_out = ETMP_OUT;
//显示信息存储位置
char data_string[100], temp_str[100], time_str[100], CPU_str[100], Wifi_str[100];
extern uint16_t lcdid;




//数据初始化
void jiao_App_Init(){
	//初始化一个信息传递的FIFO
	FIFO8_Init(&app_fifo, 100, app_fifo_buf);
}


//对收取到的信息进行处理
void deal_data(char *datas){
	int i=0, j, tempture=0;
	char temp_ch;
	temp_out = ETMP_OUT;
	if(datas[i] == 'd'){
		//传入日期
		i++;
		sprintf(data_string,"日期: ");
		for(j=0;j<4;j++){
			//添加年份
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}
		sprintf(data_string,"%s%c",data_string, '-');
		for(j=0;j<2;j++){
			//添加月份
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}
		sprintf(data_string,"%s%c",data_string, '-');
		for(j=0;j<2;j++){
			//添加日期
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}			
	}
	if(datas[i] == 't'){
		//最高温度
		i++;
		sprintf(temp_str,"当前温度T: ");
		while(datas[i] != 't'){
			tempture*=10;
			tempture += datas[i] - '0';
			sprintf(temp_str,"%s%c",temp_str, datas[i]);
			i++;
		}
		//最低温度
		sprintf(temp_str,"%s%s",temp_str, "C ");
		i++;
		//获取天气
		while(datas[i] != 't' && temp_out--){
			sprintf(temp_str,"%s%c",temp_str, datas[i]);
			i++;
		}
		if(!temp_out){
			return;
		}
		i++;
		if(tempture < 16)
			sprintf(temp_str,"%s%s",temp_str, " 注意保暖");			
		else if(tempture>35)
			sprintf(temp_str,"%s%s",temp_str, " 小心中暑");
		else
			sprintf(temp_str,"%s%s",temp_str, " 玩得开心");			
	}
	if(datas[i] == 'h'){
		i++;
		sprintf(time_str,"时间  ");
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
		sprintf(time_str,"%s%c",time_str, ':');
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
		sprintf(time_str,"%s%c",time_str, ':');
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
		temp_ch = datas[i++];
		sprintf(time_str,"%s%c",time_str, temp_ch);
	}
	
	if(datas[i++] == 'C'){

		sprintf(CPU_str,"CPU = ");
		while(datas[i] != '|'){
			sprintf(CPU_str,"%s%c",CPU_str, datas[i]);
			i++;
		}
		sprintf(CPU_str,"%s%% 内存 = ", CPU_str);
		i++;
		while(datas[i] != '|'){
			sprintf(CPU_str,"%s%c",CPU_str, datas[i]);
			i++;
		}		
	}
	i++;
	if(datas[i++] == 'W'){

		sprintf(Wifi_str,"Wifi 上行= ");
		while(datas[i] != '|'){
			sprintf(Wifi_str,"%s%c",Wifi_str, datas[i]);
			i++;
		}
		sprintf(Wifi_str,"%skb/s 下行= ", Wifi_str);
		i++;
		while(datas[i] != '|'){
			sprintf(Wifi_str,"%s%c",Wifi_str, datas[i]);
			i++;
		}
			sprintf(Wifi_str,"%skb/s",Wifi_str);		
	}
	data_string[99]=0, temp_str[99]=0, time_str[99]=0, CPU_str[99]=0, Wifi_str[99]=0;
}
//初始的时候显示的一些信息
void LCD_Show_Init(void)
{
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黿 */
	/********显示字符串示便*******/
    ILI9341_DispStringLine_EN(LINE(0),"******************************");
	ILI9341_DispStringLine_EN_CH(LINE(1),"JIAO-Show");
    ILI9341_DispStringLine_EN(LINE(2),"https://github.com/XuSenfeng/Stm32App");
    if(lcdid == LCDID_ILI9341)
    {
        ILI9341_DispStringLine_EN(LINE(4),"*******************************");
    }
    else if(lcdid == LCDID_ST7789V)
    {
        ILI9341_DispStringLine_EN(LINE(4),"******************************");
    }
    //ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
  
	/********初始化一下显示的字符格式*******/
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(GREEN);
}
char datas[100] = {0};

//实际循环显示的内容
void LCD_Test(void)
{
	int i;
	LCD_SetTextColor(GREEN);
	//向电脑发送数据从而获取信息
	while(!(FIFO8_Status(&app_fifo))){
			printf("1");
			for(i=0;i<0xfffff;i++);
	}
	i=0;
	//信息已经收到了
	while(FIFO8_Status(&app_fifo))
	{
		datas[i] = FIFO8_Get(&app_fifo);
		i++;
	}
	//获取串口的数据
	deal_data(datas);
	//显示当前的日期
    LCD_ClearLine(LINE(5));
	ILI9341_DispStringLine_EN_CH(LINE(5),data_string);
	//显示当前的天气
    LCD_ClearLine(LINE(7));
	ILI9341_DispStringLine_EN_CH(LINE(7),temp_str);
	//显示当前时间
    LCD_ClearLine(LINE(9));	
	ILI9341_DispStringLine_EN_CH(LINE(9),time_str);
	//显示CPU状态
    LCD_ClearLine(LINE(11));
	ILI9341_DispStringLine_EN_CH(LINE(11),CPU_str); 
	//显示网络速率
    LCD_ClearLine(LINE(13));
	ILI9341_DispStringLine_EN_CH(LINE(13),Wifi_str); 

	//一段时间以后会进行更改对下方显示的文字
	str_time++;
	if(str_time == SHOW_STR_TIME){
		LCD_ClearLine(LINE(15));	/* 清除单行文字 */
		LCD_ClearLine(LINE(16));
		LCD_ClearLine(LINE(17));
		LCD_ClearLine(LINE(18));
		LCD_ClearLine(LINE(19));
		LCD_SetTextColor(YELLOW);
		/*然后显示该字符串即可，其它变量也是这样处琿*/
		ILI9341_DispStringLine_EN_CH(LINE(15),jiao_strs[str_num++]);
		str_time=0;
	}
	//显示的文字进行一次循环
	if(str_num==MAX_STR_SHOW)
		str_num=0;
    HAL_Delay(700);

}


