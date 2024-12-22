#ifndef __CONTROL_H
#define __CONTROL_H

#ifndef CALIBRATE_ENABLE
#define CALIBRATE_ENABLE
#endif

#include "scene.h"

extern SU7State_t su7state;

void runInitialCalibration();
void calibrateAndRotDir(const direction_t dir);
void calibrateAndGoDir(const direction_t dir);
void set_autopilot_position(const Waypoint st, const Waypoint en);
void autopilot_update();
void autorace_update();
void safe_goto(const Waypoint en);

void control_init();

#endif