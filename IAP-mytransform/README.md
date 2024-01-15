# IAP程序

由于每一次下载的时候都需要从电脑下载, 很不方便, 所以就写了一个程序用来从SD卡读取数据

在开机的时候按住KEY2会进入程序的选择界面, 会从SD卡里面获取到所有的后缀名为.bin的文件

在按照提示显示正确的程序以后按KEY1进入程序执行

在程序使用的时候需要注意IAP会使用到一系列外设, 在重新初识化程序的时候最好删除一下之前的设置

IAP程序设置的大小的64KB(有预留位置用于更新)

Fatfs在使用的时候使用的正点原子移植的Fatfs, 所以把一部分的编码文件写入片上的Flash里面, 在4MB的位置, 我会在文件夹里面放一份我移植的烧写程序, 具体的烧写看正点原子的教程

> 实际上就是汉字显示的那一部分代码

## 具体的可移植性代码需要的设置

![image-20240115191428551](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401151914629.png)

![image-20240115191445856](https://picture-01-1316374204.cos.ap-beijing.myqcloud.com/image/202401151914886.png)

> fromelf  --bin -o ..\OBJ\@L.bin ..\OBJ\\%L

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

