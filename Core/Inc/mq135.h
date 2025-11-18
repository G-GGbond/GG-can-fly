/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mq135.h
  * @brief   MQ135气体传感器驱动头文件
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MQ135_H__
#define __MQ135_H__

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Exported_Functions */

void MX_ADC1_Init(void);
void MQ135_ADC_Init(void);
uint32_t Get_Adc_MQ135_Value(void);
uint32_t Get_MQ135_Percentage_value(void);
uint8_t Get_MQ135_DO_value(void);

/* USER CODE END Exported_Functions */

#ifdef __cplusplus
}
#endif

#endif /* __MQ135_H__ */