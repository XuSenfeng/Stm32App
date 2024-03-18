#ifndef KEY_H
#define KEY_H
#include "main.h"


#define KEY1_Press			(1<<0)
#define KEY2_Press			(1<<1)
#define KEY3_Press			(1<<2)
#define KEY4_Press			(1<<3)

#define KEY1_LONG_Press			(1<<4)
#define KEY2_LONG_Press			(1<<5)
#define KEY3_LONG_Press			(1<<6)
#define KEY4_LONG_Press			(1<<7)
typedef struct _key_t{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pinx;
	enum{
		KEY_NO_PRESS,
		KEY_PRESS_UNAFFIRM,
		KEY_PRESS_AFFIRM,
	}state;
	GPIO_PinState input;
	int time;
	unsigned char long_press;
	uint8_t to_deal;
}key_t;




void KEY_Init(void);
uint8_t key_get_state(void);
void key_do_handler(void);
#endif /*KEY_H*/
