#ifndef SCENE_H
#define SCENE_H

#include "memory.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include <math.h>

// Peripheral Configurations
#define ULTRASONIC_UNIT_CM

// Scene Configurations
#define SCENE_N_SQUARE_ROW 4
#define SCENE_N_SQUARE_COLUMN 4
#define SQUARE_LENGTH_CM 80

#define TIME_SCALE_SPD_90 1.09
// #define ROT_1d_TIME (TIME_SCALE_SPD_90 * 4.25)    // 1530/360
// #define GO_1block_TIME (TIME_SCALE_SPD_90 * 1700) // 80cm: 1.7s
// #define GO_12cm_TIME (TIME_SCALE_SPD_90 * 255)    // half length of su7
#define ROT_1d_TIME 4.15f    // 1530/360
#define GO_1block_TIME 1500 // 80cm: 1.5s
#define GO_12cm_TIME 255    // half length of su7

#define SCENE_COORDS_MAX_Y 4
#define SCENE_COORDS_MAX_X 4

typedef struct {
    int32_t x;
    int32_t y;
} Point2i;

typedef Point2i Waypoint;

// Waypoint Vector; Do not operate on any of the members directly;
// Use provided methods instead
typedef struct {
    Waypoint *arr; // Underlying storage
    size_t length;
    size_t _allocatedLength;
} WaypointVector;

typedef enum {
    SO_Unknown = 0,
    SO_Source = 1,
    SO_Destination = 2,
    SO_Empty = 3,
    SO_Obstacle = 255,
} SceneObject;

typedef struct {
    SceneObject sceneMat[SCENE_COORDS_MAX_X][SCENE_COORDS_MAX_Y]; // Scene object matrix
    WaypointVector waypoints;
} Scene;

extern Scene ShinxScene1;

void Error_Handler(); // Defined in main.h

// WaypointVector
void WaypointVector_init(WaypointVector *v);

void WaypointVector_reserve(WaypointVector *v, size_t newAllocatedLength);

void WaypointVector_pushback(WaypointVector *v, Waypoint w);

Waypoint WaypointVector_pop(WaypointVector *v);

void WaypointVector_clear(WaypointVector *v);

void WaypointVector_destroy(WaypointVector *v);

void Scene_init(Scene *scene);

void Scene_destroy(Scene *scene);

void Scene_add_waypoint(Scene *scene, Waypoint p);

Waypoint Scene_pop_waypoint(Scene *scene);

void Scene_set_object(Scene *scene, int32_t x, int32_t y, SceneObject obj);
SceneObject Scene_get_object(Scene *scene, int32_t x, int32_t y);

typedef enum {
    Y_NEGATIVE = 0,
    X_POSITIVE = 1,
    Y_POSITIVE = 2,
    X_NEGATIVE = 3,
} direction_t;

typedef struct {
    Point2i pos;
    direction_t heading;
} SU7State_t;

direction_t GetDirection(const Waypoint a, const Waypoint b);

float Direction2float(const direction_t dir);

#endif // SCENE_H