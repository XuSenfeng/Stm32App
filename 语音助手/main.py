#coding=utf-8  
import json
import sys
import os
import time
import webbrowser
import paho.mqtt.client as mqtt
sys.path.append(os.path.abspath(os.path.dirname(__file__) + '/' + '..'))
sys.path.append("..")
 
REPORT_TOPIC = 'YSQD'  # 主�??
 
 
def on_connect(client, userdata, flags, rc):
    print('connected to mqtt with resurt code ', rc)
    client.subscribe(REPORT_TOPIC)  # 订阅主�??
 
 
def on_message(client, userdata, msg):
    """
    接收客户�?发送的消息
    :param client: 连接信息
    :param userdata: 
    :param msg: 客户�?返回的消�?
    :return: 
    """
    #payload = json.loads(msg.payload.decode('utf-8'))
    print(str(msg.payload))
    if str(msg.payload).find('YS') != -1:
        os.system('"D:\\游戏\\原神\\Genshin Impact\\launcher.exe"')
    elif str(msg.payload).find('STM32')!= -1:
        # �?音嵌入式开�?
        os.system("D:\\keil5-new\\UV4\\Uv4.exe")
    elif str(msg.payload).find('SHOUCE')!= -1:
        os.startfile("E:\\a学习\\1.stm\\A盘（资料盘）\\3-STM32官方资料\\STM32F1官方手册资料")
    elif str(msg.payload).find('PROJECT')!= -1:
        os.startfile("E:\\JHY")
    elif str(msg.payload).find('SERIAL')!= -1:
        os.startfile('"E:\\a学习\\16-Linux\\原子资料\\【正点原子】阿尔法Linux开发板（A盘）-基础资料\\【正点原子】阿尔法Linux开发板（A盘）-基础资料\\03、软件\\XCOM V2.0.exe"')
    elif str(msg.payload).find('MUSIC')!= -1:   
        os.startfile('"E:\\alearn\\QQmusic\\QQMusic2005.15.48.11\\QQMusic.exe"')
    elif str(msg.payload).find('QQ')!= -1:   
        os.startfile('"E:\\alearn\\QQ\\Bin\\QQScLauncher.exe"')
    elif str(msg.payload).find('WECHAT')!= -1:   
        os.startfile('"E:\\alearn\\weixin\\WeChat\\WeChat.exe"')
    elif str(msg.payload).find('GITHUB')!= -1:   
        webbrowser.open('https://www.github.com')
    elif str(msg.payload).find('GTA5')!= -1:   
        webbrowser.open('E:\\桌面\\Grand Theft Auto V')
    elif str(msg.payload).find('VSCODE')!= -1:   
        webbrowser.open('E:\\VScode\Microsoft VS Code\\Code.exe')
    elif str(msg.payload).find('MQTT')!= -1:   
        webbrowser.open('E:\\alearn\\MQTTX\MQTTX.exe') 
 
def server_conenet(client):
    client.on_connect = on_connect  # �?用�?�阅模式
    client.on_message = on_message  # 接收消息
    client.connect("110.41.167.152", 1883, 60)  # 链接
    # client.loop_start()   # �?start方式运�?�，需要启动一�?守护线程，�?�服务�??运�?�，否则会随主线程�?�亡
    client.loop_forever()   # �?forever方式阻�?�运行�?
 
 
def server_stop(client):
    client.loop_stop()  # 停�?�服务�??
    sys.exit(0)
 
 
def server_main():
    client_id = time.strftime('%Y%m%d%H%M%S', time.localtime(time.time()))
    client = mqtt.Client(client_id, transport='tcp')
    server_conenet(client)
 
 
if __name__ == '__main__':
    # �?动监�?
    server_main()