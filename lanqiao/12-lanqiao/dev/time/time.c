#include "time/time.h"
#include "key/key.h"
#include "tim.h"
uint32_t frq_pwm;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM16){
        key_time_deal();
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		int capture;
		capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		frq_pwm = 100000 / capture;
		__HAL_TIM_SetCounter(htim, 0);
		HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_2);
	}
}

