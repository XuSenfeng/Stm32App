#ifndef RING_H
#define RING_H
#include "main.h"

typedef struct _ring_t{
    char buf[100];
    int head;//记录下一个读取的位置
    int tail;//记录下一个写入的位置
	int size;
}ring_t;

int ring_buf_read(ring_t *ring, uint8_t *buf, int len);
int ring_buf_write(ring_t *ring, uint8_t *buf, int len);

#endif
