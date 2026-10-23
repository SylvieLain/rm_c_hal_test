/**
  ******************************************************************************
  * @file    usart.c
  * @brief   4 路串口初始化（全部 115200-8-N-1，TX 轮询发送）
  *          USART1: PA9 / PB7    (AF7)  原工程 printf 口
  *          USART2: PD5 / PD6    (AF7)  C 板串口2，接 CH340
  *          USART3: PC10 / PC11  (AF7)  原工程 DBUS（正式使用时改 100000-8-E-1）
  *          USART6: PG14 / PG9   (AF8)  C 板串口4
  ******************************************************************************
  */
#include "main.h"
#include "usart.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

static void uart_common_init(UART_HandleTypeDef *huart, USART_TypeDef *inst)
{
  huart->Instance          = inst;
  huart->Init.BaudRate     = 115200;
  huart->Init.WordLength   = UART_WORDLENGTH_8B;
  huart->Init.StopBits     = UART_STOPBITS_1;
  huart->Init.Parity       = UART_PARITY_NONE;
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_USART1_UART_Init(void) { uart_common_init(&huart1, USART1); }
void MX_USART2_UART_Init(void) { uart_common_init(&huart2, USART2); }
void MX_USART3_UART_Init(void) { uart_common_init(&huart3, USART3); }
void MX_USART6_UART_Init(void) { uart_common_init(&huart6, USART6); }

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (uartHandle->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_9;           /* PA9  USART1_TX */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;                 /* PB7  USART1_RX */
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if (uartHandle->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_5 | GPIO_PIN_6;   /* PD5 TX / PD6 RX */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
  else if (uartHandle->Instance == USART3)
  {
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_10 | GPIO_PIN_11; /* PC10 TX / PC11 RX */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
  else if (uartHandle->Instance == USART6)
  {
    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_14 | GPIO_PIN_9;  /* PG14 TX / PG9 RX */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
  if (uartHandle->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
  }
  else if (uartHandle->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5 | GPIO_PIN_6);
  }
  else if (uartHandle->Instance == USART3)
  {
    __HAL_RCC_USART3_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11);
  }
  else if (uartHandle->Instance == USART6)
  {
    __HAL_RCC_USART6_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14 | GPIO_PIN_9);
  }
}
