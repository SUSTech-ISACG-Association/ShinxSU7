/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define V4 0

#include "24cxx.h"
#include "bluetooth.h"
#include "delay.h"
#include "gui.h"
#include "myiic.h"
#include "touch.h"
// #include "control.h"
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
UART_HandleTypeDef *huart;
uint8_t updated = 1;
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
    MX_TIM3_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */
    BACK_COLOR = BLACK;
    LCD_Init();
    LCD_Clear(BACK_COLOR);
    tp_dev.init();
    HAL_TIM_Base_Start_IT(&htim3);
    huart = &huart2;
    draw_manual(-1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // during while, send bluetooth communication
        if (need_greeting_flag) {
            float distance = get_distance();
            if (distance >= 0) { // negative distance means no obstacle detected, do not update
                char distance_buffer[50];
                sprintf(distance_buffer, "Dist=%.1fcm", distance);
                LCD_Fill_Window(10, 244, 100, 267, BACK_COLOR);
                POINT_COLOR = WHITE;
                LCD_ShowString(10, 244, 100, 12, 12, distance_buffer);
            }
            need_greeting_flag = 0;
        }
        tp_dev.scan(0);
        if ((tp_dev.sta & TP_PRES_DOWN) && (tp_dev.x[0] <= lcddev.width && tp_dev.y[0] <= lcddev.height)) {
            display_info("");
            if (LeiJun_mode == 0) {
                button_pressed = which_button_pressed_manual(&tp_dev);
                if (button_pressed) {
                    send_manual_inst();
                }
            }
            else if (LeiJun_mode == 1) {
                if (WITHIN_BUTTON(tp_dev.x[0], tp_dev.y[0], button_go)) {
                    // check if start, end, waypoint list are valid
                    updated = 1;
                    if (start != 0xff && end != 0xff && waypoint_cnt > 0 && waypoint_list[waypoint_cnt - 1] == end) {
                        if (is_running == 0) {
                            display_info("Path sent");
                            send_waypoint();
                            is_running = 2;
                        }
                        else if (is_running == 2) {
                            display_info("Running");
                            is_running = 1;
                            send_toggle_run();
                        }
                        else if (is_running == 1) {
                            display_info("Stopped");
                            is_running = 0;
                            send_toggle_run();
                        }
                    }
                    else {
                        display_info("Invalid waypoint");
                    }
                }
                else if (WITHIN_BUTTON(tp_dev.x[0], tp_dev.y[0], button_clear)) {
                    // clear the object(start/end/obstacle/waypoint) that current waypoint_state is setting
                    updated = 1;
                    if (waypoint_state == 0) // start
                    {
                        start = -1; // 0xff
                        waypoint_cnt = 0;
                    }
                    else if (waypoint_state == 1) // end
                    {
                        end = -1;
                        waypoint_cnt = 0;
                    }
                    else if (waypoint_state == 2) // obstacle
                    {
                        obstacles = 0x0000;
                    }
                    else // waypoint
                    {
                        waypoint_cnt = 1;
                    }
                }
                else {
                    button_pressed = which_button_pressed_map(&tp_dev);
                    if (button_pressed) {
                        updated = 1;
                        if (waypoint_state == 0) {
                            // set start, clear the waypoint list
                            for (uint8_t i = 0; i < 16; i++) {
                                if (button_pressed & (1 << i)) {
                                    if (obstacles & (1 << i)) {
                                        display_info("Start invalid");
                                    }
                                    else {
                                        start = i;
                                        waypoint_cnt = 1;
                                        waypoint_list[0] = i;
                                    }
                                    break;
                                }
                            }
                        }
                        else if (waypoint_state == 1) {
                            // set end, clear the waypoint list
                            for (uint8_t i = 0; i < 16; i++) {
                                if (button_pressed & (1 << i)) {
                                    if (obstacles & (1 << i)) {
                                        display_info("End invalid");
                                    }
                                    else {
                                        end = i;
                                        waypoint_cnt = 1;
                                    }
                                    break;
                                }
                            }
                        }
                        else if (waypoint_state == 2) {
                            // set obstacles, clear the waypoint list if new obstacle blocks the path
                            for (uint8_t i = 0; i < 16; i++) {
                                if (button_pressed & (1 << i) && start != i && end != i) {
                                    obstacles |= (1 << i);
                                    for (uint8_t j = 0; j < waypoint_cnt; j++) {
                                        if (waypoint_list[j] == i) {
                                            waypoint_cnt = 1;
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            // set waypoints
                            for (uint8_t i = 0; i < 16; i++) {
                                if (button_pressed & (1 << i)) {
                                    if (waypoint_cnt == 0) {
                                        display_info("Set start first");
                                    }
                                    else {
                                        if (obstacles & (1 << i)) {
                                            display_info("Path blocked");
                                        }
                                        else {
                                            uint8_t last = waypoint_list[waypoint_cnt - 1];
                                            if (last == i - 1 || last == i + 1 || last == i - 4 || last == i + 4) {
                                                waypoint_list[waypoint_cnt++] = i;
                                            }
											else
											{
												display_info("Not adjacent");
											}
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else {
                button_pressed = which_button_pressed_map(&tp_dev);
                if (WITHIN_BUTTON(tp_dev.x[0], tp_dev.y[0], button_mode)) {
                    updated = 1;
                    is_race = is_race ? 0 : 1;
                    if (is_running) {
                        is_running = 0;
                        send_toggle_run();
                    }
					char buf[20];
                    sprintf(buf, is_running ? "Running" : "Stopped");
                    display_info(buf);
                    send_set_mode(is_race ? 0x04 : 0x03);
                }
                else if (WITHIN_BUTTON(tp_dev.x[0], tp_dev.y[0], button_go)) {
                    updated = 1;
                    is_running = is_running ? 0 : 1;
                    char buf[20];
                    sprintf(buf, is_running ? "Running" : "Stopped");
                    display_info(buf);
                    send_toggle_run();
                }
                else {
                    for (int i = 0; i < 16; i++) {
                        if (button_pressed & (1 << i)) {
                            if (is_setting_start)
                                start = i;
                            else
                                end = i;
                            whereami = start;
                            updated = 1;
                            is_setting_start = is_setting_start ? 0 : 1;
                            if (0 <= start && start < 16 && 0 <= end && end <= start) {
                            }
                            break;
                        }
                    }
                }
            }
            if (updated) {
                drawUI();
                updated = 0;
            }
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

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
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
