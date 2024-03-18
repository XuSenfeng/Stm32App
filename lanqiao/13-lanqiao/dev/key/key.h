#ifndef KEY_H
#define KEY_H
#include "main.h"

#define KEY1_PRESS (1<<0)
#define KEY2_PRESS (1<<1)
#define KEY3_PRESS (1<<2)
#define KEY4_PRESS (1<<3)
#define KEY1_LOG_PRESS  (1<<4)
#define KEY2_LOG_PRESS  (1<<5)
#define KEY3_LOG_PRESS  (1<<6)
#define KEY4_LOG_PRESS  (1<<7)
 


typedef struct _key_t{
    GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pinx;
    enum state{
        KEY_NOPRESS,
        KEY_PRESS_PENDING,
        KEY_PRESS,
        KEY_LONGPRESS,
    }key_state;
    int count;
    char to_deal, to_deal_long;
}key_t;
void key_init(void);
void key_scan(void);
uint8_t key_get_state(void);

#endif // !KEY_H
