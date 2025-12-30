/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE   //必须定义file，不然GPIO灯不会亮，如果只用到串口不用到GPIO的话就不用定义FILE
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int axial_speed , axial_time , radial_speed, radial_time;
int flag1, flag2, flag3, flag4;
int counter_2=0,counter_3,counter_4,counter_5=0;
int axialdata_effective=0,radialdata_effective=0;
int input_effect=0;
int axial_timeout = 0,radial_timeout=0;
int tim5_cnt=0,tim2_cnt=0;
int axial_head_exti_flag=0,axial_tail_exti_flag=0,radial_left_exti_flag=0,radial_right_exti_flag=0;
int flag_head_restrict=0,flag_tail_restrict=0,flag_left_restrict=0,flag_right_restrict=0;

// flag1表示前进(0)或后退(1)，flag2表示左转(0)或右转(1).
// flag3表示轴向运动对应时间的定时器计数值乘子，flag4表示径向运动对应时间的定时器计数值乘子。
// 轴向运动时红色LED亮，径向运动时绿色LED亮。
// counter_2，counter_5决定定时器定时次数，timx_cnt作为定时次数标志位
// axialdata_effective是检测输入轴向数据是否有效的标志位，径向同理
// radialdata_effective检测输入径向数据是否有效
// input_effect检测输入数据格式是否正确
// timeout检测是否到达定时时长，若到达，PWM波停止，定时停止
// exti_flag检测外部中断的到来
// flag_restrict检测是否在限位中，若在限位中，不应继续朝对应方向前进

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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, RxBuffer, RXBUFFERSIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	if (input_effect == 1)
	{
		printf("轴向速度为%d\n轴向时间%d\n径向速度为%d\n径向时间为%d\nflag1为%d\nflag2为%d\nflag3为%d\nflag4为%d\n",axial_speed , axial_time , radial_speed, radial_time,flag1,flag2,flag3,flag4);
		input_effect = 0;
	}
	
	if (input_effect== -1)
	{
		printf("输入格式不正确！\n");
		input_effect = 0;
	}
	/*
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET)
	{
		axial_timeout = 1;
		axial_head_exti_flag = 1;
		printf("已到达轴向运动前限位！\n");
	}
	
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_SET)
	{
		axial_timeout = 1;
		axial_tail_exti_flag = 1;
		printf("已到达轴向运动后限位！\n");
	}
	
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_SET)
	{
		radial_timeout = 1;
		radial_left_exti_flag = 1;
		printf("c");
	}
	
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_SET)
	{
		radial_timeout = 1;
		radial_right_exti_flag = 1;
		printf("d");
	}*/
	
	if(radial_timeout) //径向运动停止
	{
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
		radial_timeout = 0;
		printf("径向运动停止！\n");
	}
	
	if(axial_timeout) //轴向运动停止
	{
		HAL_TIM_Base_Stop_IT(&htim5);
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
		axial_timeout = 0;
		printf("轴向运动停止！\n");
	}
	
	/*if(axial_head_exti_flag||axial_tail_exti_flag)
		printf("已到达轴向运动限位！");
	if(radial_left_exti_flag||radial_right_exti_flag)
		printf("已到达径向运动限位！");
	*/
	/*
	if (axialdata_effective)
		
		printf("counter_3为%d,counter_5为%d",counter_3,counter_5);
	
	if (radialdata_effective)
		
		printf("counter_4为%d,counter_2为%d",counter_4,counter_2);
	*/
	
	/*flag_head_restrict = flag1 == 0 && axial_head_exti_flag;  //来的数据为轴向正转(1)，现在状态为前进限位(1)，则对前进不做反应
	flag_tail_restrict = flag1 && axial_tail_exti_flag;
	flag_left_restrict = flag2 == 0 && radial_left_exti_flag;
	flag_right_restrict = flag2 && radial_right_exti_flag;*/

	if (axialdata_effective && axial_timeout == 0) 
	// 轴向运动的PWM通道无输出，且控制运动时间的定时器也准备就绪，即已经完成了上一次的运动操作
	// 接收到了有效的轴向运动数据
	// 且不在限位+对应运动方向上
	{
		if (flag1) 
		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);  //设置PC4高电平，即轴向电机反转
		else
		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);  //设置PC4低电平，即轴向电机正转	
		
		htim3.Instance->ARR = counter_3-1;
		// __HAL_TIM_SetCounter 要注意，这个宏设置的是当前计数值，不是最大计数值
		htim3.Instance->CCR1 = (counter_3-1)/2;
		
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		//htim5.Instance->ARR = counter_5-1;
		HAL_TIM_Base_Start_IT(&htim5);
		
		axialdata_effective = 0;
		axial_head_exti_flag = 0;
		axial_tail_exti_flag = 0;
		
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);
	}
	
	if ( radialdata_effective && radial_timeout == 0)
	// 径向运动的PWM通道无输出，且控制运动时间的定时器也准备就绪，即已经完成了上一次的运动操作
	// 且接收到了有效的轴向运动数据
	// 且不在限位+对应运动方向上
	{
		if (flag2)
		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);  //设置PC3低电平，即径向电机反转
		else
		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);  //设置PC3高电平，即径向电机正转
		
		htim4.Instance->ARR = counter_4-1;
		htim4.Instance->CCR1 = (counter_4-1)/2;
		
		HAL_TIM_PWM_Start(&htim4 ,TIM_CHANNEL_1);
		
		//htim2.Instance->ARR = counter_2-1;
		HAL_TIM_Base_Start_IT(&htim2);
		
		radialdata_effective = 0;
		radial_left_exti_flag = 0;
		radial_right_exti_flag = 0;
		
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);
	}
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	uint8_t head = '9';
	uint8_t verti = '0';
	if (huart == &huart1 && RxBuffer[0] == head)
  {
	 input_effect=1;
	  
	 char axial_speed_string[4];
	 char axial_time_string[5];
	 char radial_speed_string[4];
	 char radial_time_string[5];
	
	  
	 memcpy(axial_speed_string,(const char*)RxBuffer+1,3);
	 memcpy(axial_time_string,(const char*)RxBuffer+4,4);
	 memcpy(radial_speed_string,(const char*)RxBuffer+8,3);
	 memcpy(radial_time_string,(const char*)RxBuffer+11,4);
	  
	 axial_speed_string[3]= '\0';
	 axial_time_string[4] = '\0';
	 radial_speed_string[3] = '\0';
	 radial_time_string[4] = '\0';

	 axial_speed = atoi(axial_speed_string);
	 axial_time = atoi(axial_time_string);
	 radial_speed = atoi(radial_speed_string);
	 radial_time = atoi(radial_time_string);
	 
	 flag1 = RxBuffer[15]- verti;
	 flag2 = RxBuffer[16]- verti;
	 flag3 = RxBuffer[17]- verti;
	 flag4 = RxBuffer[18]- verti;
	 
	 
	 if (flag3 != 9)
		{
			axialdata_effective = 1;
			counter_3 = 20000 / axial_speed;
			counter_5 = axial_time * pow(10,flag3);
		}	
		
	 if (flag4 != 9)
		{
			radialdata_effective = 1;
			counter_4 = 9000 / radial_speed;  
			counter_2 = radial_time * (pow(10,flag4));
		}
		
	 //while(HAL_UART_Receive_IT(&huart1, RxBuffer, RXBUFFERSIZE) != HAL_OK);
	 //memset(RxBuffer, 0, sizeof(RxBuffer));
	 //__HAL_UART_FLUSH_DRREGISTER(&huart1);
	 HAL_UART_Receive_IT(&huart1, (uint8_t*)RxBuffer, RXBUFFERSIZE);
  }
  else
	  input_effect = -1; //表示传入数据头不是'9'，需要重新传递数据
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim5)
	{
		tim5_cnt++;
		
		if(tim5_cnt == counter_5)
		{
			tim5_cnt = 0;
			axial_timeout = 1;
		}
	}
	if(htim == &htim2)
	{
		tim2_cnt++;
		
		if(tim2_cnt == counter_2)
		{
			tim2_cnt = 0;
			radial_timeout = 1;
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
