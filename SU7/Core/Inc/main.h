/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
  CONTROL_MODE,
  WAYPOINT_MODE,
  AUTO_AVOID_MODE,
  AUTO_RACE_MODE
} SU7Mode_t;
extern SU7Mode_t su7mode;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint8_t set_control_mode();
uint8_t set_waypoint_mode();
uint8_t set_auto_avoid_mode();
uint8_t set_auto_race_mode();
void start_mode();
void end_mode();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SEARCH_R_Pin GPIO_PIN_13
#define SEARCH_R_GPIO_Port GPIOC
#define SONIC_WAVE_SEND_Pin GPIO_PIN_0
#define SONIC_WAVE_SEND_GPIO_Port GPIOC
#define SONIC_WAVE_RECV_Pin GPIO_PIN_1
#define SONIC_WAVE_RECV_GPIO_Port GPIOC
#define SONIC_WAVE_RECV_EXTI_IRQn EXTI1_IRQn
#define KEY3_Pin GPIO_PIN_0
#define KEY3_GPIO_Port GPIOA
#define KEY3_EXTI_IRQn EXTI0_IRQn
#define INFRA_RECV_Pin GPIO_PIN_1
#define INFRA_RECV_GPIO_Port GPIOA
#define SONIC_MOTOR_Pin GPIO_PIN_6
#define SONIC_MOTOR_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_4
#define KEY1_GPIO_Port GPIOC
#define KEY1_EXTI_IRQn EXTI4_IRQn
#define KEY2_Pin GPIO_PIN_5
#define KEY2_GPIO_Port GPIOC
#define KEY2_EXTI_IRQn EXTI9_5_IRQn
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOB
#define LEFT1_MOTOR_FORWARD_Pin GPIO_PIN_12
#define LEFT1_MOTOR_FORWARD_GPIO_Port GPIOB
#define LEFT2_MOTOR_FORWARD_Pin GPIO_PIN_13
#define LEFT2_MOTOR_FORWARD_GPIO_Port GPIOB
#define RIGHT1_MOTOR_FORWARD_Pin GPIO_PIN_14
#define RIGHT1_MOTOR_FORWARD_GPIO_Port GPIOB
#define RIGHT2_MOTOR_FORWARD_Pin GPIO_PIN_15
#define RIGHT2_MOTOR_FORWARD_GPIO_Port GPIOB
#define SEARCH_L_Pin GPIO_PIN_11
#define SEARCH_L_GPIO_Port GPIOC
#define SEARCH_M_Pin GPIO_PIN_12
#define SEARCH_M_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD
#define AVOID_RIGHT_Pin GPIO_PIN_4
#define AVOID_RIGHT_GPIO_Port GPIOB
#define AVOID_LEFT_Pin GPIO_PIN_5
#define AVOID_LEFT_GPIO_Port GPIOB
#define LEFT1_MOTOR_PWM_Pin GPIO_PIN_6
#define LEFT1_MOTOR_PWM_GPIO_Port GPIOB
#define LEFT2_MOTOR_PWM_Pin GPIO_PIN_7
#define LEFT2_MOTOR_PWM_GPIO_Port GPIOB
#define RIGHT1_MOTOR_PWM_Pin GPIO_PIN_8
#define RIGHT1_MOTOR_PWM_GPIO_Port GPIOB
#define RIGHT2_MOTOR_PWM_Pin GPIO_PIN_9
#define RIGHT2_MOTOR_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
