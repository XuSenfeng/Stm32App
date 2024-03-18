#include "time/time.h"
#include "main.h"
#include "key/key.h"
#include "tim.h"
#include "adc/myadc.h"
#include "led/led.h"
int frq_pwm, test_frq;
extern int data_X, data_Y;
extern int pwm_mode;

//使用这一个时钟检测按键状态
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM16){
        key_scan();
    }
	if(adcs[0].adc_voltage > adcs[1].adc_voltage){
		led_set_pin( led_get_pin() ^ (1<<2) ); 
	}
}
//获取PA1的频率
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		int capture;
		capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		frq_pwm = 100000 / capture;
		__HAL_TIM_SetCounter(htim, 0);
		HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_2);
		//使用PA7输出对应的频率
		__HAL_TIM_SetAutoreload(&htim17, pwm_mode ? capture * 10 * data_Y :  capture * 10 / data_X);
	}
//	if(htim->Instance == TIM15){
//		int capture;
//		capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
//		test_frq = 100000 / capture;
//		__HAL_TIM_SetCounter(htim, 0);
//		HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_2);
//	}
}



