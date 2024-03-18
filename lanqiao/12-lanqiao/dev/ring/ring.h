#ifndef RING_H
#define RING_H
#include "main.h"

typedef struct _ring_t{
    char buf[100];
    int head;//��¼��һ����ȡ��λ��
    int tail;//��¼��һ��д���λ��
	int size;
}ring_t;

int ring_buf_read(ring_t *ring, uint8_t *buf, int len);
int ring_buf_write(ring_t *ring, uint8_t *buf, int len);

#endif
