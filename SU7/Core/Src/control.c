#include "control.h"
#include "delay.h"
#include "led.h"
#include "limits.h"
#include "math.h"
#include "message_buffer.h"
#include "motor.h"
#include "scene.h"
#include "sonic.h"
#include "songs.h"

SU7State_t su7state = {{0, 0}, 0};
static int32_t dirx[4] = {0, 1, 0, -1};
static int32_t diry[4] = {-1, 0, 1, 0};
static uint32_t ld = 4;

static const int16_t max_spd = 100;

// State used while calibrating the orientation
typedef enum {
    CalibState_Entering,
    CalibState_SpinL,
    CalibState_SpinR,
    CalibState_PassingThrough,
    CalibState_Exiting,
    CalibState_Complete,
} CalibState_t;

static inline uint8_t __calibReadSearchStateOnce()
{
    uint8_t now_stateL = HAL_GPIO_ReadPin(SEARCH_L_GPIO_Port, SEARCH_L_Pin) == GPIO_PIN_SET;
    uint8_t now_stateM = HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_SET;
    uint8_t now_stateR = HAL_GPIO_ReadPin(SEARCH_R_GPIO_Port, SEARCH_R_Pin) == GPIO_PIN_SET;
    uint8_t now_state = (now_stateL << 2) | (now_stateM << 1) | now_stateR;
    return now_state;
}

static inline uint8_t calibrateReadSearchState()
{
    uint8_t s1 = __calibReadSearchStateOnce();
    HAL_Delay_us(10);
    uint8_t s2 = __calibReadSearchStateOnce();
    HAL_Delay_us(20);
    uint8_t s3 = __calibReadSearchStateOnce();
    return s1 & s2 & s3;
}

#define CALIB_STEP_DELAY_MS 300
#define CALIB_TIME_DECAY_COEFF 0.75f
#define CALIB_RETREAT_MIN_TIME 100
#define CALIB_SPIN_MIN_TIME 80
#define CALIB_INIT_HBLK_TIME 850
#define CALIB_HBLK_ENTER_OFFSET 175
#define CALIB_ROT_OFFSET 1.00e-4f
#define CALIB_ROT_FLOAT_EPS 1e-4f
#define CALIB_CONSECUTIVE_CHANGE_DECAY 0.75f
#define CALIB_FUNC_OPS_DELAY_MS 10
#define CALIB_FUNC_LOOP_CNT 2

// Calibrated half-block time
uint32_t calibratedHBlkTime = CALIB_INIT_HBLK_TIME;
// Calibrated 90-degree rotation time
float calibrated1DegRotTime = ROT_1d_TIME;
float calibrateLastSpinDeg = 0.0f;
// To prevent multiple rotations on the same spot causes
// inaccurate calibration
uint8_t calibrateHasSteppedForward = 0;
int8_t lastSpinOri = 0; // Last spin orientation

// extern uint32_t ctEnEl;
// extern uint32_t ctExEl;

// Spin until SU7 faces right toward the next square
// i.e., the current orientation makes a 90 degree angle
// relative to the borderline of the current square
void calibrateOneStepForward()
{
    HAL_Delay(CALIB_STEP_DELAY_MS);
    // We make the assumption that the first time we
    // come across a border, our orientation must be
    // 90 degrees to that borderline

    // First set to forward and record the startTick of forward
    // which will later be used to calibrate the speed
    uint32_t enterStartTick = HAL_GetTick();
    uint32_t enterEndTick = UINT32_MAX;
    uint32_t exitStartTick = 0;
    uint32_t exitEndTick = UINT32_MAX;

    LED0_Write(0);
    LED1_Write(0);
    MOTOR_FORWARD(max_spd);

    // Set to 1 if spinned to left, 2 if to right
    // Used to calibrate rotation
    int8_t firstSpin = 0;

    CalibState_t state = CalibState_Entering;

    while (state != CalibState_Complete) {
        // HAL_Delay(5);
        uint8_t searchState = calibrateReadSearchState();

        switch (searchState) {
        case 0b000:
            // Continue
            if (state != CalibState_PassingThrough && state != CalibState_Exiting && state != CalibState_SpinL &&
                state != CalibState_SpinR)
                state = CalibState_Entering;
            else
                state = CalibState_Exiting;
            break;
        case 0b111:
            // Passing the borderline and do nothing
            state = CalibState_PassingThrough;
            break;
        case 0b100:
        case 0b110:
            if (state == CalibState_Entering)
                state = CalibState_SpinL;
            break;
        case 0b001:
        case 0b011:
            if (state == CalibState_Entering)
                state = CalibState_SpinR;
            break;
        case 0b010:
        case 0b101:
            // Skip
            break;
        }

        if (state == CalibState_Exiting && exitStartTick == 0) {
            enterEndTick = HAL_GetTick() + CALIB_HBLK_ENTER_OFFSET;
            exitStartTick = HAL_GetTick();
        }

        switch (state) {
        case CalibState_Entering:
            MOTOR_FORWARD(max_spd);
            // LED0_Write(0);
            // LED1_Write(0);
            break;
        case CalibState_PassingThrough:
            MOTOR_FORWARD(max_spd);
            // LED0_Write(1);
            // LED1_Write(0);
            break;
        case CalibState_Exiting:
            exitEndTick = HAL_GetTick();
            if (exitEndTick - exitStartTick > calibratedHBlkTime) {
                state = CalibState_Complete;
                MOTOR_STOP();
                // LED0_Write(1);
                // LED1_Write(1);
            }
            else {
                MOTOR_FORWARD(max_spd);
                // LED0_Write(0);
                // LED1_Write(1);
            }
            break;
        case CalibState_SpinL:
            if (!firstSpin)
                firstSpin = 1;
            // MOTOR_BACK(max_spd);
            // HAL_Delay(CALIB_RETREAT_MIN_TIME);
            // MOTOR_SPINL(max_spd - 20);
            MOTOR_SET_SPD(-(max_spd - 30), (max_spd - 20));
            // LED0_Write(1);
            // LED1_Write(0);
            HAL_Delay(CALIB_SPIN_MIN_TIME);
            break;
        case CalibState_SpinR:
            if (!firstSpin)
                firstSpin = 2;
            // MOTOR_BACK(max_spd);
            // HAL_Delay(CALIB_RETREAT_MIN_TIME);
            // MOTOR_SPINR(max_spd - 20);
            MOTOR_SET_SPD((max_spd - 20), -(max_spd - 30));
            // LED0_Write(0);
            // LED1_Write(1);
            HAL_Delay(CALIB_SPIN_MIN_TIME);
            break;
        case CalibState_Complete:
            // LED0_Write(0);
            // LED1_Write(0);
            MOTOR_STOP();
            break;
        }
    }

#ifdef CALIBRATE_ENABLE
    // Update current half-block-required time based on #ticks elapsed on entering
    uint32_t tickElapsed = enterEndTick - enterStartTick;
    calibratedHBlkTime =
        (uint32_t)(tickElapsed * (1 - CALIB_TIME_DECAY_COEFF) + calibratedHBlkTime * CALIB_TIME_DECAY_COEFF);

    // uint32_t enterTickElapsed = enterEndTick - enterStartTick;
    // uint32_t exitTickElapsed = exitEndTick - exitStartTick;
    // append_my_message(0x82, (uint8_t *)&enterTickElapsed, sizeof(uint32_t));
    // append_my_message(0x82, (uint8_t *)&exitTickElapsed, sizeof(uint32_t));
    // ctEnEl = enterTickElapsed;
    // ctExEl = exitTickElapsed;
    if (fabsf(calibrateLastSpinDeg) > CALIB_ROT_FLOAT_EPS && firstSpin != 0) {
        // Calibrate rotation
        int8_t sgn = firstSpin == 1 ? -1 : 1;
        int8_t rotOffsetSgn = sgn * (calibrateLastSpinDeg > 0.0f ? 1 : -1);
        float consecutiveChangeDecay = (rotOffsetSgn == lastSpinOri) ? 1.0f : CALIB_CONSECUTIVE_CHANGE_DECAY;
        lastSpinOri = rotOffsetSgn;

        float newC90DegRotTime = calibrated1DegRotTime + sgn * calibrateLastSpinDeg * CALIB_ROT_OFFSET * consecutiveChangeDecay;
        calibrated1DegRotTime = newC90DegRotTime;

        // Reset calibrateLastSpinDeg
        calibrateLastSpinDeg = 0.0f;
    }
    calibrateHasSteppedForward = 1;
#endif
}

void calibrateAndRotDir(const direction_t dir)
{
    HAL_Delay(CALIB_STEP_DELAY_MS);
    float deg = Direction2float(dir) - Direction2float(su7state.heading);
    if (deg > 180)
        deg -= 360;
    else if (deg < -180)
        deg += 360;
    if (calibrateHasSteppedForward == 0) {
        calibrateLastSpinDeg += deg;
    }
    else {
        calibrateLastSpinDeg = deg;
        calibrateHasSteppedForward = 0;
    }
    if (deg < 0) {
        MOTOR_SPINL(max_spd);
        HAL_Delay(calibrated1DegRotTime * (-deg));
    }
    else {
        MOTOR_SPINR(max_spd);
        HAL_Delay(calibrated1DegRotTime * deg);
    }
    MOTOR_STOP();
    su7state.heading = dir;
}

void calibrateAndGoDir(const direction_t dir)
{
    calibrateAndRotDir(dir);
    calibrateOneStepForward();
    su7state.pos.x += dirx[dir];
    su7state.pos.y += diry[dir];
}

void runInitialCalibration()
{
    LED0_Write(0);
    LED1_Write(0);
    HAL_Delay(150);
    LED0_Write(1);
    LED1_Write(1);
    HAL_Delay(150);
    LED0_Write(0);
    LED1_Write(0);

    // !2x2 calibration
    for (int32_t i = 0; i < CALIB_FUNC_LOOP_CNT; ++i) {
        calibrateAndGoDir(Y_POSITIVE);
        calibrateAndGoDir(X_POSITIVE);
        calibrateAndGoDir(Y_POSITIVE);
        calibrateAndGoDir(X_NEGATIVE);
        calibrateAndGoDir(Y_NEGATIVE);
        calibrateAndGoDir(X_POSITIVE);
        calibrateAndGoDir(Y_NEGATIVE);
        calibrateAndGoDir(X_NEGATIVE);
    }

    // !3x3 calibration
    // calibrateAndGoDir(X_POSITIVE);
    // calibrateAndGoDir(X_POSITIVE);
    // calibrateAndGoDir(Y_POSITIVE);
    // calibrateAndGoDir(X_NEGATIVE);
    // calibrateAndGoDir(X_NEGATIVE);
    // calibrateAndGoDir(Y_POSITIVE);
    // calibrateAndGoDir(X_POSITIVE);
    // calibrateAndGoDir(X_POSITIVE);
    // calibrateAndGoDir(X_NEGATIVE);
    // calibrateAndGoDir(X_NEGATIVE);
    // calibrateAndGoDir(Y_NEGATIVE);
    // calibrateAndGoDir(Y_NEGATIVE);

    // !4x4 calibration
    // calibrateAndGoDir(Y_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(Y_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(Y_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(Y_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(Y_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_POSITIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(Y_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndGoDir(X_NEGATIVE);
    // HAL_Delay(CALIB_FUNC_OPS_DELAY_MS);
    // calibrateAndRotDir(Y_NEGATIVE);

    LED0_Write(1);
    LED1_Write(1);
    HAL_Delay(150);
    LED0_Write(0);
    LED1_Write(0);
    HAL_Delay(150);
    LED0_Write(1);
    LED1_Write(1);
    HAL_Delay(150);
    LED0_Write(0);
    LED1_Write(0);
    HAL_Delay(150);
    LED0_Write(1);
    LED1_Write(1);
    HAL_Delay(150);
    LED0_Write(0);
    LED1_Write(0);
}

// ============= END OF BASIC UTILITIES ===============

static Waypoint autopilot_start, autopilot_end;
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

void set_autopilot_position(const Waypoint st, const Waypoint en)
{
    autopilot_start = st;
    autopilot_end = en;
    Scene_set_object(&ShinxScene1, st.x, st.y, SO_Source);
    Scene_set_object(&ShinxScene1, en.x, en.y, SO_Destination);
    es_head = 0;
    su7state = (SU7State_t){st, Y_NEGATIVE};
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

void save_su7_position()
{
    uint8_t pp = (su7state.heading << 4) | (su7state.pos.y << 2) | (su7state.pos.x);
    append_my_message(0x90, &pp, 1);
}

void safe_goto(const Waypoint en)
{
    static Waypoint explore_queue[SCENE_COORDS_MAX_X * SCENE_COORDS_MAX_Y * 4];
    static uint32_t eq_head, eq_tail;
    static uint8_t vis[SCENE_COORDS_MAX_X + 5][SCENE_COORDS_MAX_Y + 5];
    static direction_t nxt[SCENE_COORDS_MAX_X + 5][SCENE_COORDS_MAX_Y + 5];

    int32_t dx = en.x - su7state.pos.x;
    int32_t dy = en.y - su7state.pos.y;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    if (dx + dy == 0) {
        return;
    }
    else if (dx + dy == 1) {
        calibrateAndGoDir(GetDirection(su7state.pos, en));
        save_su7_position();
        return;
    }
    else {
        for (int32_t x = 0; x < SCENE_COORDS_MAX_X; ++x)
            for (int32_t y = 0; y < SCENE_COORDS_MAX_Y; ++y)
                vis[x][y] = 0;
        Waypoint nx, ne;
        eq_head = eq_tail = 0;
        explore_queue[eq_tail] = en;
        ++eq_tail;
        vis[en.x][en.y] = 1;
        while (eq_head != eq_tail) {
            nx = explore_queue[eq_head];
            if (nx.x == su7state.pos.x && nx.y == su7state.pos.y) {
                while (su7state.pos.x != en.x || su7state.pos.y != en.y) {
                    calibrateAndGoDir(nxt[su7state.pos.x][su7state.pos.y]);
                    save_su7_position();
                }
                return;
            }
            eq_head = (eq_head + 1) % es_len;
            for (uint32_t i = 0; i < ld; ++i) {
                ne = (Waypoint){nx.x - dirx[i], nx.y - diry[i]};
                if (check_valid_neq(ne, SO_Obstacle) && check_valid_neq(ne, SO_Unknown) && vis[ne.x][ne.y] == 0) {
                    nxt[ne.x][ne.y] = i;
                    explore_queue[eq_tail] = ne;
                    eq_tail = (eq_tail + 1) % es_len;
                    vis[ne.x][ne.y] = 1;
                }
            }
        }
    }
    // warning: no path can be found
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

#define EXPLORE_SPIN_MIN_TIME 100

uint8_t explore_dir(const direction_t dir)
{
    calibrateAndRotDir(dir);
    uint32_t tickStart = HAL_GetTick();
    uint32_t wait = calibratedHBlkTime * 2;

    /* Add a freq to guarantee minimum wait */
    if (wait < HAL_MAX_DELAY) {
        wait += (uint32_t)(uwTickFreq);
    }

    uint8_t need_go_back = 0;
    MOTOR_FORWARD(max_spd);
    while ((HAL_GetTick() - tickStart) < wait) {
        // Calibration test
        uint8_t searchState = calibrateReadSearchState();

        switch (searchState) {
        case 0b100:
        case 0b110:
            MOTOR_SET_SPD(-(max_spd - 30), (max_spd - 20));
            HAL_Delay(EXPLORE_SPIN_MIN_TIME);
            MOTOR_FORWARD(max_spd);
            wait = (HAL_GetTick() - tickStart) + calibratedHBlkTime;
            break;
        case 0b001:
        case 0b011:
            MOTOR_SET_SPD((max_spd - 20), -(max_spd - 30));
            HAL_Delay(EXPLORE_SPIN_MIN_TIME);
            MOTOR_FORWARD(max_spd);
            wait = (HAL_GetTick() - tickStart) + calibratedHBlkTime;
            break;
        case 0b000:
        case 0b010:
        case 0b101:
        case 0b111:
            // Skip
            break;
        }

        // Infrared obstruction test
        if (HAL_GPIO_ReadPin(AVOID_LEFT_GPIO_Port, AVOID_LEFT_Pin) == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(AVOID_RIGHT_GPIO_Port, AVOID_RIGHT_Pin) == GPIO_PIN_RESET) {
            need_go_back = 1;
            break;
        }
        // Ultrasonic obstruction test
        if (wait - (HAL_GetTick() - tickStart) > 250) {
#ifdef SONIC_NOT_WORKING
            float dis = FastSonicDetect(2, 60);
            if (dis < 60) {
#else
            float dis = FastSonicDetect(2, SQUARE_LENGTH_CM);
            if (dis < SQUARE_LENGTH_CM) {
#endif
                need_go_back = 1;
                break;
            }
        }
    }
    MOTOR_STOP();

    if (need_go_back) {
        uint32_t tick = (HAL_GetTick() - tickStart);
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

void add_obstacle(const Waypoint nx)
{
    Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Obstacle);
    uint8_t nn = nx.y * 4 + nx.x;
    append_my_message(0x81, &nn, 1);
    bark(261.6, 500);
}

void autopilot_update()
{
    Waypoint nx;
    for (uint32_t i = 0; i < ld; ++i) {
        nx = (Waypoint){su7state.pos.x + dirx[i], su7state.pos.y + diry[i]};
        if (check_valid_eq(nx, SO_Unknown)) {
#ifdef SONIC_NOT_WORKING
            if (explore_dir(i)) {
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Empty);
                es_push(nx);
                break;
            }
            else {
                add_obstacle(nx);
            }
#else
            calibrateAndRotDir(i);
            // float dis = FastSonicDetect(2, 200);
            float dis1 = SonicDetect(90);
            float dis2 = SonicDetect(105);
            float dis3 = SonicDetect(80);
            float dis = dis1 < dis2 ? dis1 : dis2;
            dis = dis < dis3 ? dis : dis3;
            if (SQUARE_LENGTH_CM / 4 < dis && dis < SQUARE_LENGTH_CM * 1.5) {
                add_obstacle(nx);
            }
            else {
                Scene_set_object(&ShinxScene1, nx.x, nx.y, SO_Empty);
                es_push(nx);
                // if (dis < SQUARE_LENGTH_CM * 2.5) {
                //     nx = (Waypoint){nx.x + dirx[i], nx.y + diry[i]};
                //     if (check_valid_eq(nx, SO_Unknown)) {
                //         add_obstacle(nx);
                //     }
                // } // TODO: an optimization
                // calibrateAndGoDir(i);
            }
#endif
        }
    }
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
        safe_goto(autopilot_end);
        play_song(HARUHIKAGE_id);
        end_mode();
        return;
    }
    nx = es_get();
    safe_goto(nx);
    es_pop();
}

typedef enum { FOLLOWING, TURNL_WAITING, TURNL, TURNR_WAITING, TURNR } RaceState_t;

static RaceState_t race_state = FOLLOWING;

void autorace_update()
{
RE_UPDATE:
    HAL_Delay(5);
    uint8_t now_state = calibrateReadSearchState();
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
    case 0b110:
        race_state = TURNL_WAITING;
        break;
    case 0b111:
    case 0b101:
        // if (race_state == TURNL) {
        //     race_state = TURNR;
        // }
        // else if (race_state == TURNR) {
        //     race_state = TURNL;
        // }
        // MOTOR_BACK(max_spd);
        // HAL_Delay(100);
        // race_state = FOLLOWING;
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
    for(int32_t i = 0; i < SCENE_COORDS_MAX_X; ++i)
        for(int32_t j = 0; j < SCENE_COORDS_MAX_Y; ++j)
            Scene_set_object(&ShinxScene1, i, j, SO_Unknown);
    Scene_set_object(&ShinxScene1, autopilot_start.x, autopilot_start.y, SO_Source);
    Scene_set_object(&ShinxScene1, autopilot_end.x, autopilot_end.y, SO_Destination);
    race_state = FOLLOWING;
    es_head = 0;
}