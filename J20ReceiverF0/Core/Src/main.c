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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "nrf24L01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
u8 update_count = 0;
u8 PPM_count=0;
uint16_t CH_Safe[16];			//系统保存的失控保护通道值
uint32_t CH_PPM[9];				//该缓存区仅用于DMA搬运PPM，因此需要8个最后一个用于计算间隔。
u16 PWMvalue[16];// 控制PWM占空比
u16 recPWMvalue[SBUS_CHANNEL_NUMBER];// 控制PWM占空比
u8 sbusPacket[SBUS_PACKET_LENGTH];// 25个字节的SBUS数据包
u8 signalLoss = 0;  // 1表示丢失信号
u16 i=0,startIndex=0;
u32 nowTime = 0;
u32 lastTime = 0;
u32 adcTime = 0;
u8 chPacket[32];
u16 batAdcNum;//电压采样值
u8 batVolt;//电压值转换成一个字节，方便传输
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
HAL_StatusTypeDef My_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//重写fputc函数
int fputc(int c, FILE *stream)    
{
	HAL_UART_Transmit(&huart1, (unsigned char *)&c, 1, 1000);   
    return 1;
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
  MX_TIM2_Init();
  MX_ADC_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
	HAL_Delay(500);
	//启动后首先需要从EEPROM中读取配置信息，用于确定无线模块参数、运行模式、失控保护等。
	
	//配置无线模块
	NRF24L01_Init();
	//下面的值是方便调试写的，实际使用从EEPROM读取
	for(i = 0; i<16; i ++) {CH_Safe[i]=1500;}
	CH_Safe[2] = 1000;		//油门通道最低
	
	//初始各通道值，以失控保护值为准
	for(i = 0; i<16; i ++) {PWMvalue[i] = CH_Safe[i];}
	
	//PPM
	for(i = 0; i<8; i ++) {CH_PPM[i] = PWMvalue[i];}//PWM高电平时间
	CH_PPM[8] = 20000 - PWMvalue[0] - PWMvalue[1] - PWMvalue[2] - PWMvalue[3] - PWMvalue[4] - PWMvalue[5] - PWMvalue[6] - PWMvalue[7];
	My_HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, CH_PPM, 9);
	
	//PWM输出
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	PWM_SetDuty(&htim2, TIM_CHANNEL_1,PWMvalue[0]);
	PWM_SetDuty(&htim2, TIM_CHANNEL_2,PWMvalue[1]);
	PWM_SetDuty(&htim2, TIM_CHANNEL_3,PWMvalue[2]);
	PWM_SetDuty(&htim2, TIM_CHANNEL_4,PWMvalue[3]);
	PWM_SetDuty(&htim3, TIM_CHANNEL_1,PWMvalue[4]);
	PWM_SetDuty(&htim3, TIM_CHANNEL_2,PWMvalue[5]);
	PWM_SetDuty(&htim3, TIM_CHANNEL_3,PWMvalue[6]);
	
	HAL_TIM_Base_Start_IT(&htim17);//1ms定时器开启

	while(NRF24L01_Check())
	{
 		HAL_Delay(500);
		LED_BINK();//闪烁LED
	}
	NRF24L01_RX_Mode();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(NRF24L01_RxPacket(chPacket)==0)
	{
		for(i=0;i<32-chNum*2;i++)//寻找起始位置，一般在第2-3个元素
		{
			if(chPacket[i]==0x00 & chPacket[i+1]==0x00) 
			{
				startIndex = i+2;
				break;
			}
		}
		for(i=0;i<chNum;i++)//从起始位置开始处理
		{
			recPWMvalue[i] = ((u16)chPacket[startIndex] << 8) | ((u16)(chPacket[startIndex+1]));// 合并u8为u16
			if(recPWMvalue[i]!=0) PWMvalue[i] = recPWMvalue[i];
			startIndex = startIndex+2;
		}
		for (i=chNum; i<16; i++) 
		{
			PWMvalue[i] = 1500;//未用到的通道全部置中
		}
		LED_ON();
		lastTime = nowTime;
	}
	if(nowTime-lastTime>2000)//距离上次接收时间大于2s，则说明失去信号
	{
		LED_BINK();//闪烁LED
		signalLoss = 1;//失去信号标志
		PWMvalue[0] = 1500;//通道1置中
		PWMvalue[1] = 1500;//通道2置中
		PWMvalue[2] = 1000;//油门最低
		PWMvalue[3] = 1500;//通道4置中
		HAL_Delay(200);
	}
	if(nowTime-adcTime>1000)//1s转换一次电压
	{
		adcTime = nowTime;
		HAL_ADCEx_Calibration_Start(&hadc);//ADC校准
		HAL_ADC_Start(&hadc);//启动ADC转换
		HAL_ADC_PollForConversion(&hadc,50);//等待转换完成
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC))
		{
			batAdcNum = HAL_ADC_GetValue(&hadc);   //获取AD值
			batVolt = batAdcNum>>4;//右移四位，采样值0-255
		}
		HAL_ADC_Stop(&hadc);//停止转换
	}
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//定时器17用作系统定时器，运行频率为1000Hz，提供系统心跳1ms一次
	if (htim->Instance == htim17.Instance)
	{
		nowTime ++;//对系统计数器进行累加
		
		update_count++;
		if(update_count == 100) {update_count = 0;}
		//根据读取无线模块的通道数据输出PWM，10ms更新一次。
		if(update_count % 10 == 0)
		{
			PWM_SetDuty(&htim2, TIM_CHANNEL_1,PWMvalue[0]);
			PWM_SetDuty(&htim2, TIM_CHANNEL_2,PWMvalue[1]);
			PWM_SetDuty(&htim2, TIM_CHANNEL_3,PWMvalue[2]);
			PWM_SetDuty(&htim2, TIM_CHANNEL_4,PWMvalue[3]);
			PWM_SetDuty(&htim3, TIM_CHANNEL_1,PWMvalue[4]);
			PWM_SetDuty(&htim3, TIM_CHANNEL_2,PWMvalue[5]);
			PWM_SetDuty(&htim3, TIM_CHANNEL_3,PWMvalue[6]);
		}
		if(update_count % SBUS_UPDATE_RATE == 0)
		{
			//输出S.bus，10ms更新一次
			sbusPreparePacket(sbusPacket, PWMvalue, signalLoss, 0); //chNum通道数值转换为SBUS数据包
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)sbusPacket, sizeof(sbusPacket));
		}
		
		//按键应该在按下时记录当前 Sys_count ，并且计算出下一个检查值，
		//并根据情况在Sys_count 到达检查值时进行判断处理。
		/*if((Sys_count == check_value) && (Key_Status == Key_Mode))…………*/
	}
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	for(uint8_t i = 0; i != 8; i ++) {CH_PPM[i] = PWMvalue[i];}
	CH_PPM[8] = 20000 - PWMvalue[0] - PWMvalue[1] - PWMvalue[2] - PWMvalue[3] - PWMvalue[4] - PWMvalue[5] - PWMvalue[6] - PWMvalue[7];
}

//改变原有库函数，将DMA目标寄存器改为ARR。
HAL_StatusTypeDef My_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  uint32_t tmpsmcr;

  /* Check the parameters */
  assert_param(IS_TIM_CCX_INSTANCE(htim->Instance, Channel));

  if (htim->State == HAL_TIM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (htim->State == HAL_TIM_STATE_READY)
  {
    if ((pData == NULL) && (Length > 0U))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  else
  {
    /* nothing to do */
  }

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->ARR, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }

      /* Enable the TIM Capture/Compare 1 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
      break;
    }

    case TIM_CHANNEL_2:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC2]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Capture/Compare 2 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
      break;
    }

    case TIM_CHANNEL_3:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC3]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Output Capture/Compare 3 request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
      break;
    }

    case TIM_CHANNEL_4:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC4]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)pData, (uint32_t)&htim->Instance->CCR4, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Capture/Compare 4 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
      break;
    }

    default:
      break;
  }

  /* Enable the Capture compare channel */
  TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);

  if (IS_TIM_BREAK_INSTANCE(htim->Instance) != RESET)
  {
    /* Enable the main output */
    __HAL_TIM_MOE_ENABLE(htim);
  }

  /* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

  /* Return function status */
  return HAL_OK;
}

/*PPM输出*/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim1.Instance)
	{
		TIM1->ARR = CH_PPM[PPM_count];
		PPM_count ++;
		if(PPM_count >= 8)
		{
			PPM_count = 0;
			//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			//HAL_TIM_Base_Stop(&htim1);
		}
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
