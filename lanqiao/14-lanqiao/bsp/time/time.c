#include "time/time.h"
#include "main.h"
#include "tim.h"
#include "key/key.h"
#include "adc/bsp_adc.h"
#include "time/time.h"
uint32_t curr_val;
uint32_t frq, duty;
uint32_t rise, fall;
int pwm_duty = 0;
int pwm_change_time = 0;

int get_PA7_frq(void){
	return frq;
}
	
void set_duty_from_adc(void){
	int duty = get_pwm_duty_cycle();
	pwm_duty = duty;
	int auto_load = __HAL_TIM_GetAutoreload(&htim2);
	duty = duty *auto_load / 100;
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, duty);
}
//处理PA7的捕获
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM17){
		curr_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COUNTER(htim, 0);
		frq = (80000000 / 80) / curr_val;
		HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
	}
	if(htim->Instance == TIM8){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			rise = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			fall = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			__HAL_TIM_SET_COUNTER(htim, 0);
			duty = 100 * fall / rise;
			
			HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
			HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_2);
		}
	}
}

int fre_curr = 4000;
enum pwm_direction{
	PWM_FRE_UP,
	PWM_FRE_DOWN,
};

enum pwm_state{
	PWM_STATE_HIGH,
	PWM_STATE_LOW,
};
volatile enum pwm_state     pwm_state = PWM_STATE_LOW;
volatile enum pwm_direction direction = PWM_FRE_DOWN;
volatile int deal_pwm = 1;
int finish = 1;
//获取当前的PWM状态
int PWM_GetDirection(void){
	return pwm_state;
}
//改变PWM的频率
void PWM_SetDirection(int ndirection){
	if(finish){
		finish = 0;
		direction = (enum pwm_direction)ndirection;
		HAL_TIM_Base_Start_IT(&htim16);
	}
}

//使用一个时钟进行轮询
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM6)
	{
		key_do_handler();
		if(deal_pwm)
			set_duty_from_adc();
	}else if(htim->Instance == TIM16){
		//处理一下PWM的频率
		if(direction == PWM_FRE_UP){
			//PWM频率增加
			fre_curr += 100;
			if(fre_curr <= 8000){
				int period = 80000000 / 80 / fre_curr;
				__HAL_TIM_SetAutoreload(&htim2, period);
			}else{
				fre_curr = 8000;
				HAL_TIM_Base_Stop(htim);
				finish = 1;
				pwm_state = PWM_STATE_HIGH;
				pwm_change_time++;
			}
		}else{
			fre_curr -= 100;
			if(fre_curr >= 4000){
				int period = 80000000 / 80 / fre_curr;
				__HAL_TIM_SetAutoreload(&htim2, period);
			}else{
				fre_curr = 4000;
				HAL_TIM_Base_Stop(htim);
				finish = 1;
				pwm_state = PWM_STATE_LOW;
				pwm_change_time++;
			}
		}
	}
}


