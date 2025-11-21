#ifndef __TIM3_PWM_H
#define __TIM3_PWM_H

#include "main.h"

// 导出定时器句柄，供其他文件使用
extern TIM_HandleTypeDef htim3;

// 函数声明
void MX_TIM3_Init(void);
void TIM3_PWM_Init(void);
void TIM3_PWM_Start(void);
void TIM3_PWM_Stop(void);
void TIM3_PWM_Set_Speed(uint8_t speed_percent);  // 0-100%
#endif