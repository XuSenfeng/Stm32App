#ifndef __AT_COMMEND
#define __AT_COMMEND
#define AT_OK			0
#define AT_ERR			-1
#define AT_TIMEOUT		-2
void ATInit(void);
void Task_AClient_Test(void const * argument);
void ATRetParser(void const * argument);
int ATSendCmd(char *buf, char *resp, int resp_len, int timeout);
int ATSendData(char *buf,int resp_len, int timeout);
int ATReadData(char * c, int timeout);
void UART2RetParser(void const * argument);

#endif /*__AT_COMMEND*/
