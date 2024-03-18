#ifndef DEALSTRING_H
#define DEALSTRING_H
#include "main.h"


typedef struct _time_t{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}time_t;

typedef enum {
	VNBR_TYPE,
	CNBR_TYPE,
}car_type_e;

typedef struct _car_t{
	char name[5];
	car_type_e car_type;
	time_t coming_time;
	time_t leaving_time;
	uint8_t used;
}car_t;
void car_init(void);
void deal_string(void);
#endif

