#ifndef _DELAY_H_
#define _DELAY_H_

#include "main.h"

// Two way to count, not test which is better
void HAL_Delay_us(uint32_t us);
void HAL_Delay_us2(uint32_t nus);

#endif