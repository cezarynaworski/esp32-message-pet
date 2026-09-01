#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "animation/animation.h"
#include "fonts/font.h"
#include "menu.h"
#include "sdkconfig.h"
#include "sprites/sprite.h"
#include "telegram_client.h"
#include <stdbool.h>
#include <stdint.h>
#include "app.h"
#define LCD_WIDTH 128
#define LCD_HEIGHT 160

typedef enum {
  DRAW_OK = 0,
  DRAW_ERR_WORD_TOO_LONG = 1,
  DRAW_ERR_NO_GLYPH = 2,
  DRAW_ERR_TEXT_TOO_LONG = 3

} DrawStatus;

// variable that keeps current frame. We send it with display_flush func from
// display_manager
extern uint16_t framebuffer[LCD_WIDTH * LCD_HEIGHT];

void draw_pixel_landscape(int x, int y, uint16_t color);
void draw_pixel_portrait(int x, int y, uint16_t color);
// void draw_pixel(int x, int y, uint16_t color);
void draw_rect(int x, int y, int w, int h, bool rounded, uint16_t color);

void draw_circle(int x0, int y0, int r, uint16_t color);
void clear_background(uint16_t color);

void draw_char(int x, int y, const Glyph *glyph, const Font *font,
               uint16_t color);

DrawStatus draw_text(int x, int y, const char *text, const Font *font,
                     uint16_t color, uint8_t max_lines,
                     uint16_t max_line_x_position, const char **remaining_text);

void draw_sprite(int x, int y, const Sprite *sprite, bool mirror_horizontal);
bool render_animation(Animation *anim);
bool render_menu(Menu *menu, bool new_message);
void render_messages(TelegramMessage *message, uint8_t msg_left);
bool render_hud(AppState *app_state, bool animation_changed);

#if CONFIG_SCREEN_ROTATION == 0
#define draw_pixel draw_pixel_portrait
#define MAX_WIDTH LCD_WIDTH
#else
#define draw_pixel draw_pixel_landscape
#define MAX_WIDTH LCD_HEIGHT
#endif

#endif