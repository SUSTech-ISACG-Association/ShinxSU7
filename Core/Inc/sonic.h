#ifndef _SONIC_H_
#define _SONIC_H_

#include "main.h"

float SonicDetect(float angle);

#define SonicDetectF() SonicDetect(90)
#define SonicDetectL() SonicDetect(135)
#define SonicDetectR() SonicDetect(45)

#endif