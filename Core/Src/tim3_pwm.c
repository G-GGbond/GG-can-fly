#include "tim3_pwm.h"

// 定时器句柄定义
TIM_HandleTypeDef htim3;

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;  // 72MHz / (7199+1) = 10kHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;      // 10kHz / 1000 = 10Hz PWM
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief 启动风扇PWM
  */
void TIM3_PWM_Start(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

/**
  * @brief 停止风扇PWM
  */
void TIM3_PWM_Stop(void)
{
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

/**
  * @brief 设置风扇速度
  * @param speed_percent: 风扇速度百分比 (0-100)
  * 0=停止, 100=全速
  */
void TIM3_PWM_Set_Speed(uint8_t speed_percent)
{
    // 限制速度范围
    if (speed_percent > 100) speed_percent = 100;
    
    // 计算脉冲值：Period=999，所以有效范围是0-999
    uint32_t pulse = speed_percent * 10;  // 100%对应1000，但Period=999，所以100%时用999
    
    // 确保不超过Period值
    if (pulse > htim3.Init.Period) pulse = htim3.Init.Period;
    
    // 设置比较值
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
}


