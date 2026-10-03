/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd/lcd.h"
#include "tof_vl53l1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
tof_vl53l1_dev_t dev1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  tft_init(PIN_ON_TOP, BLACK, WHITE, YELLOW, DARK_GREEN);
  tft_force_clear();

  // The sensor is permanently enabled; initialize it at its default address.
  HAL_Delay(10);

  uint8_t sensor_found = 0;
  uint16_t sensor_address = 0;
  for (uint8_t address = 1; address < 0x7F; address++) {
    if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 2, 100) == HAL_OK) {
        sensor_found = 1;
        sensor_address = address << 1;
        tft_prints(0, 6, "I2C found: 0x%02X", address);
        tft_update(0);
        HAL_Delay(1000);
        break;
    }
  }
  if (!sensor_found) {
    uint32_t i2c_error = HAL_I2C_GetError(&hi2c2);
    tft_prints(0, 6, "No I2C device");
    tft_prints(0, 7, "0x%08X", (int)i2c_error);
    tft_update(0);
    Error_Handler();
  }

  dev1.I2cDevAddr = sensor_address;
  int sensor_status = VL53L1X_SensorInit(dev1.I2cDevAddr);
  if (sensor_status != 0) {
    tft_force_clear();
    tft_prints(0, 0, "VL53L1X init failed");
    tft_prints(0, 1, "Status: %d", sensor_status);
    tft_update(0);
    led_on(LED1);
    Error_Handler();
  }

  sensor_status = VL53L1X_SetDistanceMode(dev1.I2cDevAddr, 2);
  if (sensor_status != 0) {
    tft_force_clear();
    tft_prints(0, 0, "VL53L1X mode failed");
    tft_prints(0, 1, "Status: %d", sensor_status);
    tft_update(0);
    led_on(LED1);
    Error_Handler();
  }

  sensor_status = VL53L1X_SetTimingBudgetInMs(dev1.I2cDevAddr, 100);
  if (sensor_status != 0) {
    tft_force_clear();
    tft_prints(0, 0, "VL53L1X timing failed");
    tft_prints(0, 1, "Status: %d", sensor_status);
    tft_update(0);
    led_on(LED1);
    Error_Handler();
  }

  sensor_status = VL53L1X_SetInterMeasurementInMs(dev1.I2cDevAddr, 100);
  if (sensor_status != 0) {
    tft_force_clear();
    tft_prints(0, 0, "VL53L1X interval failed");
    tft_prints(0, 1, "Status: %d", sensor_status);
    tft_update(0);
    led_on(LED1);
    Error_Handler();
  }

  sensor_status = VL53L1X_StartRanging(dev1.I2cDevAddr);
  if (sensor_status != 0) {
    tft_force_clear();
    tft_prints(0, 0, "VL53L1X ranging failed");
    tft_prints(0, 1, "Status: %d", sensor_status);
    tft_update(0);
    led_on(LED1);
    Error_Handler();
  }
	// we turn off all the led first
	led_off(LED1);
	led_off(LED2);
	led_off(LED3);
  led_off(LED4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    int sample_status = tof_regular_sample(&dev1);

    if (sample_status == 0) {
        // Distance is stored directly in millimeters
        uint16_t distance_mm = dev1.Distance;
        uint8_t range_status = dev1.RangeStatus; // 0 = valid measurement
        tft_prints(0, 6, "D: %d mm", distance_mm);
        tft_prints(0, 7, "RS: %d", range_status);

        // Use distance_mm for your robot / application logic here
    } else {
      tft_prints(0, 6, "Sample err: %d", sample_status);
      tft_prints(0, 7, "No valid range");
      dev1.Distance = 0;
    }

    HAL_Delay(120);
    if (btn_read(BTN2)) {
			led_on(LED2);
		} else {
			led_off(LED2);
		}
    
    if (btn_read(BTN1)) {
			led_on(LED3);
		} else {
			led_off(LED3);
		}
    led_on(LED1);
    led_on(LED4);
		if (tft_update(500) == 0) {
			tft_prints(0, 0, "Hello World"); // normal
			tft_prints(0, 1, "[Hello World]"); // This is a special text with differnt color
			tft_prints(0, 2, "{Hello World}");  // This is a higlighted text
			tft_prints(0, 3, "|Hello World|");  // This is a underlined text
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
