import serial#导入串口通信库
from time import sleep
import jiao_getWeather
import datetime
import jiao_getCpu
import jiao_getWifi

ser = serial.Serial()

def port_open_recv():#对串口的参数进行配置
    ser.port='com7'
    ser.baudrate=115200
    ser.bytesize=8
    ser.stopbits=1
    ser.parity="N"#奇偶校验位
    ser.open()
    if(ser.isOpen()):
        print("串口打开成功！")
    else:
        print("串口打开失败！")
#isOpen()函数来查看串口的开闭状态


i=0
def port_close():
    ser.close()
    if(ser.isOpen()):
        print("串口关闭失败！")
    else:
        print("串口关闭成功！")

def send(send_data):
    if(ser.isOpen()):
        ser.write(send_data.encode('gbk'))#编码
        print("发送成功",send_data)
    else:
        print("发送失败！")
def get():
    if(ser.isOpen()):
        str1 = str(ser.read())
        # print(str1)
        return str1
    else:
        return None

if __name__ == '__main__':
    port_open_recv()
    data = jiao_getWeather.get_serial_data()
    while True:
        if(get() == "b'1'"):
            a = data + datetime.datetime.now().strftime('h%H%M%S') + 'C' + jiao_getCpu.get_cpu_data() + '|' + 'W' + jiao_getWifi.getNet()
            send(a)
            i+=1
            if(1%1000 == 0):
                i=0
                data = jiao_getWeather.get_serial_data()

        else:
            sleep(0.1)


