/* USER CODE BEGIN Header */
  /**
    ******************************************************************************
    * @file           : main.c
    * @brief          : Main program body
    ******************************************************************************
    * @attention
    *
    * Copyright (c) 2025 STMicroelectronics.
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
#include "dma.h"
#include "memorymap.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
  #include <stdio.h>
  #include <stdlib.h>

  #include "lcd.h"
  #include "lcd_init.h"
  #include "myfft.h"
  #include "fft_phase.h"
  #include "fft_window.h"
  #include "fft_disp.h"
  #include "myfir.h"
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
  #define FFT_LENGTH 8192
  #define ADC_RANGE  4095
  //数字量


  char str1[100];
  char str2[50];
  char str3[50];
  char str4[50];
  char str5[50];
  char str6[50];
  char str7[50];
  char str8[50];

  int flag_adcdone;

  uint32_t time_idx;
  float time_float ;
  float fft_input_temp[FFT_LENGTH*2];
  float fft_output_temp[FFT_LENGTH]; //用于存储数组，便于debug直接查看
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
  int fputc(int ch, FILE *f)
  {
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&ch,1);
    return ch;
  }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC2_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

    HAL_Delay(200);
    LCD_ST7789V3_Init();
    HAL_ADCEx_Calibration_Start(&hadc2,LL_ADC_CALIB_LINEARITY,ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc1,LL_ADC_CALIB_LINEARITY,ADC_SINGLE_ENDED);
    HAL_Delay(200);

    FFT_Handler* FFT_Handle = FFT_Handler_Init(FFT_LENGTH);
    FFT_Handler* FFT_Handle2 = FFT_Handler_Init(FFT_LENGTH);
    FFT_Handle->adc_rate = 100000;
    FFT_Handle2->adc_rate = 100000;
    HAL_TIM_Base_Start(&htim6); //TIM6同步触发两个ADC进行采样
    HAL_TIM_Base_Start_IT(&htim3); //TIM5 用于做时间定时


    HAL_ADC_Start_DMA(&hadc2,FFT_Handle->adc_buf,FFT_LENGTH);
    HAL_ADC_Start_DMA(&hadc1,FFT_Handle2->adc_buf,FFT_LENGTH);
    LCD_Fill(0,0,320,240,BLACK);
    //FFT Init

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {

    /**************************************FFT计算部分**********************************************/
        while (flag_adcdone != 1){};         //必须等待数组全部更新完毕 在进入
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_ADC_Stop_DMA(&hadc2);
        HAL_TIM_Base_Stop(&htim6);

        for (int i = 0;i<FFT_LENGTH;i++)
        {
          FFT_Handle->adc_val[i] = (float)FFT_Handle->adc_buf[i] /ADC_RANGE * 3.3;
          FFT_Handle2->adc_val[i] = (float)FFT_Handle2->adc_buf[i] /ADC_RANGE * 3.3;
        }
        fir_calculate(FFT_Handle);

        //加窗运算
        window_calculate(FFT_Handle->adc_val,FFT_LENGTH,1);
        window_calculate(FFT_Handle2->adc_val,FFT_LENGTH,1);
        //不放中断，害怕中断更改val数据

        LCD_ShowString(30,20,"hello world",WHITE,BLACK,32,0);
        fft_calculate(FFT_Handle);
        fft_calculate(FFT_Handle2);
        float phase = fft_calculate_phase(FFT_Handle,FFT_Handle2);

        float fft_fv = (float)FFT_Handle->fft_fv;
        for (int i = 0;i<FFT_LENGTH;i++)
        {
          fft_output_temp[i] = FFT_Handle->FFT_OutputBuf[i];
        }

        for (int i = 0;i<FFT_LENGTH*2;i++)
        {
          fft_input_temp[i] = FFT_Handle->FFT_InputBuf[i];
        }
      /************************************实验性代码,FFT_OVER2*************************************/



      /************************************显示&&串口发送*******************************************/
        for (int i =0; i < FFT_LENGTH; i++)
        {
          sprintf(str1,"val:%3f,%3f,%3f\r\n",FFT_Handle->adc_val[i],(float)i/1000,fft_output_temp[i]);
          HAL_UART_Transmit_DMA(&huart1,str1,sizeof(str1));
        }
        sprintf(str3, "Freq: %.1f Hz", FFT_Handle->fft_fv);
        LCD_ShowString(30, 50, str3 ,WHITE, BLACK, 16, 0);
        // 显示电压有效值
        sprintf(str4, "Vpp: %.5f V", FFT_Handle->fft_vpp);
        LCD_ShowString(30, 70, str4, WHITE, BLACK, 16, 0);

        sprintf(str5,"RMS: %.5f v",FFT_Handle->fft_rms);
        LCD_ShowString(30, 90, str5, WHITE, BLACK, 16, 0);

        sprintf(str6, "Phase: %.5f deg",phase);
        LCD_ShowString(30, 110, str6, WHITE, BLACK, 16, 0);

        fft_freq_disp(FFT_Handle,0.1);

        HAL_Delay(1000);
        LCD_Fill(0,0,320,240,BLACK);
        flag_adcdone = 0;


    /**********************重新启动ADC******************************/
      HAL_TIM_Base_Start(&htim6);
      HAL_ADC_Start_DMA(&hadc1,FFT_Handle->adc_buf,FFT_LENGTH);
      HAL_ADC_Start_DMA(&hadc2,FFT_Handle2->adc_buf,FFT_LENGTH);




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */





      /*********************************FFT计算部分**************************************/




      /*********************************屏幕显示部分**************************************/
      //fft_freq_disp(FFT_Handle,300);

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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 10;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
  //回调函数，采样完毕一次后更新数据
  void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hdac2)
  {
    flag_adcdone = 1;
  }

  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
  {

  }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
  {
    if (GPIO_Pin == BTN1_Pin)
    {

      __HAL_GPIO_EXTI_CLEAR_IT(BTN1_Pin);
    }
  }
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
