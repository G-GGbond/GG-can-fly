#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief USART1初始化函数声明
  * @param None
  * @retval None
  */
extern UART_HandleTypeDef huart1;
void MX_USART1_UART_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __UART_H */