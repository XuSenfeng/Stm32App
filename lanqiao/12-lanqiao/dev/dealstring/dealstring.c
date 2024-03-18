#include "dealstring/dealstring.h"
#include "ring/ring.h"
#include "string.h"
#include "usart.h"
#include "stdio.h"
#include "led/led.h"
car_t CNBR_car[2];
car_t VNBR_car[4];
extern int CNBR_num, VNBR_num;
extern double CNBR_price, VNBR_price;
//初始化这几个车
void car_init(void){
	for(int i = 0;i<2;i++){
		CNBR_car[i].car_type = CNBR_TYPE;
		CNBR_car[i].used = 0;
	}
	for(int i = 0;i<4;i++){
		VNBR_car[i].car_type = VNBR_TYPE;
		VNBR_car[i].used = 0;
	}
}

//获取一个车
//参数是车的类型
car_t * car_alloc(int type){
	if(type){
		//CNBR
		for(int i = 0;i<2;i++){
			if(CNBR_car[i].used == 0){
				CNBR_car[i].used = 1;
				CNBR_num--;
				return &CNBR_car[i];
			}	
		}
	}else{
		for(int i = 0;i<4;i++){
			if(VNBR_car[i].used == 0){
				VNBR_car[i].used = 1;
				VNBR_num--;
				return &VNBR_car[i];
			}	
		}
	}
	return (car_t *)0;	

}

//获取一个空闲的车
void car_free(car_t * car){
	if(car->car_type == CNBR_TYPE)
		CNBR_num++;
	else
		VNBR_num++;
	car->used = 0;

	int led_state = led_read();
	led_state |= 1<<0;
	led_write(led_state);

}

//通过车的名字获取一个车
car_t * car_get_car_by_name(const char * name, car_type_e type){
	if(type == CNBR_TYPE)
	{
		for(int i = 0;i<2;i++){
			if(strncmp((char *)CNBR_car[i].name, name, 4)==0){
				if(CNBR_car[i].used)
					return &CNBR_car[i];
			}
		}
	}
	if(type == VNBR_TYPE)
	{
		for(int i = 0;i<4;i++){
			if(strncmp((char *)VNBR_car[i].name, name, 4)==0){
				if(VNBR_car[i].used)
					return &VNBR_car[i];
			}
		}
	}
	return (car_t * )0;
}

extern ring_t uart_ring;
//通过串口信息获取时间
void get_time(time_t *time){
	uint8_t buf[20];
	ring_buf_read(&uart_ring, buf, 12);
	sscanf((const char *)buf, "%2d%2d%2d%2d%2d%2d", 
	&time->year,
	&time->month, 
	&time->day, 
	&time->hour, 
	&time->minute, 
	&time->second
);
}
//看一看这一年是不是闰年
int is_year_366(int year){
	if((year % 4 == 0) && year % 100 != 0){
		return 1;
	}
	return 0;
}

//获取这一个时间离这一年结束的天数
int left_days(time_t *time){
	int day = 0;
	switch (time->month){
		case 1:
			day+=31;
		case 2:
			if(is_year_366(time->year))
				day+=29;
			else
				day+=28;
		case 3:
			day+=31;
		case 4:
			day+=30;
		case 5:
			day+=31;
		case 6:
			day+=30;
		case 7:
			day+=31;
		case 8:
			day+=31;
		case 9:
			day+=30;
		case 10:
			day+=31;
		case 11:
			day+=30;
		case 12:
			day+=31;
	}
	day-=time->day;
	return day;
}

//获取两个时间之间的小时数
int get_hour(time_t *coming_time, time_t *leaving_time){
//	uint8_t buf[30];
	int year = coming_time->year;
	int day = 0;
	day = left_days(coming_time);
	if(leaving_time->year > coming_time->year){
		while(++year <= leaving_time->year){
			if(is_year_366(year))
				day+=366;
			else
				day+=365;
		}
	}
	day -= left_days(leaving_time);
//	sprintf((char *)buf, "day = %d\n" ,day);
//	HAL_UART_Transmit(&huart1, buf, strlen((const char *)buf), 1000);
	int hour = 24 * day;
	hour = hour + leaving_time->hour - coming_time->hour;
	if(leaving_time->minute > coming_time->minute)
		hour += 1;
	return hour;
}
//处理一个新来的车
int deal_car_coming(const char * name, car_type_e type){
	car_t * car_curr = 0;
	car_curr = car_alloc(type);
	if(!(VNBR_num + CNBR_num)){
		int led_state = led_read();
		led_state &= ~(1<<0);
		led_write(led_state);
	}
	if(car_curr == (car_t *)0){
		return -1;
	}
	strcpy(car_curr->name, name);
	get_time(&car_curr->coming_time);
	return 0;
}
//处理一个离开的车
int deal_car_leaving(car_t * car){
	int hour;
	uint8_t buf[30];
	get_time(&car->leaving_time);
	hour = get_hour(&car->coming_time, &car->leaving_time);
	if(hour < 0){
		car_free(car);
		return -1;
	}	
	sprintf((char *)buf, "%s:%s:%d:%.2f\n", 
	car->car_type == VNBR_TYPE ? "VNBR" : "CNBR",
	car->name, 
	hour, 
	hour * (car->car_type == VNBR_TYPE ? VNBR_price : CNBR_price)
	);
	HAL_UART_Transmit(&huart1, buf, strlen((const char *)buf), 1000);
	car_free(car);
	return 0;
}

//VNBR:D583:200202120000
//YYMMDDHHmmSS
void deal_string(void){
	car_t * car_curr = 0;
	uint8_t temp_buf1[20] = {0};
	uint8_t temp_buf2[20] = {0};
	car_type_e car_type_curr = VNBR_TYPE;
	
	ring_buf_read(&uart_ring, temp_buf1, 5);
	ring_buf_read(&uart_ring, temp_buf2, 5);
	temp_buf2[4] = 0;
	
	//获取被一个车的类型
	if(strncmp((const char *)temp_buf1, "VNBR:", 5)==0){
		car_type_curr = VNBR_TYPE;
	}else if(strncmp((const char *)temp_buf1, "CNBR:", 5)==0){
		car_type_curr = CNBR_TYPE;
	}else{
		goto deal_err;
	}
	//处理车的名字
	int err = 0;
	car_curr = car_get_car_by_name((const char *)temp_buf2, car_type_curr);
	if(car_curr == (car_t *)0){
		//这一个车是新进来的
		err = deal_car_coming((const char *)temp_buf2, car_type_curr);
	}else{
		err = deal_car_leaving(car_curr);
	}
	if(err < 0){
		goto deal_err;
	}
	
	return;
deal_err:
	//获取的数据是错误的
	//清空缓冲区
	while(ring_buf_read(&uart_ring, temp_buf1, 1));
	HAL_UART_Transmit(&huart1, "Error", 5, 500);

}



