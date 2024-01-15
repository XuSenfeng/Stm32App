#include "at_commend.h"
#include "at_uart_hal.h"
#include "platform_mutex.h"
#include "stdio.h"
#include "string.h"
#include "task.h"
#include "ring_buffer.h"
#include "mqttclient.h"


#define AT_CMD_TIME_OUT	1000
#define AT_RESP_LEN	100
//保存AT命令的执行状态
static int g_at_status;
//记录AT命令返回值
static char g_at_resp[AT_RESP_LEN];
//记录收取到的包
ring_buffer packet_buffer;
//记录当前的命令
static char *g_cur_cmd;

/*状态
* 0  - ok
* -1 - error
* -2 - timeout
*/
void SetATStatus(int status)
{
	g_at_status = status;
}
int GetATStatus(void)
{
	return g_at_status;
}
//用于AT命令的一个互斥锁
static platform_mutex_t at_ret_mutex;
//一个解析包的锁
static platform_mutex_t at_packets_mutex;
void ATInit(void)
{
	//初始化互斥量
	platform_mutex_init(&at_ret_mutex);
	platform_mutex_lock(&at_ret_mutex);
	platform_mutex_init(&at_packets_mutex);
	platform_mutex_lock(&at_packets_mutex);
	ring_buffer_init(&packet_buffer);
}
int ATSendData(char *buf,int resp_len, int timeout){
	int ret;
	HAL_AT_Send(buf, resp_len);
	ret = platform_mutex_lock_timeout(&at_ret_mutex, timeout);
	if(ret)
	{
		//判断返回值,存储respond
		ret = GetATStatus();
		return ret;
	}
	return AT_TIMEOUT;
	
}

//发送某一个命令例如: buf = "AT+CIPMODE=1"
//len = strlen(buf)
//timeout:ms
int ATSendCmd(char *buf, char *resp, int resp_len, int timeout)
{
	int ret;
	int err;
	//char temp_c;
	//发送命令
	//while(HAL_AT_Secv(&temp_c, 0)==1);
	//记录当前命令
	g_cur_cmd = buf;
	//发送命令
	HAL_AT_Send(buf, strlen(buf));
	HAL_AT_Send("\r\n", 2);
	//等待结果,成功返回1
	ret = platform_mutex_lock_timeout(&at_ret_mutex, timeout);
	if(ret)
	{
		//判断返回值,存储respond
		err = GetATStatus();
		if(!err && resp)
		{
			memcpy(resp, g_at_resp, resp_len>AT_RESP_LEN?AT_RESP_LEN:resp_len);
		}
		return err;
	}
	return AT_TIMEOUT;
	
}

int GetCIPSENDResult(char *buf){
	if(g_cur_cmd && strstr(g_cur_cmd, "AT+CIPSEND=") && (buf[0]=='>'))
		return 1;
	return 0;
}

//检测是否获取到特殊字符
int GetSpecialATString(char *buf)
{
	if(strstr(buf, "+IPD,"))
		return 1;

	return 0;
}
//处理特殊字符
void ProcessSpecialATString(void)
{
	int i = 0;
	int len = 0;
	char temp;
	/* +IPD,n:xxxxxxxxx   */
	while(1)
	{
		HAL_AT_Secv(&temp, (int)portMAX_DELAY);
		//读取后续字符
		if(temp!=':')
		{
			//获取数据长度
			len*=10;
			len+=temp - '0';
		}else
			break;
	}

	//获取真正的数据, 把数据写入缓冲区同时进行解锁唤醒
	for(i=0;i<len;i++){
		HAL_AT_Secv(&temp, (int)portMAX_DELAY);
		ring_buffer_write(temp, &packet_buffer);
		//解锁
		platform_mutex_unlock(&at_packets_mutex);
	}
}

int ATReadData(char * c, int timeout)
{
	int ret;
	do{
		
		if(ring_buffer_read((unsigned char *)c, &packet_buffer)==0)
		{
			return AT_OK;
		}else{
			ret = platform_mutex_lock_timeout(&at_packets_mutex, timeout);
			//超时返回0, 成功返回1, 继续循环
		}
	}while(ret);
	return AT_TIMEOUT;
}
//一个解析命令的任务
void ATRetParser(void const * argument)
{
	char buf[AT_RESP_LEN];
	int i = 0;
	while(1)
	{
		
		
		//获取WIFI模块发送来的命令
		HAL_AT_Secv(&buf[i], (int)portMAX_DELAY);
		//进行解析
		/*1. 什么时候解析 */
		
		if(i && buf[i-1] == '\r' && buf[i] == '\n')
		{
			//1.1 收到回车换行
			/*2. 怎么解析 */
			buf[i+1] = 0;
			if(strstr(buf, "OK"))
			{
				//接收到成功标志
				SetATStatus(AT_OK);
				//记录数据
				memcpy(g_at_resp, buf, i+1);
				if(g_cur_cmd && strstr(g_cur_cmd, "AT+CIPSEND=")){
					//如果是发送数据的第一次返回OK这时候先不放开
					if(strstr(buf, "SEND OK")){
						platform_mutex_unlock(&at_ret_mutex);
					}
				}else
					platform_mutex_unlock(&at_ret_mutex);
				
			}else if(strstr(buf, "ERROR"))
			{
				SetATStatus(AT_ERR);
				platform_mutex_unlock(&at_ret_mutex);
			}
			i = -1;
			
		}else if(GetCIPSENDResult(buf))
		{
			//接收发送命令的返回值
			SetATStatus(AT_OK);
			i=-1;
			//记录数据
			platform_mutex_unlock(&at_ret_mutex);
		}else if(GetSpecialATString(buf))
		{
			//1.2 收到特殊字符(主要是接收到服务器发过来的数据)
			
			ProcessSpecialATString();
			memset(&buf, 0, sizeof(buf));
			i = -1;
		}
		i++;
		if(i==AT_RESP_LEN)
		{
			i = AT_RESP_LEN-1;
		}
	}
}


extern volatile uint8_t ESP8266_FLOG;
//topic1处理函数
static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}


//测试使用的任务
void Task_AClient_Test(void const * argument)
{
	//等待ESP8266初始化
	while(ESP8266_FLOG==0);
	mqtt_client_t *client = NULL;
	mqtt_log_init();
	client = mqtt_lease();
	//设置端口号
	mqtt_set_port(client, "1883");
	mqtt_set_host(client, "Your ip");
	mqtt_set_client_id(client, random_string(10));
	mqtt_set_user_name(client, random_string(10));
	mqtt_set_password(client, random_string(10));
	
	if(mqtt_connect(client) != 0)
	{
		printf("Connot connect\r\n");
		vTaskDelete(NULL);
	}
	mqtt_subscribe(client, "topic1", QOS0, topic1_handler);
	mqtt_subscribe(client, "topic2", QOS1, NULL);
	mqtt_subscribe(client, "topic3", QOS2, NULL);

	char buf[30], i=0;
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.payload = buf;
    msg.qos = QOS0;
	msg.payloadlen = strlen(msg.payload);
	while(1)
	{
		sprintf(buf, "jiao test%d", i++);
        mqtt_publish(client, "jhytest", &msg);
		vTaskDelay(10000);
	}
}


