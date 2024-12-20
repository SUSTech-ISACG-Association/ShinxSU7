#ifndef _SONIC_H_
#define _SONIC_H_

#include "main.h"

float SonicDetect(float angle);

#define SonicDetectF() SonicDetect(90)
#define SonicDetectL() SonicDetect(135)
#define SonicDetectR() SonicDetect(45)

float FastSonicDetect(uint32_t times, uint32_t max_val);

#endif