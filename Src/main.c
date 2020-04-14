/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "74HC595.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
enum
{
	NULL_SET,
	SECOND_SET,
	MINUTE_SET,
	HOUR_SET
};
enum
{
	NULL_CLICK,
	SHORT_CLICK,
	LONG_CLICK
};	
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t btn_status = 0;
uint16_t push_tick = 0;
uint8_t bounce_tick = 0;
//uint8_t cmd_num = 0;
uint16_t update_tick = 0;
uint8_t cmd_num = 0;
uint8_t click = NULL_CLICK;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void handle_button(void);
void service_blink_disp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	uint8_t h = 0;
	uint8_t m = 0;
	uint8_t s = 0;

	uint32_t tmp_h = 0;
	uint32_t tmp_m = 0;
	uint32_t tmp_s = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	LL_SYSTICK_EnableIT();
	LL_RTC_EnableInitMode(RTC);
	
	bin_disp_init();

//	LL_RTC_DisableWriteProtection(RTC);
//	LL_RTC_EnterInitMode(RTC);
//	LL_RTC_TIME_Config(RTC, LL_RTC_HOURFORMAT_24HOUR, 0x1, 0x1, 0x1);
//	LL_RTC_DisableInitMode(RTC);
//	LL_RTC_EnableWriteProtection(RTC);
	
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
		
		if(!update_tick)
		{
			h = LL_RTC_TIME_GetHour(RTC);
			m = LL_RTC_TIME_GetMinute(RTC);
			s = LL_RTC_TIME_GetSecond(RTC);
			
			h = (h & 0xF) + ((h & 0xF0) >> 4)*10; //bcd to dec
			m = (m & 0xF) + ((m & 0xF0) >> 4)*10;
			s = (s & 0xF) + ((s & 0xF0) >> 4)*10;
			bin_disp_write(s, m, h);
			update_tick = 300;
		}
		
		handle_button();
		
		if(click == LONG_CLICK)
		{
			cmd_num = SECOND_SET;
			click = NULL_CLICK;
		}		

		while(cmd_num)
		{
			service_blink_disp();
			handle_button();			
		}	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_HSE_DIV32);
  LL_RCC_EnableRTC();
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  LL_RTC_InitTypeDef RTC_InitStruct = {0};

  /* Peripheral clock enable */
  LL_RCC_EnableRTC();

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC and set the Time and Date 
  */
  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
  RTC_InitStruct.AsynchPrescaler = 99;
  RTC_InitStruct.SynchPrescaler = 399;
  LL_RTC_Init(RTC, &RTC_InitStruct);
  LL_RTC_SetAsynchPrescaler(RTC, 99);
  LL_RTC_SetSynchPrescaler(RTC, 399);
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);
  
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration  
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_ResetOutputPin(ENTER_GPIO_Port, ENTER_Pin);

  /**/
  GPIO_InitStruct.Pin = ENTER_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(ENTER_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void handle_button()
{
	if((LL_GPIO_ReadInputPort(GPIOA) & BTN_Pin) == BTN_Pin)
	{
		if(push_tick > 100) click = SHORT_CLICK;
		push_tick = 0;
	}
	else if(!push_tick) push_tick = 1;
	
	if(push_tick > 3000)
	{
		click = LONG_CLICK;
		push_tick = 0;
	}
	
	if(click == LONG_CLICK)
	{
		click = NULL_CLICK;
		if(++cmd_num > 3)
		{ 
			cmd_num = 0;
			LL_RTC_DisableWriteProtection(RTC);
			LL_RTC_EnterInitMode(RTC);
			LL_RTC_TIME_Config(RTC, LL_RTC_HOURFORMAT_24HOUR, ((h / 10) << 4) + h % 10, ((m / 10) << 4) + m % 10, ((s / 10) << 4) + s % 10);
			LL_RTC_DisableInitMode(RTC);
			LL_RTC_EnableWriteProtection(RTC);
		}
	}
	
	if(click == SHORT_CLICK)
	{
		click = NULL_CLICK;
		update_tick = 1000;
		switch(cmd_num)
		{
			case SECOND_SET:
				if(++s >= 60) s = 0;
				bin_disp_write(s, 0, 0);
				break;
			case MINUTE_SET:
				if(++m >= 60) m = 0;
				bin_disp_write(0, m, 0);
				break;
			case HOUR_SET:
				if(++h >= 24) h = 0;
				bin_disp_write(0, 0, h);
				break;
		}
	}
}

void service_blink_disp()
{
	if(update_tick == 500)
				{
					switch(cmd_num)
					{
						case SECOND_SET:
							bin_disp_write(s, 0, 0);
							break;
						case MINUTE_SET:
							bin_disp_write(0, m, 0);
							break;
						case HOUR_SET:
							bin_disp_write(0, 0, h);
							break;
					}
				}
				if(update_tick == 0)
				{
					bin_disp_write(0, 0, 0);
					update_tick = 1000;
				}	
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
