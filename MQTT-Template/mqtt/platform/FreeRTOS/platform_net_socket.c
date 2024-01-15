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

//返回小于0表示失败
//参数是IP和端口(服务器的), 最后一个表示使用的额协议
//PLATFORM_NET_PROTO_TCP  0 /**< The TCP transport protocol */
//PLATFORM_NET_PROTO_UDP  1 /**< The UDP transport protocol */
int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	int err;
	char cmd[50];
	//配置WIFI模式
	err = ATSendCmd("AT+CWMODE=3", NULL, 0, 2000);
	if(err){
		printf("AT+CWMODE=3 ERR = %d\r\n", err);
		return err;
	}
	//先断开网络连接
	err = ATSendCmd("AT+CWQAP", NULL, 0, 5000);
	if(err){
		printf("DisConnect WIFI ERR = %d\r\n", err);
		return err;
	}
	//连接路由器
	err = ATSendCmd("AT+CWJAP=\"" TEST_SSID "\",\"" TEST_PASSWD "\"", NULL, 0, 20000);
	if(err){
		printf("AT+CWJAP=\"" TEST_SSID "\",\"" TEST_PASSWD "\"");
		printf("Connect WIFI ERR = %d\r\n", err);
		return err;
	}	
	//连接服务器
	if(proto == PLATFORM_NET_PROTO_TCP){
		//使用TCP协议连接服务器
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", host, port);
	}else
	{
		//使用UDP协议
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

//返回的结果是得到的数据的数量
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	//读取数据, 失败就阻塞
	int i=0;
	int err;
	while(i<len)
	{
		err = ATReadData((char *)&buf[i], timeout);
		if(err)
		{
			//读取失败
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
	//配置WIFI模式
	err = ATSendCmd("AT+CIPCLOSE", NULL, 0, 2000);
	if(err){
		printf("SOCKET CLOSE ERR = %d\r\n", err);
		return err;
	}
    return err;
}



