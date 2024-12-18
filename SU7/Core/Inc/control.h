#ifndef __CONTROL_H
#define __CONTROL_H

#include "scene.h"

extern SU7State_t su7state;

void rotDirection(const direction_t dir);
void goDirection(const direction_t dir);
void set_autovoid_position(const Waypoint st, const Waypoint en);
void autoavoid_update();
void autorace_update();

void control_init();

#endif