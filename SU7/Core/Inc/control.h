#ifndef __CONTROL_H
#define __CONTROL_H

#include "scene.h"

extern SU7State_t su7state;

void runInitialCalibration();
void calibrateAndRotDir(const direction_t dir);
void calibrateAndGoDir(const direction_t dir);
void set_autovoid_position(const Waypoint st, const Waypoint en);
void autoavoid_update();
void autorace_update();
void safe_goto(const Waypoint en);

void control_init();

#endif