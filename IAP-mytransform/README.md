# IAP程序

![image-20240126160329355](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401261603445.png)

由于每一次下载的时候都需要从电脑下载, 很不方便, 所以就写了一个程序用来从SD卡读取数据

在开机的时候按住KEY2会进入程序的选择界面, 会从SD卡里面获取到所有的后缀名为.bin的文件(这一种的文件在定义以后可以直接执行)

> **默认的不支持中文, 使用的话需要配置一下FatFS(详细的使用可以看一下我的笔记里面的FatFS)**

在按照提示显示正确的程序以后按KEY1进入程序执行

在程序使用的时候需要注意IAP会使用到一系列外设, 在重新初识化程序的时候最好删除一下之前的设置

IAP程序设置的大小的64KB(有预留位置用于更新)

Fatfs在使用的时候使用的正点原子移植的Fatfs, 所以把一部分的编码文件写入片上的Flash里面, 在4MB的位置, 我会在文件夹里面放一份我移植的烧写程序, 具体的烧写看正点原子的教程

> 实际上就是汉字显示的那一部分代码

## 具体的可移植性代码需要的设置

![image-20240115191428551](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401151914629.png)

![image-20240115191445856](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401151914886.png)

> fromelf  --bin -o ..\OBJ\@L.bin ..\OBJ\\%L
>
> 这一句需要设置一下实际的Keil的输出的目录, 以及你希望.bin文件的输出的位置

```c
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}
int main(void)
{						
	sys_nvic_set_vector_table(FLASH_BASE, 0x10000);//设置中断表偏移

    	...
}
```

> 要在代码里加上这一句偏移

## 实现的思路

通过CubeMX配置一下SDIO驱动SD卡, 以及移植一下FatFS, (默认不支持中文), 在上电以后会检测一下KEY的状态, 如果是按下的, 递归扫描一下SD卡里面的文件, 如果有.bin文件, 显示在OLED显示屏上面, 检测到用户想要烧写以后会把它写入Flash的0x81000的位置(前面用于IAP程序了)

如果按键不是按下的话, 会直接进入Flash检测一下有没有可以执行的文件, 有的话直接执行

## 需要的支持

主要是正点原子的中文需要的支持, 他把中文字模以及FatFS的字符转换表写在了SPI Flash W25Q64里面, 具体的可以看他们的教程, 这里有一个我移植的适配野火开发板的程序, 会从SD卡里面获取到需要的文件, 写入SPI Flash的4M的位置
