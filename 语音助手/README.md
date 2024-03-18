# stm32电脑语音助手

![image-20240126153639763](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261536839.png)

使用的是ASRPRO语音开发板和野火的指南者开发板

语音助手连接开发板的USART2

电脑运行main.py, 连接同一个MQTT服务器即可(需要修改main.py(IP地址)和platform_net_socket.c(WIFI密码)和at_commend.c(ip地址))

之后电脑运行main.py

默认下载使用我的那个API进行加载, 不使用的话要改这一个选项

![image-20240119172523726](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401191725946.png)

> 改成0x80000000和0x80000

## 主要的实现的思路

### 和语音模块的通信

通过语音模块识别语音以后会发给stm32通过串口2

![image-20240126152522120](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261525177.png)

stm32通过FreeRTSO会创建一个任务UART2RetParser, 这一个任务会读取USART2中断中获取到的信息, 在每一个换行以后进行一次处理

平时这一个函数处于阻塞的状态通过一个二值信号量, USART2_IRQHandler里面会把获取到的数据写入到一个环形缓冲区里面, 然后唤醒这一个任务

在分析的时候如果是操控这一个板子上的命令直接执行, 否则通过WIFI模块进行发送给MQTT的YSQD订阅

### WIFI模块

会创建一个创建MQTT服务的任务Task_AClient_Test, 一个等待MQTT信息的任务(移植的框架里面的),  一个用于解释AT命令的任务ATRetParser, 主要的任务和模版里面一样

## 电脑的任务

获取MQTT的YSQD订阅, 然后解析, 通过python进行剩下的操作, 主要是打开对应的文件

## 连接

把语音模块的串口和stm32的USART2连接, 语音模块的麦克风以及音响按照说明进行连接, 

![image-20240126155843269](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261558396.png)

![image-20240126155928568](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261559642.png)

![image-20240126160008684](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261600707.png)
