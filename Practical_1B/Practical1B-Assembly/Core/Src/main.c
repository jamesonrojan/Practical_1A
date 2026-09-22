/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * EEE3096S 2026 - Practical 1B
  * Tasks 4 and 5: cycle-counted phase delay, and LCD analog debugging
  *
  * Student 1 : <name>  <student number>
  * Student 2 : <name>  <student number>
  * Date      : <date>
  *
  * This file starts the peripherals and hands control to your Assembly.
  * The work happens in Core/Src/dsp.s (Task 4) and Core/Src/lcd.s (Task 5).
  *
  * Task 4 pins
  *   PB0  : ADC input, signal generator. Remove the POT0 jumper first.
  *   PA4  : DAC1 output, scope CH2.
  *
  * Task 5 pins
  *   PC15 : LCD Enable, 3.3 V side. Scope CH1.
  *   PC14 : LCD Register Select.
  *   PB8  : LCD D4      PB9  : LCD D5
  *   PA12 : LCD D6      PA15 : LCD D7
  *
  * Set ACTIVE_TASK below, rebuild, and flash.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN PD */

/* Pick the task to run: 4 for the phase delay, 5 for the LCD. */
#define ACTIVE_TASK   4

/*
 * ADC channel for Task 4.
 * ADC_CHANNEL_8 is PB0, the pin named in the practical sheet.
 * PB0 also drives user LED D1 through a 150 ohm resistor, so the LED loads
 * the signal generator and clips the top of the wave. Measure the input on
 * CH1 and report what you see. If your bench setup uses PA1 instead,
 * change this to ADC_CHANNEL_1 and update the .ioc to match.
 */
#define ADC_INPUT_CHANNEL   ADC_CHANNEL_8

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DAC_HandleTypeDef hdac1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_DAC1_Init(void);

/* USER CODE BEGIN PFP */
extern void DSP_Loop(void);   /* Task 4, defined in dsp.s. Never returns. */
extern void LCD_Run(void);    /* Task 5, defined in lcd.s. Never returns. */
/* USER CODE END PFP */

/**
  * @brief  The application entry point.
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC_Init();
  MX_DAC1_Init();

  /* USER CODE BEGIN 2 */

#if (ACTIVE_TASK == 4)

  /*
   * TODO 1
   * Start the ADC in continuous mode and start DAC channel 1, then hand
   * over to the Assembly loop.
   *
   * HAL_ADC_Start(&hadc);
   * HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
   * DSP_Loop();
   *
   * Two settings in the .ioc decide whether this works at all:
   *   Continuous Conversion Mode must be Enabled, or the ADC converts once
   *   and stops, and your DAC output sits flat.
   *   Overrun must be set to "Overrun data overwritten", or the ADC halts
   *   the moment your Assembly reads it slower than it converts.
   */

#elif (ACTIVE_TASK == 5)

  /*
   * TODO 2
   * Hand over to the LCD routine.
   *
   * LCD_Run();
   *
   * The LCD needs its power rail settled before the initialisation
   * sequence starts. Add the wait inside lcd.s, not here.
   */

#else
  #error "Set ACTIVE_TASK to 4 or 5"
#endif

  /* USER CODE END 2 */

  while (1)
  {
    /* Your Assembly routine never returns, so nothing runs here. */
  }
}

/**
  * @brief System Clock Configuration
  * @note  HSI at 8 MHz, no PLL. One CPU cycle is 125 ns.
  *        Every cycle count in dsp.s and lcd.s depends on this. Leave it.
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
                                   | RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialisation. Task 4.
  */
static void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;          /* keep enabled */
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;   /* keep overwritten */
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_INPUT_CHANNEL;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialisation. Task 4. Output on PA4.
  */
static void MX_DAC1_Init(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};

  hdac1.Instance = DAC;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialisation. Task 5 LCD pins.
  * @note  Output speed stays at HIGH on purpose. Drop it to LOW and the
  *        internal slew rate limiting hides the level shifter fault you
  *        are asked to find.
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* Start every LCD line low */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8  | GPIO_PIN_9,  GPIO_PIN_RESET);

  /* PC14 RS, PC15 Enable */
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* PA12 D6, PA15 D7 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PB8 D4, PB9 D5 */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */