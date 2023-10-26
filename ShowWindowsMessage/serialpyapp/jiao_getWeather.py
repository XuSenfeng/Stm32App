import requests
from plyer import notification
import datetime
"""
plyer是用来显示弹窗的模块
https://blog.csdn.net/wangzirui32/article/details/116862589
安装命令：pip install plyer
"""
date_Y = datetime.datetime.now().strftime('%Y')
date_m = datetime.datetime.now().strftime('%m')
date_d = datetime.datetime.now().strftime('%d')
params = {
    "key": "S8Y08EJ9uxbeN4_f2",
    "location": "ip",    # 查询地点设置为访问IP所在地
    "language": "zh-Hans",
    "unit": "c",
}

url = "https://api.seniverse.com/v3/weather/now.json"

# 获取数据

          
"""
标题为“当前天气”
显示10秒钟（timeout参数）
"""
"""
notification.notify(title="当前天气",
                    message=message,
                    timeout=10)
"""


def get_serial_data_0():
    r = requests.get(url, params=params)

    # 解析数据
    data = r.json()["results"]

    address = data[0]["location"]['path']        # 地点
    temperature = data[0]['now']["temperature"]  # 温度
    text = data[0]['now']["text"]			     # 天气情况

    """ 弹窗显示消息
    message = address + " 当前天气：\n" + \
            "温度：" + temperature + "℃" + \
            "\n天气情况：" + text + \
            "\n祝您心情愉悦！(^o^)"
    """

    # 获取日期
    result_data = 'd' + date_Y + date_m + date_d
    # 获取温度
    result_data = result_data + 't' + temperature + 't'  + text
    result_data = result_data + 't'
    # print(result_data)
    # print(message)
    return result_data

def get_serial_data():
    try:
        return get_serial_data_0()
    except:
        return "d20231022t14t7t"

# d20231022t14t7t


if __name__ == '__main__':
    print("hello")