#ifndef __CONTROL_H
#define __CONTROL_H

#include "scene.h"

extern SU7State_t su7state;

void goDirection(const direction_t dir, const float heading);

void explore();

#endif