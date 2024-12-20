#include "control.h"
#include "message_buffer.h"
#include "motor.h"
#include "scene.h"
#include "led.h"
#include "sonic.h"

SU7State_t su7state = {{0, 0}, 0};
static int32_t dirx[4] = {0, 1, 0, -1};
static int32_t diry[4] = {-1, 0, 1, 0};
static uint32_t ld = 4;

static const int16_t max_spd = 100;

void rotDirection(const direction_t dir)
{
    float ang = Direction2float(dir) - Direction2float(su7state.heading);
    if (ang > 180)
        ang -= 360;
    else if (ang < -180)
        ang += 360;
    if (ang < 0) {
        MOTOR_SPINL(max_spd);
        HAL_Delay(ROT_1d_TIME * (-ang));
    }
    else {
        MOTOR_SPINR(max_spd);
        HAL_Delay(ROT_1d_TIME * ang);
    }
    MOTOR_STOP();
    su7state.heading = dir;
}
void goDirection(const direction_t dir)
{
    rotDirection(dir);
    MOTOR_FORWARD(max_spd);
    HAL_Delay(GO_1block_TIME);
    MOTOR_STOP();
    su7state.pos.x += dirx[dir];
    su7state.pos.y += diry[dir];
}

static Waypoint autoavoid_start, autoavoid_end;
static const uint32_t es_len = SCENE_COORDS_MAX_X * SCENE_COORDS_MAX_Y * 4;
static Waypoint explore_stack[SCENE_COORDS_MAX_X * SCENE_COORDS_MAX_Y * 4];
static uint32_t es_head;

// 0 for good
uint8_t es_push(const Waypoint a)
{
    if (es_head == es_len)
        return 1;
    explore_stack[es_head] = a;
    ++es_head;
    return 0;
}

Waypoint es_get()
{
    if (es_head != 0) {
        return explore_stack[es_head - 1];
    }
    else {
        return (Waypoint){0, 0};
    }
}

uint8_t es_pop()
{
    if (es_head != 0) {
        --es_head;
        return 0;
    }
    else {
        return 1;
    }
}

void set_autovoid_position(const Waypoint st, const Waypoint en)
{
    autoavoid_start = st;
    autoavoid_end = en;
    Scene_set_object(&ShinxScene1, st.x, st.y, SO_Source);
    Scene_set_object(&ShinxScene1, en.x, en.y, SO_Destination);
    es_head = 0;
    su7state = (SU7State_t){st, 0};
}

uint8_t check_valid_eq(const Waypoint pos, const SceneObject so)
{
    if (pos.x < 0 || pos.x >= SCENE_COORDS_MAX_X || pos.y < 0 || pos.y >= SCENE_COORDS_MAX_Y) {
        return 0;
    }
    return Scene_get_object(&ShinxScene1, pos.x, pos.y) == so;
}
uint8_t check_valid_neq(const Waypoint pos, const SceneObject so)
{
    if (pos.x < 0 || pos.x >= SCENE_COORDS_MAX_X || pos.y < 0 || pos.y >= SCENE_COORDS_MAX_Y) {
        return 0;
    }
    return Scene_get_object(&ShinxScene1, pos.x, pos.y) != so;
}

void safe_goto(const Waypoint en)
{
    int32_t dx = en.x - su7state.pos.x;
    int32_t dy = en.y - su7state.pos.y;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    if (dx + dy == 0)
        return;
    else if (dx + dy == 1) {
        goDirection(GetDirection(su7state.pos, en));
        return;
    }
    else {
        static Waypoint explore_queue[SCENE_COORDS_MAX_X * SCENE_COORDS_MAX_Y * 4];
        static uint32_t eq_head, eq_tail;
        static uint8_t vis[SCENE_COORDS_MAX_X][SCENE_COORDS_MAX_Y];
        static direction_t nxt[SCENE_COORDS_MAX_X][SCENE_COORDS_MAX_Y];

        for (int32_t y = 0; y < SCENE_COORDS_MAX_Y; ++y)
            for (int32_t x = 0; x < SCENE_COORDS_MAX_X; ++x)
                vis[y][x] = 0;
        Waypoint nx, ne;
        eq_head = eq_tail = 0;
        explore_queue[eq_tail] = en;
        ++eq_tail;
        while (eq_head != eq_tail) {
            nx = explore_queue[eq_head];
            if (nx.x == su7state.pos.x && nx.y == su7state.pos.y) {
                goDirection(nxt[nx.x][nx.y]);
                return;
            }
            eq_head = (eq_head + 1) % es_len;
            for (uint32_t i = 0; i < ld; ++i) {
                ne = (Waypoint){nx.x + dirx[i], nx.y + diry[i]};
                if (check_valid_neq(ne, SO_Obstacle) && check_valid_neq(ne, SO_Unknown) && vis[ne.x][ne.y] == 0) {
                    nxt[ne.x][ne.y] = i ^ 2;
                    explore_queue[eq_tail] = ne;
                    eq_tail = (eq_tail + 1) % es_len;
                    vis[ne.x][ne.y] = 1;
                }
            }
        }
    }

    LED0_Write(0);
    LED1_Write(0);
    HAL_Delay(200);
    LED0_Write(1);
    LED1_Write(1);
    HAL_Delay(200);
    LED0_Write(0);
    LED1_Write(0);
    HAL_Delay(200);
    LED0_Write(1);
    LED1_Write(1);
    return;
}

uint8_t explore_dir(const direction_t dir)
{
    rotDirection(dir);
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = GO_1block_TIME;

    /* Add a freq to guarantee minimum wait */
    if (wait < HAL_MAX_DELAY) {
        wait += (uint32_t)(uwTickFreq);
    }

    uint8_t need_go_back = 0;
    MOTOR_FORWARD(max_spd);
    while ((HAL_GetTick() - tickstart) < wait) {
        if (HAL_GPIO_ReadPin(AVOID_LEFT_GPIO_Port, AVOID_LEFT_Pin) == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(AVOID_RIGHT_GPIO_Port, AVOID_RIGHT_Pin) == GPIO_PIN_RESET) {
            need_go_back = 1;
            break;
        }
        if (wait - (HAL_GetTick() - tickstart) > 250) {
            float dis = FastSonicDetect(3, 40);
            if (dis < 40) {
                need_go_back = 1;
                break;
            }
        }
    }
    MOTOR_STOP();

    if (need_go_back) {
        uint32_t tick = (HAL_GetTick() - tickstart);
        MOTOR_BACK(max_spd);
        HAL_Delay(tick);
        MOTOR_STOP();
        return 0;
    }
    else {
        su7state.pos = (Waypoint){su7state.pos.x + dirx[dir], su7state.pos.y + diry[dir]};
        return 1;
    }
}

void autoavoid_update()
{
    Waypoint nx;
    LED0_Write(0);
    LED1_Write(0);
    LED0_Write((su7state.pos.x & 2) >> 1);
    LED1_Write(su7state.pos.x & 1);
    HAL_Delay(1000);
    LED0_Write((su7state.pos.y & 2) >> 1);
    LED1_Write(su7state.pos.y & 1);
    HAL_Delay(1000);
    LED0_Write((su7state.heading & 2) >> 1);
    LED1_Write(su7state.heading & 1);
    HAL_Delay(1000);
    for (uint32_t i = 0; i < ld; ++i) {
        nx = (Waypoint){su7state.pos.x + dirx[i], su7state.pos.y + diry[i]};
        if (check_valid_eq(nx, SO_Unknown)) {
            LED0_Write((i & 2) >> 1);
            LED1_Write(i & 1);
            HAL_Delay(1000);
            if (explore_dir(i)) {

                LED0_Write(1);
                LED1_Write(0);
                // HAL_Delay(1000);
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Empty);
                es_push(nx);
                break;
            }
            else {
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Obstacle);
                uint8_t nn = nx.y * 4 + nx.x;
                append_my_message(0x81, &nn, 1);
            }
        }
    }
    LED0_Write(0);
    LED1_Write(1);
    if (es_head == 0) {
        for (int32_t i = 0; i < SCENE_COORDS_MAX_X && es_head == 0; ++i) {
            for (int32_t j = 0; j < SCENE_COORDS_MAX_Y && es_head == 0; ++j) {
                if (Scene_get_object(&ShinxScene1, i, j) == SO_Unknown) {
                    for (int32_t k = 0; k < ld; ++k) {
                        nx = (Waypoint){i + dirx[k], j + diry[k]};
                        if (check_valid_neq(nx, SO_Obstacle) && check_valid_neq(nx, SO_Unknown)) {
                            es_push(nx);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (es_head == 0) {
        safe_goto(autoavoid_end);
        end_mode();
        return;
    }

    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    nx = es_get();
    safe_goto(nx);
    es_pop();
}

typedef enum { FOLLOWING, TURNL_WAITING, TURNL, TURNR_WAITING, TURNR } RaceState_t;

static RaceState_t race_state = FOLLOWING;

void autorace_update()
{
RE_UPDATE:
    uint8_t now_stateL = HAL_GPIO_ReadPin(SEARCH_L_GPIO_Port, SEARCH_L_Pin) == GPIO_PIN_SET;
    uint8_t now_stateM = HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_SET;
    uint8_t now_stateR = HAL_GPIO_ReadPin(SEARCH_R_GPIO_Port, SEARCH_R_Pin) == GPIO_PIN_SET;
    uint8_t now_state = (now_stateL << 2) | (now_stateM << 1) | now_stateR;
    switch (now_state) {
    case 0b000:
        if (race_state == TURNL_WAITING || race_state == TURNL) {
            MOTOR_SPINL(max_spd - 20);
            while (HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_RESET)
                ;
            goto RE_UPDATE;
        }
        else if (race_state == TURNR_WAITING || race_state == TURNR) {
            MOTOR_SPINR(max_spd - 20);
            while (HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_RESET)
                ;
            goto RE_UPDATE;
        }
        else {
            race_state = FOLLOWING;
        }
        break;
    case 0b001:
        race_state = TURNR;
        break;
    case 0b010:
        // TODO: 回正
        race_state = FOLLOWING;
        break;
    case 0b011:
        race_state = TURNR_WAITING;
        break;
    case 0b100:
        race_state = TURNL;
        break;
    case 0b101:
        break;
    case 0b110:
        race_state = TURNL_WAITING;
        break;
    case 0b111:
        if (race_state == TURNL) {
            race_state = TURNR;
        }
        else if (race_state == TURNR) {
            race_state = TURNL;
        }
        break;

    default:
        break;
    }
    if (race_state == TURNL) {
        MOTOR_FORWARD_L(max_spd - 20, max_spd - 20);
    }
    else if (race_state == TURNR) {
        MOTOR_FORWARD_R(max_spd - 20, max_spd - 20);
    }
    else {
        MOTOR_FORWARD(max_spd);
    }
}

void control_init()
{
    race_state = FOLLOWING;
    es_head = 0;
}