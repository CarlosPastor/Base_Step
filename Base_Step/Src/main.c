/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

uint8_t buff_m1[4];
uint8_t buff_m2[4];
uint8_t tx_buff[2];
uint8_t tx_nop[] = {0, 0};

//change speed call

void change_speed_timed(int speed_8bit, int time_ms)
{
	  //start motor script
	  //Time greater to 10ms
	  //command 1
	  buff_m1[0] = 0b01010000;
	  //data (speed)
	  buff_m1[1] = 0;
	  buff_m1[2] = speed_8bit;
	  buff_m1[3] = 0;

	  //command 2
	  buff_m2[0] = 0b01010001;
	  //data (speed)
	  buff_m2[1] = 0;
	  buff_m2[2] = speed_8bit;
	  buff_m2[3] = 0;

	  //The trasnmision od multybyte commands is complex as tima shoud be left for the logic inside
	  //the driver to decode each part of the mesage. To solve this we will do a blank write between commands
	  // but its not a good solution, a real time implementation is needed to provide robustness to the software.
	  //A well timed software loop should be enought.

	  for(int i = 0; i < 4; i++)
	  {
		  //put the data in the out buffer
		  tx_buff[0] = buff_m1[i];
		  tx_buff[1] = buff_m2[i];
		  //stat write
		  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_RESET);
		  HAL_SPI_Transmit(&hspi1, tx_buff, 2, 1000);
		  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
		  //wait write
		  HAL_Delay(5);
	  }

	  HAL_Delay(time_ms);
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();

  /* USER CODE BEGIN 2 */

  //HAL_TIM_Base_Start_IT(&htim6);

  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, nSTBY_nRST_Pin, GPIO_PIN_RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(GPIOB, nSTBY_nRST_Pin, GPIO_PIN_SET);

  //Reset
  //put the data in the out buffer
  tx_buff[0] = 0b11000000;
  tx_buff[1] = 0b11000000;

  //stat write
  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, tx_buff, 2, 5000);
  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
  //one byte command end

  HAL_Delay(1000);

  //GetStatus
  //put the data in the out buffer
  //command 1
  buff_m1[0] = 0b1101000;
  //data (speed)
  buff_m1[1] = 0;
  buff_m1[2] = 0;
  buff_m1[3] = 0;

  //command 2
  buff_m2[0] = 0b1101000;
  //data (speed)
  buff_m2[1] = 0;
  buff_m2[2] = 0;
  buff_m2[3] = 0;

  for(int i = 0; i < 4; i++)
  {
	  //put the data in the out buffer
	  tx_buff[0] = buff_m1[i];
	  tx_buff[1] = buff_m2[i];
	  //stat write
	  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, tx_buff, 2, 1000);
	  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
	  //wait write
	  HAL_SPI_Transmit(&hspi1, tx_nop, 2, 1000);
  }

  HAL_Delay(1000);

  change_speed_timed(50, 2000);
  change_speed_timed(120, 2000);
  change_speed_timed(10, 2000);
  change_speed_timed(100, 2000);
  change_speed_timed(25, 2000);
  change_speed_timed(86, 2000);
  change_speed_timed(30, 2000);

  //REPEAT TO STOP
  //start motor script
  //command 1
  buff_m1[0] = 0b01010000;
  //data (speed)
  buff_m1[1] = 0;
  buff_m1[2] = 0;
  buff_m1[3] = 0;

  //command 2
  buff_m2[0] = 0b01010001;
  //data (speed)
  buff_m2[1] = 0;
  buff_m2[2] = 0;
  buff_m2[3] = 0;

  for(int i = 0; i < 3; i++)
  {
	  //put the data in the out buffer
	  tx_buff[0] = buff_m1[i];
	  tx_buff[1] = buff_m2[i];
	  //stat write
	  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, tx_buff, 2, 5000);
	  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
	  //wait write
	  HAL_SPI_Transmit(&hspi1, tx_nop, 2, 5000);
  }

  HAL_Delay(1000);

  //HI-Z OUT
  //put the data in the out buffer
  tx_buff[0] = 0b10100000;
  tx_buff[1] = 0b10100000;
  //stat write
  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, tx_buff, 2, 5000);
  HAL_GPIO_WritePin(GPIOA, SPI_CS1_Pin, GPIO_PIN_SET);
  //one byte command end



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

	  HAL_Delay(50);

	  HAL_GPIO_TogglePin(GPIOA, LED_2_Pin);

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//
//
//
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
