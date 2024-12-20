#ifndef LED_H
#define LED_H

#define LED0_Write(x) \
   HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, (x) ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define LED1_Write(x) \
   HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (x) ? GPIO_PIN_RESET : GPIO_PIN_SET)

#endif // LED_H