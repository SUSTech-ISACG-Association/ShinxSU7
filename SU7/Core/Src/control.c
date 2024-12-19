#include "control.h"
#include "motor.h"
#include "scene.h"
#include "sonic.h"
#include "message_buffer.h"

SU7State_t su7state = {{0, 0}, 0};
static int32_t dirx[] = {0, 1, 0, -1};
static int32_t diry[] = {-1, 0, 1, 0};
static uint32_t ld = 4;

static const int16_t max_spd = 90;

void rotDirection(const direction_t dir){
    float ang = Direction2float(dir) - Direction2float(su7state.heading);
    if (ang < 0) {
        MOTOR_SPINL(max_spd);
        HAL_Delay(ROT_1d_TIME*(-ang));
    } else {
        MOTOR_SPINR(max_spd);
        HAL_Delay(ROT_1d_TIME*ang);
    }
    MOTOR_STOP();
    su7state.heading = dir;
}
void goDirection(const direction_t dir){
    rotDirection(dir);
    MOTOR_FORWARD(max_spd);
    HAL_Delay(GO_1block_TIME);
    MOTOR_STOP();
    su7state.pos.x += dirx[dir];
    su7state.pos.y += diry[dir];
}

static Waypoint autoavoid_start, autoavoid_end;
static const uint32_t es_len = SCENE_COORDS_MAX_X*SCENE_COORDS_MAX_Y*4;
static Waypoint explore_stack[SCENE_COORDS_MAX_X*SCENE_COORDS_MAX_Y*4];
static uint32_t es_head;

// 0 for good
uint8_t es_push(const Waypoint a){
    if (es_head == es_len) return 0;
    explore_stack[es_head] = a;
    ++ es_head;
    return 1;
}

Waypoint es_get() {
    if(es_head != 0) {
        return explore_stack[es_head-1];
    } else {
        return (Waypoint){0,0};
    }
}

uint8_t es_pop(){
    if(es_head != 0) {
        --es_head;
        return 0;
    } else {
        return 1;
    }
}

void set_autovoid_position(const Waypoint st, const Waypoint en) {
    autoavoid_start = st;
    autoavoid_end = en;
    ShinxScene1.sceneMat[st.x][st.y] = SO_Source;
    ShinxScene1.sceneMat[en.x][en.y] = SO_Destination;
    es_head = 0;
    su7state = (SU7State_t){st, 0};
}

uint8_t check_valid_eq(const Waypoint pos, const SceneObject so){
    if (pos.x < 0 || pos.x >= SCENE_COORDS_MAX_X || pos.y < 0 || pos.y >= SCENE_COORDS_MAX_Y){
        return 0;
    }
    return Scene_get_object(&ShinxScene1, pos.x, pos.y) == so;
}
uint8_t check_valid_neq(const Waypoint pos, const SceneObject so){
    if (pos.x < 0 || pos.x >= SCENE_COORDS_MAX_X || pos.y < 0 || pos.y >= SCENE_COORDS_MAX_Y){
        return 0;
    }
    return Scene_get_object(&ShinxScene1, pos.x, pos.y) != so;
}

void save_goto(const Waypoint en){
    int32_t dx = en.x - su7state.pos.x;
    int32_t dy = en.y - su7state.pos.y;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    if (dx + dy == 0) return;
    else if (dx + dy == 1) {
        goDirection(GetDirection(su7state.pos, en));
        return;
    } else {
        static Waypoint explore_queue[SCENE_COORDS_MAX_X*SCENE_COORDS_MAX_Y*4];
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
        while(eq_head != eq_tail) {
            nx = explore_queue[eq_head];
            if (nx.x == su7state.pos.x && nx.y == su7state.pos.y) {
                goDirection(nxt[nx.x][nx.y]);
                return;
            }
            eq_head = (eq_head + 1) % es_len;
            for(uint32_t i=0;i<ld;++i){
                ne = (Waypoint){nx.x+dirx[i], nx.y+diry[i]};
                if(check_valid_neq(ne, SO_Obstacle) && check_valid_neq(ne, SO_Unkown) && vis[ne.x][ne.y] == 0) {
                    nxt[ne.x][ne.y] = ld^2;
                    explore_queue[eq_tail] = ne;
                    eq_tail = (eq_tail + 1) % es_len;
                    vis[ne.x][ne.y] = 1;
                }
            }
        }
    }
    return;
}

void autoavoid_update(){
    Waypoint nx;
    for(uint32_t i=0; i<ld; ++i) {
        nx = (Waypoint){su7state.pos.x+dirx[i],su7state.pos.y+diry[i]};
        if (check_valid_eq(nx, SO_Unkown)){
            rotDirection(ld);
            // su7state.heading = ld;
            float dis = SonicDetectF();
            if (SQUARE_LENGTH_CM/4 < dis && dis < SQUARE_LENGTH_CM * 1.5) {
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Obstacle);
                uint8_t nn = nx.y*4+nx.x;
                append_my_message(0x81, &nn, 1);
            } else {
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Empty);
                es_push(nx);
                // if (dis < SQUARE_LENGTH_CM * 2.5) {
                //     Scene_set_object(&ShinxScene1, nx.x + dirx[i], nx.y + dirx[i], SO_Obstacle);
                // } // TODO: an optimization
            }
        }
    }
    if (es_head == 0) {
        for(uint32_t i=0;i<SCENE_COORDS_MAX_X; ++i) {
            for(uint32_t j=0;j<SCENE_COORDS_MAX_Y; ++j) {
                if (Scene_get_object(&ShinxScene1, i, j) == SO_Unkown) {
                    for(uint32_t k=0;k<ld;++k) {
                        nx = (Waypoint){i+dirx[k],j+diry[k]};
                        if (check_valid_neq(nx, SO_Obstacle) && check_valid_neq(nx, SO_Unkown)){
                            es_push(nx);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (es_head == 0) {
        save_goto(autoavoid_end);
        end_mode();
        return;
    }
    nx = es_get();
    save_goto(nx);
    es_pop();
}

typedef enum {
    FOLLOWING,
    TURNL_WAITING,
    TURNL,
    TURNR_WAITING,
    TURNR
} RaceState_t;

static RaceState_t race_state = FOLLOWING;

void autorace_update(){
    RE_UPDATE:
    uint8_t now_stateL = HAL_GPIO_ReadPin(SEARCH_L_GPIO_Port, SEARCH_L_Pin) == GPIO_PIN_SET;
    uint8_t now_stateM = HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_SET;
    uint8_t now_stateR = HAL_GPIO_ReadPin(SEARCH_R_GPIO_Port, SEARCH_R_Pin) == GPIO_PIN_SET;
    uint8_t now_state = (now_stateL << 2) | (now_stateM << 1) | now_stateR;
    switch (now_state)
    {
    case 0b000:
        if (race_state == TURNL_WAITING || race_state == TURNL) {
            MOTOR_SPINL(max_spd);
            while(HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_RESET) ;
            goto RE_UPDATE;
        } else if (race_state == TURNR_WAITING || race_state == TURNR) {
            MOTOR_SPINR(max_spd);
            while(HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_RESET) ;
            goto RE_UPDATE;
        } else {
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
        } else if (race_state == TURNR) {
            race_state = TURNL;
        }
        break;
    
    default:
        break;
    }
    if (race_state == TURNL) {
        MOTOR_FORWARD_L(max_spd, max_spd);
    } else if (race_state == TURNR) {
        MOTOR_FORWARD_R(max_spd, max_spd);
    } else {
        MOTOR_FORWARD(max_spd);
    }
}

void control_init(){
    su7state = (SU7State_t){{0, 0}, 0};
    Scene_destroy(&ShinxScene1);
    Scene_init(&ShinxScene1);
}