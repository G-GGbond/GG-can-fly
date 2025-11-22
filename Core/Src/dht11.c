#include "dht11.h"
#include "main.h"
#include "core_cm3.h" 
#include <stdio.h>

void DWT_Init(void)
{
    // 启用跟踪功能
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    
    // 重置周期计数器
    DWT->CYCCNT = 0;
    
    // 启用周期计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief 微秒延时函数
  * @param us: 延时的微秒数
  */
void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    // 计算需要的时钟周期数
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    
    // 等待周期计数器达到目标值
    while ((DWT->CYCCNT - start) < cycles) {
        // 空循环
    }
}

/**
  * @brief 获取DWT计数器值（用于调试）
  */
uint32_t get_dwt_count(void)
{
    return DWT->CYCCNT;
}


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* TIM4 init function */


/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Mode_OUT
 * 函 数 说 明：配置DHT11的数据引脚为输出模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_IO_OUT()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Mode_IN
 * 函 数 说 明：配置DHT11的数据引脚为输入模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_IO_IN()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // 上拉输入模式
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_LOW; 	//拉低DQ
    HAL_Delay(20);     	//拉低至少18ms
    DHT11_HIGH; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
// 检查DHT11响应（您缺少这个函数）
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN();
    
    // 等待DHT11拉低
    while (DHT11_DQ_IN && retry < 100) {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1;
    
    retry = 0;
    // 等待DHT11拉高
    while (!DHT11_DQ_IN && retry < 100) {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1;
    
    return 0;
}

// 读取一个位
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    
    // 等待低电平开始
    while (DHT11_DQ_IN && retry < 100) {
        retry++;
        delay_us(1);
    }
    
    retry = 0;
    // 等待高电平开始
    while (!DHT11_DQ_IN && retry < 100) {
        retry++;
        delay_us(1);
    }
    
    delay_us(40); // 等待40us后采样
    
    return DHT11_DQ_IN ? 1 : 0;
}

// 读取一个字节
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// 读取温湿度数据
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    
    DHT11_Rst();
    
    if (DHT11_Check() == 0) {
        for (i = 0; i < 5; i++) {
            buf[i] = DHT11_Read_Byte();
        }
        
        // 校验数据
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
            *humi = buf[0];
            *temp = buf[2];
            return 0; // 成功
        }
    }
    return 1; // 失败
}