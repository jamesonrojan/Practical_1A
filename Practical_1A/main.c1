/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32f0xx.h"
#include <stdint.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */
// LED toggle pin (PB0 = LED0)

/*Define the LED*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM16_Init(void);

/* USER CODE BEGIN PFP */
void TIM16_IRQHandler(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM16_Init();

  /* USER CODE BEGIN 2 */
  // Start TIM16 in interrupt mode
  HAL_TIM_Base_Start_IT(&htim16);
  /* USER CODE END 2 */

  while (1)
  {
    // Main loop does nothing – all work is in the ISR
  }
}

/**
  * @brief System Clock Configuration (HSI 8 MHz)
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {}

  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1) {}

  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {}

  LL_SetSystemCoreClock(8000000);
  if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief TIM16 Initialization|
  * Insert your calculated values
  */
static void MX_TIM16_Init(void)
{
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 7999; // insert your psc value
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 999;          // insert your calculated period or ARR
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
    Error_Handler();
  }
  NVIC_EnableIRQ(TIM16_IRQn);
}

/**
  * @brief GPIO Initialization – PB0 as output (LED0)
  */
static void MX_GPIO_Init(void)
{
/*Insert PB0 initialization code*/
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief TIM16 interrupt handler – toggles PB0 each time
  */
void TIM16_IRQHandler(void)
{
/*Toggle the pin*/
  if (__HAL_TIM_GET_FLAG(&htim16, TIM_SR_UIF) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE); // Clear the interrupt flag
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle PB0
    }
  }

/**
  * @brief Error handler
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
