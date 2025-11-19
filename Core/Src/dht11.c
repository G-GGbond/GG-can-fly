#include "dht11.h"

// 定义DHT11引脚 - 请根据您的实际配置修改
#ifndef DHT11_Pin
#define DHT11_Pin GPIO_PIN_0
#endif

#ifndef DHT11_GPIO_Port
#define DHT11_GPIO_Port GPIOB
#endif

// 定义宏函数
#define DATA_GPIO_OUT(state) HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, (state ? GPIO_PIN_SET : GPIO_PIN_RESET))
#define DATA_GPIO_IN HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)

float temperature = 0;
float humidity = 0;

// 使用简单的空循环延时（针对72MHz STM32F103优化）
void delay_us(uint16_t us)
{
    uint32_t cycles = us * 9;  // 72MHz下大约每9个循环1us
    while (cycles--)
    {
        __NOP();
    }
}

/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Init
 * 函 数 说 明：DHT11温湿度传感器初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置为推挽输出，无上拉下拉，低速
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

    DATA_GPIO_OUT(1);  // 初始化为高电平
}

/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Mode_OUT
 * 函 数 说 明：配置DHT11的数据引脚为输出模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_GPIO_Mode_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

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
void DHT11_GPIO_Mode_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // 上拉输入模式

    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/******************************************************************
 * 函 数 名 称：DHT11_Read_Data
 * 函 数 说 明：根据时序读取温湿度数据
 * 函 数 形 参：无
 * 函 数 返 回：0=数据校验失败  其他=温湿度未处理的数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int DHT11_Read_Data(void)
{
    int i;
    uint64_t val = 0;  // 改为uint64_t避免符号扩展问题
    int timeout = 0;
    float small_point = 0;
    unsigned char verify_num = 0; // 验证值

    DATA_GPIO_OUT(0); // 数据线输出低电平
    HAL_Delay(19);    // 起始信号保持时间19ms
    DATA_GPIO_OUT(1); // 主机释放总线
    delay_us(30);     // 拉高等待30us

    DHT11_GPIO_Mode_IN(); // 数据线转为输入模式
    
    // 等待DHT11响应信号 - 等待低电平
    timeout = 1000;
    while ((DATA_GPIO_IN == GPIO_PIN_SET) && (timeout > 0)) 
    {
        timeout--;
        delay_us(1);
    }
    
    if (timeout <= 0) 
        return 0; // 超时

    // 等待DHT11响应信号 - 等待高电平
    timeout = 1000;
    while ((DATA_GPIO_IN == GPIO_PIN_RESET) && (timeout > 0)) 
    {
        timeout--;
        delay_us(1);
    }
    
    if (timeout <= 0) 
        return 0; // 超时

    // 等待DHT11开始发送数据 - 等待低电平
    timeout = 1000;
    while ((DATA_GPIO_IN == GPIO_PIN_SET) && (timeout > 0)) 
    {
        timeout--;
        delay_us(1);
    }
    
    if (timeout <= 0) 
        return 0; // 超时

    // 接收40位数据
    for (i = 0; i < 40; i++)
    {
        // 等待低电平结束
        timeout = 1000;
        while ((DATA_GPIO_IN == GPIO_PIN_RESET) && (timeout > 0)) 
        {
            timeout--;
            delay_us(1);
        }

        // 延时40us后检测电平
        delay_us(40);
        
        if (DATA_GPIO_IN == GPIO_PIN_SET) // 高电平表示'1'
        {
            val = (val << 1) | 1;
            
            // 等待高电平结束
            timeout = 1000;
            while ((DATA_GPIO_IN == GPIO_PIN_SET) && (timeout > 0)) 
            {
                timeout--;
                delay_us(1);
            }
        }
        else // 低电平表示'0'
        {
            val = val << 1;
        }
    }

    DHT11_GPIO_Mode_OUT(); // 转为输出模式
    DATA_GPIO_OUT(1);      // 主机释放总线

    // 校验数据
    verify_num = ((val >> 32) & 0xFF) + ((val >> 24) & 0xFF) + 
                 ((val >> 16) & 0xFF) + ((val >> 8) & 0xFF);
    
    if (verify_num != (val & 0xFF))
    {
        // 校验错误
        return 0;
    }
    else // 校验成功
    {
        // 数据处理
        humidity = (val >> 32) & 0xFF;     // 湿度整数部分
        small_point = (val >> 24) & 0xFF;  // 湿度小数部分
        humidity = humidity + small_point * 0.1;

        temperature = (val >> 16) & 0xFF;  // 温度整数部分
        small_point = (val >> 8) & 0xFF;   // 温度小数部分
        temperature = temperature + small_point * 0.1;

        return (unsigned int)(val >> 8); // 返回未处理的数据
    }
}

/******************************************************************
 * 函 数 名 称：Get_temperature
 * 函 数 说 明：获取温度数据
 * 函 数 形 参：无
 * 函 数 返 回：温度值
 * 作       者：LC
 * 备       注：使用前必须先调用 DHT11_Read_Data 读取有数据
******************************************************************/
float Get_temperature(void)
{
    return temperature;
}

/******************************************************************
 * 函 数 名 称：Get_humidity
 * 函 数 说 明：获取湿度数据
 * 函 数 形 参：无
 * 函 数 返 回：湿度值
 * 作       者：LC
 * 备       注：使用前必须先调用 DHT11_Read_Data 读取有数据
******************************************************************/
float Get_humidity(void)
{
    return humidity;
}