#include "adc/bsp_adc.h"
#include "main.h"
#include "adc.h"


double get_adc(void){
	uint32_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 50);
	adc = HAL_ADC_GetValue(&hadc2);
	return adc * 3.3 / 4096;
}

int get_adc_original(void){
	uint32_t adc;
	HAL_ADC_Start(&hadc2);
	adc = HAL_ADC_GetValue(&hadc2);
	return adc;
}

int get_pwm_duty_cycle(void){
	int data = get_adc_original();
	int ret = 0;
	if(data < 1233){
		ret = 10;
	}else if(data > 3699){
		ret = 85;
	}else{
		ret =  15 + (data - 1233) * 75 / 2466;
	}
	return ret;
}




