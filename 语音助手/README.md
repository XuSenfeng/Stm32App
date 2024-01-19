# 语音助手

使用的是ASRPRO语音开发板和野火的指南者开发板

语音助手连接开发板的USART2

电脑运行main.py, 连接同一个MQTT服务器即可(需要修改main.py(IP地址)和platform_net_socket.c(WIFI密码)和at_commend.c(ip地址))

之后电脑运行main.py

默认下载使用我的那个API进行加载, 不使用的话要改这一个选项

![image-20240119172523726](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401191725946.png)

> 改成0x80000000和0x80000









