#include "jiao_app.h"
struct FIFO8 app_fifo;
unsigned char app_fifo_buf[100];
//�ⲿ�ľ���
extern char * jiao_strs[50];
char str_num = 0, str_time = SHOW_STR_TIME-1;	//����Ϊ��һ�ڵ�һ��ѭ����һ�����������ʾ��һ������

extern struct FIFO8 app_fifo;
int temp_out = ETMP_OUT;
//��ʾ��Ϣ�洢λ��
char data_string[100], temp_str[100], time_str[100], CPU_str[100], Wifi_str[100];
extern uint16_t lcdid;




//���ݳ�ʼ��
void jiao_App_Init(){
	//��ʼ��һ����Ϣ���ݵ�FIFO
	FIFO8_Init(&app_fifo, 100, app_fifo_buf);
}


//����ȡ������Ϣ���д���
void deal_data(char *datas){
	int i=0, j, tempture=0;
	char temp_ch;
	temp_out = ETMP_OUT;
	if(datas[i] == 'd'){
		//��������
		i++;
		sprintf(data_string,"����: ");
		for(j=0;j<4;j++){
			//������
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}
		sprintf(data_string,"%s%c",data_string, '-');
		for(j=0;j<2;j++){
			//����·�
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}
		sprintf(data_string,"%s%c",data_string, '-');
		for(j=0;j<2;j++){
			//�������
			temp_ch = datas[i];
			sprintf(data_string,"%s%c",data_string, temp_ch);
			i++;
		}			
	}
	if(datas[i] == 't'){
		//����¶�
		i++;
		sprintf(temp_str,"��ǰ�¶�T: ");
		while(datas[i] != 't'){
			tempture*=10;
			tempture += datas[i] - '0';
			sprintf(temp_str,"%s%c",temp_str, datas[i]);
			i++;
		}
		//����¶�
		sprintf(temp_str,"%s%s",temp_str, "C ");
		i++;
		//��ȡ����
		while(datas[i] != 't' && temp_out--){
			sprintf(temp_str,"%s%c",temp_str, datas[i]);
			i++;
		}
		if(!temp_out){
			return;
		}
		i++;
		if(tempture < 16)
			sprintf(temp_str,"%s%s",temp_str, " ע�Ᵽů");			
		else if(tempture>35)
			sprintf(temp_str,"%s%s",temp_str, " С������");
		else
			sprintf(temp_str,"%s%s",temp_str, " ��ÿ���");			
	}
	if(datas[i] == 'h'){
		i++;
		sprintf(time_str,"ʱ��  ");
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
		sprintf(CPU_str,"%s%% �ڴ� = ", CPU_str);
		i++;
		while(datas[i] != '|'){
			sprintf(CPU_str,"%s%c",CPU_str, datas[i]);
			i++;
		}		
	}
	i++;
	if(datas[i++] == 'W'){

		sprintf(Wifi_str,"Wifi ����= ");
		while(datas[i] != '|'){
			sprintf(Wifi_str,"%s%c",Wifi_str, datas[i]);
			i++;
		}
		sprintf(Wifi_str,"%skb/s ����= ", Wifi_str);
		i++;
		while(datas[i] != '|'){
			sprintf(Wifi_str,"%s%c",Wifi_str, datas[i]);
			i++;
		}
			sprintf(Wifi_str,"%skb/s",Wifi_str);		
	}
	data_string[99]=0, temp_str[99]=0, time_str[99]=0, CPU_str[99]=0, Wifi_str[99]=0;
}
//��ʼ��ʱ����ʾ��һЩ��Ϣ
void LCD_Show_Init(void)
{
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�x */
	/********��ʾ�ַ���ʾ��*******/
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
  
	/********��ʼ��һ����ʾ���ַ���ʽ*******/
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(GREEN);
}
char datas[100] = {0};

//ʵ��ѭ����ʾ������
void LCD_Test(void)
{
	int i;
	LCD_SetTextColor(GREEN);
	//����Է������ݴӶ���ȡ��Ϣ
	while(!(FIFO8_Status(&app_fifo))){
			printf("1");
			for(i=0;i<0xfffff;i++);
	}
	i=0;
	//��Ϣ�Ѿ��յ���
	while(FIFO8_Status(&app_fifo))
	{
		datas[i] = FIFO8_Get(&app_fifo);
		i++;
	}
	//��ȡ���ڵ�����
	deal_data(datas);
	//��ʾ��ǰ������
    LCD_ClearLine(LINE(5));
	ILI9341_DispStringLine_EN_CH(LINE(5),data_string);
	//��ʾ��ǰ������
    LCD_ClearLine(LINE(7));
	ILI9341_DispStringLine_EN_CH(LINE(7),temp_str);
	//��ʾ��ǰʱ��
    LCD_ClearLine(LINE(9));	
	ILI9341_DispStringLine_EN_CH(LINE(9),time_str);
	//��ʾCPU״̬
    LCD_ClearLine(LINE(11));
	ILI9341_DispStringLine_EN_CH(LINE(11),CPU_str); 
	//��ʾ��������
    LCD_ClearLine(LINE(13));
	ILI9341_DispStringLine_EN_CH(LINE(13),Wifi_str); 

	//һ��ʱ���Ժ����и��Ķ��·���ʾ������
	str_time++;
	if(str_time == SHOW_STR_TIME){
		LCD_ClearLine(LINE(15));	/* ����������� */
		LCD_ClearLine(LINE(16));
		LCD_ClearLine(LINE(17));
		LCD_ClearLine(LINE(18));
		LCD_ClearLine(LINE(19));
		LCD_SetTextColor(YELLOW);
		/*Ȼ����ʾ���ַ������ɣ���������Ҳ���������q*/
		ILI9341_DispStringLine_EN_CH(LINE(15),jiao_strs[str_num++]);
		str_time=0;
	}
	//��ʾ�����ֽ���һ��ѭ��
	if(str_num==MAX_STR_SHOW)
		str_num=0;
    HAL_Delay(700);

}


