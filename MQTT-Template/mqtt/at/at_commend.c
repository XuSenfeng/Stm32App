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
//����AT�����ִ��״̬
static int g_at_status;
//��¼AT�����ֵ
static char g_at_resp[AT_RESP_LEN];
//��¼��ȡ���İ�
ring_buffer packet_buffer;
//��¼��ǰ������
static char *g_cur_cmd;

/*״̬
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
//����AT�����һ��������
static platform_mutex_t at_ret_mutex;
//һ������������
static platform_mutex_t at_packets_mutex;
void ATInit(void)
{
	//��ʼ��������
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
		//�жϷ���ֵ,�洢respond
		ret = GetATStatus();
		return ret;
	}
	return AT_TIMEOUT;
	
}

//����ĳһ����������: buf = "AT+CIPMODE=1"
//len = strlen(buf)
//timeout:ms
int ATSendCmd(char *buf, char *resp, int resp_len, int timeout)
{
	int ret;
	int err;
	//char temp_c;
	//��������
	//while(HAL_AT_Secv(&temp_c, 0)==1);
	//��¼��ǰ����
	g_cur_cmd = buf;
	//��������
	HAL_AT_Send(buf, strlen(buf));
	HAL_AT_Send("\r\n", 2);
	//�ȴ����,�ɹ�����1
	ret = platform_mutex_lock_timeout(&at_ret_mutex, timeout);
	if(ret)
	{
		//�жϷ���ֵ,�洢respond
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

//����Ƿ��ȡ�������ַ�
int GetSpecialATString(char *buf)
{
	if(strstr(buf, "+IPD,"))
		return 1;

	return 0;
}
//���������ַ�
void ProcessSpecialATString(void)
{
	int i = 0;
	int len = 0;
	char temp;
	/* +IPD,n:xxxxxxxxx   */
	while(1)
	{
		HAL_AT_Secv(&temp, (int)portMAX_DELAY);
		//��ȡ�����ַ�
		if(temp!=':')
		{
			//��ȡ���ݳ���
			len*=10;
			len+=temp - '0';
		}else
			break;
	}

	//��ȡ����������, ������д�뻺����ͬʱ���н�������
	for(i=0;i<len;i++){
		HAL_AT_Secv(&temp, (int)portMAX_DELAY);
		ring_buffer_write(temp, &packet_buffer);
		//����
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
			//��ʱ����0, �ɹ�����1, ����ѭ��
		}
	}while(ret);
	return AT_TIMEOUT;
}
//һ���������������
void ATRetParser(void const * argument)
{
	char buf[AT_RESP_LEN];
	int i = 0;
	while(1)
	{
		
		
		//��ȡWIFIģ�鷢����������
		HAL_AT_Secv(&buf[i], (int)portMAX_DELAY);
		//���н���
		/*1. ʲôʱ����� */
		
		if(i && buf[i-1] == '\r' && buf[i] == '\n')
		{
			//1.1 �յ��س�����
			/*2. ��ô���� */
			buf[i+1] = 0;
			if(strstr(buf, "OK"))
			{
				//���յ��ɹ���־
				SetATStatus(AT_OK);
				//��¼����
				memcpy(g_at_resp, buf, i+1);
				if(g_cur_cmd && strstr(g_cur_cmd, "AT+CIPSEND=")){
					//����Ƿ������ݵĵ�һ�η���OK��ʱ���Ȳ��ſ�
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
			//���շ�������ķ���ֵ
			SetATStatus(AT_OK);
			i=-1;
			//��¼����
			platform_mutex_unlock(&at_ret_mutex);
		}else if(GetSpecialATString(buf))
		{
			//1.2 �յ������ַ�(��Ҫ�ǽ��յ�������������������)
			
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
//topic1������
static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}


//����ʹ�õ�����
void Task_AClient_Test(void const * argument)
{
	//�ȴ�ESP8266��ʼ��
	while(ESP8266_FLOG==0);
	mqtt_client_t *client = NULL;
	mqtt_log_init();
	client = mqtt_lease();
	//���ö˿ں�
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


