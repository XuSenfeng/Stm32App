#ifndef KEY_H
#define KEY_H
#include "main.h"

#define KEY1_PRESS  (1<<0)
#define KEY2_PRESS  (1<<1)
#define KEY3_PRESS  (1<<2)
#define KEY4_PRESS  (1<<3)
#define KEY1_LONG_PRESS  (1<<4)
#define KEY2_LONG_PRESS  (1<<5)
#define KEY3_LONG_PRESS  (1<<6)
#define KEY4_LONG_PRESS  (1<<7)

typedef struct _key_t {
    GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pinx;
	enum {
		KEY_NOPRESS,
		KEY_PRESS_TO_DEAL,
		KEY_PRESS_DEALED,
	}key_state;
	int count;
	char to_deal, to_deal_long;
	
}key_t;

void key_init(void);
uint8_t key_get_state(void);
void key_time_deal(void);
#endif
