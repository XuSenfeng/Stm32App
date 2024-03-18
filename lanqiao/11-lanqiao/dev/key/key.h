#ifndef KEY_H
#define KEY_H
#include "main.h"

#define KEY1_PRESS			(1<<0)
#define KEY2_PRESS			(1<<1)
#define KEY3_PRESS			(1<<2)
#define KEY4_PRESS			(1<<3)
#define KEY1_LONG_PRESS			(1<<4)
#define KEY2_LONG_PRESS			(1<<5)
#define KEY3_LONG_PRESS			(1<<6)
#define KEY4_LONG_PRESS			(1<<7)


typedef enum _key_state_e
{
    KEY_NOPRESS,
    KEY_PRESS,
    KEY_LONG_PRESS,
}key_state_e;

typedef struct  _key_t
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pinx;
    uint8_t to_deal, to_deal_long;
    key_state_e state;
    int count; 
}key_t;
 
uint8_t key_scan(void);
void key_init(void);
void key_deal_time(void);
#endif // !KEY_H
