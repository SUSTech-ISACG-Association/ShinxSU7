#ifndef SCENE_H
#define SCENE_H

#include "memory.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

// Peripheral Configurations
#define ULTRASONIC_UNIT_CM

// Scene Configurations
#define SCENE_N_SQUARE_ROW 4
#define SCENE_N_SQUARE_COLUMN 4
#define SQUARE_LENGTH_CM 80

#define SCENE_COORDS_MAX_Y 4
#define SCENE_COORDS_MAX_X 4

typedef struct {
    int32_t y;
    int32_t x;
} Point2i;

typedef Point2i Waypoint;

// Waypoint Vector; Do not operator on any of the members directly;
// Use provided methods instead
typedef struct {
    Waypoint *arr; // Underlying storage
    size_t length;
    size_t _allocatedLength;
} WaypointVector;

typedef enum
{
    SO_Empty = 0,
    SO_Source = 1,
    SO_Destination = 2,
    SO_Obstacle = 255,
} SceneObject;

typedef struct {
    SceneObject sceneMat[SCENE_COORDS_MAX_Y][SCENE_COORDS_MAX_X]; // Scene object matrix
    WaypointVector waypoints;
} Scene;

void Error_Handler(); // Defined in main.h

// WaypointVector
static inline void WaypointVector_init(WaypointVector *v)
{
    v->arr = NULL;
    v->length = 0UL;
    v->_allocatedLength = 0UL;
    return;
}

static inline void WaypointVector_reallocated(WaypointVector *v, size_t newAllocatedLength)
{
    Waypoint *newStorage = NULL;
    if ((newStorage = (Waypoint *)realloc(v->arr, newAllocatedLength * sizeof(Waypoint))) == NULL)
        Error_Handler();
    v->_allocatedLength = newAllocatedLength;
    return;
}

static inline void WaypointVector_pushback(WaypointVector *v, Waypoint w)
{
    if (v->length >= v->_allocatedLength)
        WaypointVector_reallocated(v, v->length + 1); // Should be *2?

    v->arr[v->length] = w;
    ++v->length;
    return;
}

static inline Waypoint WaypointVector_pop(WaypointVector *v)
{
    if (v->length == 0)
        Error_Handler();
    return v->arr[--v->length];
}

static inline void WaypointVector_clear(WaypointVector *v)
{
    v->length = 0UL;
    return;
}

static inline void WaypointVector_destroy(WaypointVector *v)
{
    free(v->arr);
    v->arr = NULL;
    v->length = 0UL;
    v->_allocatedLength = 0UL;
    return;
}

void Scene_init(Scene *scene);

void Scene_add_waypoint(Scene *scene, Waypoint p);

Waypoint Scene_pop_waypoint(Scene *scene);

void Scene_set_object(Scene *scene, int32_t y, int32_t x, SceneObject obj);

#endif // SCENE_H