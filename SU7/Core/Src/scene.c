#include "scene.h"

Scene ShinxScene1;

void Scene_init(Scene *scene)
{
    for (int32_t x = 0; x < SCENE_COORDS_MAX_X; ++x)
        for (int32_t y = 0; y < SCENE_COORDS_MAX_Y; ++y)
            scene->sceneMat[x][y] = SO_Unknown;
    WaypointVector_init(&scene->waypoints);
    return;
}

void Scene_destroy(Scene *scene)
{
    WaypointVector_destroy(&scene->waypoints);
    return;
}

void Scene_add_waypoint(Scene *scene, Waypoint p)
{
    WaypointVector_pushback(&scene->waypoints, p);
    return;
}

Waypoint Scene_pop_waypoint(Scene *scene) { return WaypointVector_pop(&scene->waypoints); }

void Scene_set_object(Scene *scene, int32_t x, int32_t y, SceneObject obj)
{
    scene->sceneMat[x][y] = obj;
    return;
}

SceneObject Scene_get_object(Scene *scene, int32_t x, int32_t y) { return scene->sceneMat[x][y]; }

void WaypointVector_init(WaypointVector *v)
{
    v->length = 0UL;
    v->_allocatedLength = 10UL;
    v->arr = (Waypoint *)malloc(v->_allocatedLength * sizeof(Waypoint));
    if (v->arr != NULL)
        return;
    v->_allocatedLength = 1UL;
    v->arr = (Waypoint *)malloc(v->_allocatedLength * sizeof(Waypoint));
    if (v->arr != NULL)
        return;
    Error_Handler();
}

void WaypointVector_reserve(WaypointVector *v, size_t newAllocatedLength)
{
    Waypoint *newStorage = NULL;
    if ((newStorage = (Waypoint *)realloc(v->arr, newAllocatedLength * sizeof(Waypoint))) != NULL) {
        v->arr = newStorage;
        v->_allocatedLength = newAllocatedLength;
        return;
    }
    Error_Handler();
}

void WaypointVector_defaultExtend(WaypointVector *v)
{
    Waypoint *newStorage = NULL;
    size_t newAllocatedLength = v->_allocatedLength * 2;
    if (newAllocatedLength >= v->_allocatedLength) {
        if ((newStorage = (Waypoint *)realloc(v->arr, newAllocatedLength * sizeof(Waypoint))) != NULL) {
            v->arr = newStorage;
            v->_allocatedLength = newAllocatedLength;
            return;
        }
    }
    newAllocatedLength = v->_allocatedLength + 1;
    if ((newStorage = (Waypoint *)realloc(v->arr, newAllocatedLength * sizeof(Waypoint))) != NULL) {
        v->arr = newStorage;
        v->_allocatedLength = newAllocatedLength;
        return;
    }
    Error_Handler();
}

void WaypointVector_pushback(WaypointVector *v, Waypoint w)
{
    if (v->length >= v->_allocatedLength)
        WaypointVector_defaultExtend(v); // Should be *2?

    v->arr[v->length] = w;
    ++v->length;
    return;
}

Waypoint WaypointVector_pop(WaypointVector *v)
{
    if (v->length == 0)
        Error_Handler();
    return v->arr[--v->length];
}

void WaypointVector_clear(WaypointVector *v)
{
    v->length = 0UL;
    return;
}

void WaypointVector_destroy(WaypointVector *v)
{
    free(v->arr);
    v->arr = NULL;
    v->length = 0UL;
    v->_allocatedLength = 0UL;
    return;
}

direction_t GetDirection(const Waypoint a, const Waypoint b)
{
    if (a.x < b.x)
        return X_POSITIVE;
    else if (a.x > b.x)
        return X_NEGATIVE;
    else if (a.y < b.y)
        return Y_POSITIVE;
    else
        return Y_NEGATIVE;
}

float Direction2float(const direction_t dir) { return dir * 90; }