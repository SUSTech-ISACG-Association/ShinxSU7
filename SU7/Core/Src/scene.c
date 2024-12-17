#include "scene.h"

Scene ShinxScene1;

void Scene_init(Scene *scene)
{
    for (int32_t y = 0; y < SCENE_COORDS_MAX_Y; ++y)
        for (int32_t x = 0; x < SCENE_COORDS_MAX_X; ++x)
            scene->sceneMat[y][x] = SO_Empty;
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

Waypoint Scene_pop_waypoint(Scene *scene)
{
    return WaypointVector_pop(&scene->waypoints);
}

void Scene_set_object(Scene *scene, int32_t y, int32_t x, SceneObject obj)
{
    scene->sceneMat[y][x] = obj;
    return;
}