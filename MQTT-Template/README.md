## MQTT移植

基于野火指南者开发板

移植大佬的MQTT框架, 这个是代码网址, 学的是韦东山100ask的课程

[mqttclient](https://github.com/jiejieTop/mqttclient)

主要实现的是使用开发板上面的ESP8266连接MQTT服务器, 目前使用的是ip地址进行连接(FreeRTOS版本)

![image-20240115185851572](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401151858807.png)

测试程序在mqtt/at/at_comment.c文件里面, 需要改成你自己的服务器地址, 测试程序的代码是不断的向topic1主题发送信息

我主要实现的就是AT命令的处理, 还没有进行加密之类的网络操作的测试(我还没学过)

目前收发程序跑了两天没有出现崩溃的情况

要连接的Wifi密码在E:\JHY\stm32\github\Stm32App\MQTT-Template\mqtt\platform\FreeRTOS\platform_net_socket.c文件里面