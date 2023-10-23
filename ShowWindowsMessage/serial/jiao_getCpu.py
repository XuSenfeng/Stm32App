import psutil

def line(n): #输出制定个数的横线
    print("--"*n)

def get_cpu_data():
    # 获取CPU信息
    # print("CPU信息")
    # line(10)
    # print("CPU 使用率",psutil.cpu_percent(interval=1),"%")
    
    # line(10)

    # 获取内存信息
    # print("获取内存信息")
    # line(10)
    mem = psutil.virtual_memory()
    # print("使用内存占比:", mem.percent)
    str1 = str(psutil.cpu_percent(interval=1))
    str2 = str(mem.percent)
    return str1 + '|' +str2

if __name__ == '__main__':
    get_cpu_data()
