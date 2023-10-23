import psutil
import time

def getNet():
    sent_before = psutil.net_io_counters().bytes_sent  # 已发送的流量
    recv_before = psutil.net_io_counters().bytes_recv  # 已接收的流量
    sent_now = psutil.net_io_counters().bytes_sent
    recv_now = psutil.net_io_counters().bytes_recv
    sent = (sent_now - sent_before)/1024  # 算出1秒后的差值
    recv = (recv_now - recv_before)/1024
    # print(time.strftime(" [%Y-%m-%d %H:%M:%S] ", time.localtime()))
    str1 = ("{0}".format("%.2f"%sent))
    str2 = ("{0}".format("%.2f"%recv))
    
    return_result = str1 +'|' + str2 + '|'
    return return_result


if __name__ == "__main__":
    print('-'*32)
    while 1:
        time.sleep(1)
        getNet()
