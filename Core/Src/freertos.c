/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "adc.h"
#include "T5UIC1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
  Sta1=0,
  Sta2,
  Sta3
} My_State;
typedef struct {
  uint16_t cp_vol;
  My_State cp_sta;
} My_St;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId TaskHandle;
osThreadId LCDHandle;
osThreadId AdcTaskHandle;
osMessageQId myQueue01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask(void const * argument);
void LCDTask(void const * argument);
void StartAdc03(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task */
  osThreadDef(Task, StartTask, osPriorityNormal, 0, 128);
  TaskHandle = osThreadCreate(osThread(Task), NULL);

  /* definition and creation of LCD */
  osThreadDef(LCD, LCDTask, osPriorityLow, 0, 256);
  LCDHandle = osThreadCreate(osThread(LCD), NULL);

  /* definition and creation of AdcTask */
  osThreadDef(AdcTask, StartAdc03, osPriorityLow, 0, 128);
  AdcTaskHandle = osThreadCreate(osThread(AdcTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTask */
/**
  * @brief  Function implementing the Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask */
void StartTask(void const * argument)
{
  /* USER CODE BEGIN StartTask */

  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartTask */
}

/* USER CODE BEGIN Header_LCDTask */
/**
* @brief Function implementing the LCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCDTask */
void LCDTask(void const * argument)
{
  /* USER CODE BEGIN LCDTask */
  DWINLCD_Init(USART3);
  DWINLCD_Clear(White);
	DWINLCD_ShowXChar(1, 1, A64, Black, White, 30, 50, "Vol:");
  osEvent event;
  /* Infinite loop */
  for(;;)
  {
  // DWINLCD_ShowXChar(1, 1, 0x05, Black, White, 30, 210, "hello");
  event=osMessageGet(myQueue01Handle,osWaitForever);
	My_St *re;	
	if(osEventMessage==event.status)
	{ 
    re=event.value.p;
    osThreadSuspendAll();
    printf("sta:%d\r\n",re->cp_sta);
    printf("%d\r\n",re->cp_vol);
    DWINLCD_ShowXNum(1, 1, 1, 0, A64, Black, White, 1, 3, 125,50,re->cp_vol);
    osThreadResumeAll();
    // switch (pData->cp_ad_status)
    // {
    // case H_AD_STATUS_0:
    //   printf("H_AD_STATUS_0\r\n");
    //   break;
    // case H_AD_STATUS_9:
    //   printf("H_AD_STATUS_9\r\n");
    //   break;

    // default:
    //   break;
    // }
	}
	else
	{
		printf("error:0x%d\n",event.status);
	}

 
  // osDelay(1000);
  }
  /* USER CODE END LCDTask */
}

/* USER CODE BEGIN Header_StartAdc03 */
/**
* @brief Function implementing the AdcTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAdc03 */
void StartAdc03(void const * argument)
{
  /* USER CODE BEGIN StartAdc03 */
  
  HAL_ADCEx_Calibration_Start(&hadc1);    //ADC内部校准
  HAL_ADC_Start(&hadc1);                  //ADC开启转换
  osEvent xReturn;
  uint16_t ADC_V =0;

  /* Infinite loop */
  for(;;)
  {
    uint16_t ADC_num = HAL_ADC_GetValue(&hadc1);    //获取ADC端口数据
    My_St adc_st;
    ADC_V = 3300*ADC_num/4096;  
    adc_st.cp_vol=ADC_V;
    adc_st.cp_sta=Sta3;
    xReturn.status=osMessagePut(myQueue01Handle,(uint32_t)&adc_st,0);	
		// if(osOK!=xReturn.status)
		// {
		// 	printf("adc send fail\n");
		// }
  
  }
  /* USER CODE END StartAdc03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

