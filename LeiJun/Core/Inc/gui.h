#include "lcd.h"
#include "objects.h"

#define BUTTON_EDGE_COLOR WHITE
#define BUTTON_BACK_COLOR BLACK
#define BUTTON_TEXT_COLOR WHITE
#define INVERT_RGB565(color)                                                                                           \
    ((((color) ^ 0xF800) & 0xF800) | (((color) ^ 0x07E0) & 0x07E0) | (((color) ^ 0x001F) & 0x001F))

/**
 * @brief draw a button with text on it
 *
 * @param sx upper left x position
 * @param sy upper left y position
 * @param ex lower right x position
 * @param ey lower right y position
 * @param text_size sizeof text
 * @param text text to be displayed
 * @param button button to be updated
 */
void draw_button_text(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t text_size, const char *text,
                      button_area *button);

void draw_button_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t color, button_area *button);

void draw_button_image(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t *color, button_area *button);

void draw_waypoint_objects();

/**
 * @brief Draw UI for manual control mode
 *
 * 9 buttons: turns*2, spin*2, direction*4, stop
 * display float distance to the obstacle ahead
 *
 * @param distance distance to the obstacle ahead
 */
void draw_manual(float distance);

/**
 * @brief Draw UI for waypoint control mode
 *
 * 4 * 4 map grids = 16 buttons
 * a button to switch between modes setting different elements:
 * 4 states: set_start, set_end, set_obstacles, set_waypoints
 */
void draw_waypoint();

/**
 * @brief Draw UI for auto control modes: sweeping and racing
 *
 * 2 buttons: start_sweep, start_race
 */
void draw_auto();