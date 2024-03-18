#ifndef MYADC_H
#define MYADC_H
#include "main.h"

typedef struct _adc_t{
	int adc_original;
	double adc_voltage;
}adc_t;
void read_adc(void);
extern adc_t adcs[2];
#endif

