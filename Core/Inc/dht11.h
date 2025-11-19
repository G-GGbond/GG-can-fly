#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

// 函数声明
void DHT11_GPIO_Init(void);
void DHT11_GPIO_Mode_OUT(void);
void DHT11_GPIO_Mode_IN(void);
unsigned int DHT11_Read_Data(void);
float Get_temperature(void);
float Get_humidity(void);
void delay_us(uint16_t us); 

extern float temperature;
extern float humidity;

#endif