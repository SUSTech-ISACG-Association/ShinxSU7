/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "delay.h"
#include "bluetooth.h"
#include "control.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t RmtSta=0;
uint16_t Dval;
uint32_t RmtRec=0;
uint8_t RmtCnt=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
extern TIM_HandleTypeDef htim2;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(KEY3_Pin);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SONIC_WAVE_RECV_Pin);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(KEY1_Pin);
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(KEY2_Pin);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/* USER CODE BEGIN 1 */
float UltrasonicWave_Distance; // in centimeter(maybe)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  switch (GPIO_Pin)
  {
  case KEY1_Pin:
    HAL_Delay(50);
    if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET){
      set_autopilot_position((Waypoint){0, 0}, (Waypoint){3, 3});
      set_auto_avoid_mode();
      toggle_mode();
    }
    while(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
      ;
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
    break;
  case KEY2_Pin:
    HAL_Delay(50);
    if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET){
      set_auto_race_mode();
      toggle_mode();
    }
    while(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
      ;
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    break;
  case KEY3_Pin:
    HAL_Delay(50);
    if(HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET){
      runInitialCalibration();
    }
    while(HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET){
    }
    break;
  
  case SONIC_WAVE_RECV_Pin:
    HAL_Delay_us(10); // wait sonic module send sonic pulse
    __HAL_TIM_SetCounter(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
    uint32_t cnt = 0;
    while((__HAL_TIM_GetCounter(&htim2) < 4900)){
      if (HAL_GPIO_ReadPin(SONIC_WAVE_RECV_GPIO_Port, SONIC_WAVE_RECV_Pin)) {
        cnt = __HAL_TIM_GetCounter(&htim2);
      } else if (__HAL_TIM_GetCounter(&htim2) - cnt >= 10) {
        break;
      }
    }
    HAL_TIM_Base_Stop(&htim2);
    UltrasonicWave_Distance = cnt * 340 / 200.0;
  default:
    break;
  }
}

/* RmtSta:
  [8:8] is receiving(lead in) 0x80
  [7:7] is reveived  0x40
  [5:5] rising/falling capture 0x10
  [4:0] timer for timeout
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)	// 判断是定时器5�?�生中断
  {
    if(RmtSta & 0x80){
      RmtSta &= ~0x10;
      if((RmtSta & 0x0f) == 0x00) RmtSta |= 0x40;
      if((RmtSta & 0x0f) < 0x0e){
        ++RmtSta;
      } else {
        RmtSta &= ~0x80;
        RmtSta &= 0xf0;
      }
    }
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)	// 判断是定时器5�?�生中断
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)  // Make sure this is for CC2
    {
      if((RmtSta & 0x10) == 0){
        RmtSta |= 0x10;
        __HAL_TIM_DISABLE(&htim5);
        __HAL_TIM_SetCounter(&htim5, 0);
        TIM_RESET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_2);
        TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_2, TIM_ICPOLARITY_FALLING);
        __HAL_TIM_ENABLE(&htim5);
      } else {
        // Read the capture value for CC2 (similar to Dval in Keil)
        Dval = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);  // Capture value of TIM5_CH2

        // Handle the received data
        if (RmtSta & 0x80)  // If we've received the lead-in code
        {
          if (Dval > 300 && Dval < 800)  // 560us standard for '0'
          {
            RmtRec <<= 1;
            RmtRec |= 0;
          }
          else if (Dval > 1400 && Dval < 1900)  // 1680us standard for '1'
          {
            RmtRec <<= 1;
            RmtRec |= 1;
          }
          else if (Dval > 2200 && Dval < 2700)  // 2500us standard for key press signal
          {
            RmtCnt++;  // Increment the key press count
            RmtSta &= 0xF0;  // Reset the timer
          }
        }
        else if (Dval > 4200 && Dval < 4700)  // 4500us standard for lead-in code
        {
          RmtSta |= 0x80;  // Mark successful reception of lead-in code
          RmtCnt = 0;  // Reset the key press counter
        }
        
        // Clear the rising edge capture flag
        RmtSta &= ~0x10;
        TIM_RESET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_2);
        TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_2, TIM_ICPOLARITY_RISING);
      }
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if (huart->Instance == USART2) {
    bluetooth_RxCallback();
  }
}
/* USER CODE END 1 */
