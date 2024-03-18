#include "adc/myadc.h"
#include "adc.h"

adc_t adcs[2];

void read_adc(void){
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 50);
	adcs[0].adc_original =  HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 50);
	adcs[1].adc_original = HAL_ADC_GetValue(&hadc2);
	
	
	adcs[0].adc_voltage = adcs[0].adc_original * 3.3 /4096;
	adcs[1].adc_voltage = adcs[1].adc_original * 3.3 /4096;
}


