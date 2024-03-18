# ShowWindowsMessage

![QQ图片20240125175403](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401251754576.jpg)

通过与Windows连接获取一些实时的信息显示在屏幕上面

基于野火开发板, 使用需要Flash里面保存的字模

使用的时候保证开压板正常连接之后调用, 注意不要使用代理, 默认使用CM3, 执行的文件是jiao_serial.py, 可以在里面进行更改, 需要联网保证正常使用

## 文件

+ serialpyapp: 包含python写的上位机程序
+ + jiao_getCpu.py获取CPU的状态
    + jiao_getWeather.py获取今天的天气, 使用ip地址进行定位
    + jiao_getWifi.py获取当前的网络信息
    + jiao_serial.py对获取的信息进行传输通过发送到单片机然后进行显示
+ DataStructure: 数据类型
+ + jiao_FIFO.h实现FIFO
+ Driver: 一些驱动文件
+ + CMSIS内核相关
    + flash: 主要用来获取SPIFlash里面的字模数据
    + lcd: 屏幕的驱动
    + HAL库相关问阿金
    + usart串口相关文件
+ User: 对应的单片机app实现
+ + jiao_app.c实现对串口发送信息的处理
    + jiao_text.c显示在屏幕上的数据储存文件

## 实现思路

使用python通过API获取今天的天气信息, 使用库读取当前电脑的运行状态, 以及网络信息, 使用一定的数据格式通过串口发送给单片机

```
d20231023t11t霾th212839C4.6|41.6|W0.00|0.00|
```

> d日期t温度t天气th时间CCPU占用|内存占用|W网络上行|网络下行速度|
