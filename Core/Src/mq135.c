#include "main.h"
#include "mq135.h"
/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */

#define SAMPLES           30
ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/******************************************************************
 * 函 数 名 称：MQ135_ADC_Init
 * 函 数 说 明：启动ADC连续转换（在main函数中调用）
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：User
 * 备       注：需在MX_ADC1_Init()后调用
******************************************************************/
void MQ135_ADC_Init(void)
{
    // 执行ADC校准
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    // 启动连续转换
    HAL_ADC_Start(&hadc1);
}


/******************************************************************
 * 函 数 名 称：Get_Adc_MQ135_Value
 * 函 数 说 明：获取ADC采样平均值
 * 函 数 形 参：无
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint32_t  Get_Adc_MQ135_Value(void)
{
    uint32_t Data = 0;

    for(int i = 0; i < SAMPLES; i++)
    {
        // 等待转换完成并读取值
        HAL_ADC_PollForConversion(&hadc1, 10);
        Data += HAL_ADC_GetValue(&hadc1);
        
        HAL_Delay(5);
    }

    Data = Data / SAMPLES;
    return Data;
}

/******************************************************************
 * 函 数 名 称：Get_MQ135_Percentage_value
 * 函 数 说 明：读取MQ135值，并且返回百分比
 * 函 数 形 参：无
 * 函 数 返 回：返回百分比
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint32_t  Get_MQ135_Percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;

    adc_new = Get_Adc_MQ135_Value();
    Percentage_value = ((float)adc_new / (float)adc_max) * 100.f;
    return Percentage_value;
}

/******************************************************************
 * 函 数 名 称：Get_MQ135_DO_value
 * 函 数 说 明：获取MQ135DO引脚的电平状态
 * 函 数 形 参：无
 * 函 数 返 回：0=未检测到高于灵敏度的酒精值 1=检测到高于灵敏度的酒精值
 * 作       者：LC
 * 备       注：调整模块上的滑动电阻即可调整灵敏度
******************************************************************/
uint8_t Get_MQ135_DO_value(void)
{
    // 读取数字输出引脚状态
   return HAL_GPIO_ReadPin(MQ135_GPIO_Port, MQ135_Pin);
}