#include "ring/ring.h"

int ring_buf_read(ring_t *ring, uint8_t *buf, int len){
    int i;
    for(i = 0; i < len; i++){
        if(ring->size == 0){
            return i;
        }
        buf[i] = ring->buf[ring->tail];
        ring->tail++;
		ring->size--;
        if(ring->tail >= 100){
            ring->tail = 0;
        }
    }
    return i;
}

int ring_buf_write(ring_t *ring, uint8_t *buf, int len){
    int i;
    for(i = 0; i < len; i++){
		if(ring->size >= 100 ){
			return i;
		}
        ring->buf[ring->head] = buf[i];
        ring->head++;
		ring->size ++;

        if(ring->head >= 100){
            ring->head = 0;
        }
		
    }
    return i;
}
