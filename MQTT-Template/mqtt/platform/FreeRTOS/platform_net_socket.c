/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime: 2020-04-25 17:50:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_net_socket.h"
#include "at_commend.h"

#define TEST_SSID "TP-LINK_5G_2B18"
#define TEST_PASSWD "13838106970"

//����С��0��ʾʧ��
//������IP�Ͷ˿�(��������), ���һ����ʾʹ�õĶ�Э��
//PLATFORM_NET_PROTO_TCP  0 /**< The TCP transport protocol */
//PLATFORM_NET_PROTO_UDP  1 /**< The UDP transport protocol */
int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	int err;
	char cmd[50];
	//����WIFIģʽ
	err = ATSendCmd("AT+CWMODE=3", NULL, 0, 2000);
	if(err){
		printf("AT+CWMODE=3 ERR = %d\r\n", err);
		return err;
	}
	//�ȶϿ���������
	err = ATSendCmd("AT+CWQAP", NULL, 0, 5000);
	if(err){
		printf("DisConnect WIFI ERR = %d\r\n", err);
		return err;
	}
	//����·����
	err = ATSendCmd("AT+CWJAP=\"" TEST_SSID "\",\"" TEST_PASSWD "\"", NULL, 0, 20000);
	if(err){
		printf("AT+CWJAP=\"" TEST_SSID "\",\"" TEST_PASSWD "\"");
		printf("Connect WIFI ERR = %d\r\n", err);
		return err;
	}	
	//���ӷ�����
	if(proto == PLATFORM_NET_PROTO_TCP){
		//ʹ��TCPЭ�����ӷ�����
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", host, port);
	}else
	{
		//ʹ��UDPЭ��
		sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%s", host, port);
	}
	err = ATSendCmd(cmd, NULL, 0, 2000);
	if(err){
		printf("Connect HOST ERR = %d\r\n", err);
		return err;
	}	
	//
    return 0;
}

//���صĽ���ǵõ������ݵ�����
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	//��ȡ����, ʧ�ܾ�����
	int i=0;
	int err;
	while(i<len)
	{
		err = ATReadData((char *)&buf[i], timeout);
		if(err)
		{
			//��ȡʧ��
			return i;
		}
		i++;
	}
    return i;
}


int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	int err;
	char cmd[20];
	sprintf(cmd, "AT+CIPSEND=%d", len);
	err = ATSendCmd(cmd, NULL, 0, 2000);
	if(err){
		printf("Send DATACmd ERR = %d\r\n", err);
		return err;
	}
	err = ATSendData((char *)buf, len, 5000);
	if(err){
		printf("Send DATA ERR= %d\r\n", err);
		return err;
	}
    return len;
}

int platform_net_socket_close(int fd)
{
	int err;
	//����WIFIģʽ
	err = ATSendCmd("AT+CIPCLOSE", NULL, 0, 2000);
	if(err){
		printf("SOCKET CLOSE ERR = %d\r\n", err);
		return err;
	}
    return err;
}



