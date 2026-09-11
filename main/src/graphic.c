#include "graphic.h"
#include "animation/animation.h"
#include "app.h"
#include "esp_timer.h"
#include "fonts/PressStart2P13.h"
#include "fonts/PressStart2P14.h"
#include "fonts/Tiny16.h"
#include "fonts/font.h"
#include "menu.h"
#include "my_cololors.h"
#include "my_queue.h"
#include "sprites/hud.h"
#include "sprites/menu_sprites.h"
#include "sprites/sprite.h"
#include "stdio.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
uint16_t framebuffer[LCD_WIDTH * LCD_HEIGHT] = {0};

void draw_pixel_portrait(int x, int y, uint16_t color) {
  if (x < 0 || x >= LCD_WIDTH)
    return;

  if (y < 0 || y >= LCD_HEIGHT)
    return;
  color = ((color << 8) & 0xff00) | (color >> 8);
  framebuffer[y * LCD_WIDTH + x] = color;
}
void draw_pixel_landscape(int x, int y, uint16_t color) {
  if (x < 0 || x >= LCD_HEIGHT)
    return;

  if (y < 0 || y >= LCD_WIDTH)
    return;

  int px = y;
  int py = LCD_HEIGHT - 1 - x;
  color = ((color << 8) & 0xff00) | (color >> 8);
  framebuffer[py * LCD_WIDTH + px] = color;
}

void draw_line(int x1, int y1, int x2, int y2, int thickness, uint16_t color) {

  int dx = x2 - x1;
  int dy = y2 - y1;

  int min_x = (x1 < x2) ? x1 : x2;
  int max_x = (x1 > x2) ? x1 : x2;
  int min_y = (y1 < y2) ? y1 : y2;
  int max_y = (y1 > y2) ? y1 : y2;

  int r = thickness / 2;

  min_x -= r;
  max_x += r;
  min_y -= r;
  max_y += r;

  int line_len_sq = dx * dx + dy * dy;

  for (int y = min_y; y <= max_y; y++) {
    for (int x = min_x; x <= max_x; x++) {

      int px = x - x1;
      int py = y - y1;

      int closest_x;
      int closest_y;

      if (line_len_sq == 0) {
        closest_x = x1;
        closest_y = y1;
      } else {
        int t = (px * dx + py * dy);

        if (t <= 0) {
          closest_x = x1;
          closest_y = y1;
        } else if (t >= line_len_sq) {
          closest_x = x2;
          closest_y = y2;
        } else {
          closest_x = x1 + (t * dx) / line_len_sq;
          closest_y = y1 + (t * dy) / line_len_sq;
        }
      }

      int dist_x = x - closest_x;
      int dist_y = y - closest_y;

      if (dist_x * dist_x + dist_y * dist_y <= r * r) {
        draw_pixel(x, y, color);
      }
    }
  }
}

void draw_rect(int x, int y, int w, int h, bool rounded, uint16_t color) {
  // color = ((color << 8) & 0xff00) | (color >> 8);

  if (!rounded) {
    for (int xx = x; xx < x + w; xx++) {
      for (int yy = y; yy < y + h; yy++) {
        draw_pixel(xx, yy, color);
      }
    }
    return;
  }

  const int r = 4;

  for (int yy = y; yy < y + h; yy++) {
    for (int xx = x; xx < x + w; xx++) {

      int dx = 0;
      int dy = 0;

      if (xx < x + r)
        dx = x + r - xx;
      else if (xx >= x + w - r)
        dx = xx - (x + w - r - 1);

      if (yy < y + r)
        dy = y + r - yy;
      else if (yy >= y + h - r)
        dy = yy - (y + h - r - 1);

      if (dx == 0 || dy == 0 || dx * dx + dy * dy <= r * r)
        draw_pixel(xx, yy, color);
    }
  }
}

void draw_circle(int x0, int y0, int r, uint16_t color) {
  color = ((color << 8) & 0xff00) | (color >> 8);
  int x = r;
  int y = 0;

  int err = 1 - r;

  while (x >= y) {

    draw_pixel(x0 + x, y0 + y, color);
    draw_pixel(x0 + y, y0 + x, color);

    draw_pixel(x0 - y, y0 + x, color);
    draw_pixel(x0 - x, y0 + y, color);

    draw_pixel(x0 - x, y0 - y, color);
    draw_pixel(x0 - y, y0 - x, color);

    draw_pixel(x0 + y, y0 - x, color);
    draw_pixel(x0 + x, y0 - y, color);

    y++;

    if (err <= 0) {
      err += 2 * y + 1;
    } else {
      x--;
      err += 2 * (y - x) + 1;
    }
  }
}
void clear_background(uint16_t color) {
  color = ((color << 8) & 0xff00) | (color >> 8);
  uint32_t frame_size = LCD_WIDTH * LCD_HEIGHT;

  for (int i = 0; i < frame_size; i++) {
    framebuffer[i] = color;
  }
}

void clear_area(int x, int y, int width, int height, uint16_t color) {
  color = ((color << 8) & 0xff00) | (color >> 8);
  for (int py = y; py < y + height; py++) {
    for (int px = x; px < x + width; px++) {
      draw_pixel(px, py, color);
    }
  }
}

void draw_char(int x, int y, const Glyph *glyph, const Font *font,
               uint16_t color) {
  // color = ((color << 8) & 0xff00) | (color >> 8);
  if (glyph == NULL)
    return;

  int bit_index = 0;

  for (int yy = 0; yy < glyph->height; yy++) {
    for (int xx = 0; xx < glyph->width; xx++) {

      int byte_index = bit_index / 8;

      int bit_index_in_byte = 7 - (bit_index % 8);

      if (glyph->bitmap[byte_index] & (1 << bit_index_in_byte)) {
        draw_pixel(x + xx + glyph->xOffset, y + yy + glyph->yOffset, color);
      }

      bit_index++;
    }
  }
}

static int draw_word(int x, int y, const Glyph **word_glyphs,
                     size_t word_length, const Font *font, uint16_t color) {
  // color = ((color << 8) & 0xff00) | (color >> 8);
  int cursor_x = x;

  for (int i = 0; i < word_length; i++) {
    draw_char(cursor_x, y, word_glyphs[i], font, color);

    cursor_x += word_glyphs[i]->xAdvance;
  }

  return cursor_x;
}
DrawStatus draw_text(int x, int y, const char *text, const Font *font,
                     uint16_t color, uint8_t max_lines,
                     uint16_t max_line_x_position,
                     const char **remaining_text) {
  // color = ((color << 8) & 0xff00) | (color >> 8);

  const char *word_start = text;

  int cursor_x = x;
  int cursor_y = y;

  int word_pixel_length = 0;
  uint8_t word_index = 0;
  uint8_t line_index = 1;

  const Glyph *word_glyphs[MAX_WORD_SIZE];

  while (*text) {
    word_start = text;
    uint32_t unicode;

    // iterate through unicode
    if ((uint8_t)*text < 0x80) {
      // ASCII
      unicode = (uint8_t)*text;
      text++;
    } else if (((uint8_t)*text & 0xE0) == 0xC0) {
      // 2 bytes UTF-8
      unicode = ((uint8_t)text[0] & 0x1F) << 6 | ((uint8_t)text[1] & 0x3F);

      text += 2;
    } else if (((uint8_t)*text & 0xF0) == 0xE0) {
      // 3 bytes UTF-8
      unicode = ((uint8_t)text[0] & 0x0F) << 12 |
                ((uint8_t)text[1] & 0x3F) << 6 | ((uint8_t)text[2] & 0x3F);

      text += 3;
    } else {
      text++;
      continue;
    }
    if (word_index > (MAX_WORD_SIZE - 1))
      return DRAW_ERR_WORD_TOO_LONG;
    // if it is not 'space'
    if (unicode != 0x20) {
      // append word_glyphs table
      const Glyph *glyph = find_glyph(unicode, font);
      if (glyph) {
        word_glyphs[word_index] = find_glyph(unicode, font);

        word_pixel_length += word_glyphs[word_index]->xAdvance;

        word_index++;
      }
    } else
    // draw word when there is a space
    {
      // wrap
      if ((cursor_x + word_pixel_length) >= max_line_x_position) {
        if (line_index < max_lines) {
          cursor_x = x;
          cursor_y += font->lineHeight;
          line_index++;
        } else {
          if (remaining_text != NULL) {
            if ((uint8_t)*word_start == 0x20)
              *remaining_text = ++word_start;
            else {
              *remaining_text = word_start;
            }
          }
          return DRAW_ERR_TEXT_TOO_LONG;
        }
      }
      cursor_x =
          draw_word(cursor_x, cursor_y, word_glyphs, word_index, font, color);

      word_index = 0;
      word_pixel_length = 0;
      // add space
      cursor_x += font->space_width;
    }
  }
  // last word in text - no space after that word
  if (word_index > 0) {

    if ((cursor_x + word_pixel_length) >= max_line_x_position) {
      if (line_index < max_lines) {
        cursor_x = x;
        cursor_y += font->lineHeight;
        line_index++;
      } else {
        if (remaining_text != NULL) {
          if ((uint8_t)*word_start == 0x20)
            *remaining_text = ++word_start;
          else {
            *remaining_text = word_start;
          }
        }
        return DRAW_ERR_TEXT_TOO_LONG;
      }
    }
    draw_word(cursor_x, cursor_y, word_glyphs, word_index, font, color);
  }
  return DRAW_OK;
}

void draw_sprite(int x, int y, const Sprite *sprite, bool mirror_horizontal) {
  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int src_x = sx;

      if (mirror_horizontal) {
        src_x = sprite->width - 1 - sx;
      }

      uint16_t pixel = sprite_get_pixel(sprite, src_x, sy);
      if (pixel != TRANSPARENT_COLOR)
        draw_pixel(x + sx, y + sy, pixel);
    }
  }
}
// render current animation frame
bool render_animation(Animation *anim) {
  // Check if frame is already displayed
  if (anim->frame_flushed) {
    return false;
  }
  const LayerInfo *layer_info = anim->layer_info;
  if (layer_info == NULL)
    return false;

  const AnimationFrame *frame = animation_get_current_frame(anim);
  // const AnimationFrame *previous_frame =
  // animation_get_previous_frame(anim);

  const LayerOffset *layer_offset = frame->layer_offset;
  // const LayerOffset *layer_previous_offset = previous_frame->layer_offset;

  clear_background(BLACK);
  if (frame->text != NULL) {
    draw_text(0 + frame->text_offset.x, 0 + frame->text_offset.y, frame->text,
              &PressStart2P13, WHITE, 1, MAX_WIDTH, NULL);
  }
  // iterate through layers
  for (uint8_t layer = 0; layer < anim->layer_count; layer++) {
    int16_t x_offset = 0, y_offset = 0;

    const Sprite *sprite = frame->layers[layer];

    if (sprite == NULL) {
      continue;
    }
    if (layer_offset != NULL) {
      x_offset = layer_offset[layer].x;
      y_offset = layer_offset[layer].y;
    }
    draw_sprite(layer_info[layer].x + x_offset, layer_info[layer].y + y_offset,
                sprite, layer_info[layer].mirror);
  }
  anim->frame_flushed = true;

  return true;
}

bool render_menu(Menu *menu, bool new_message) {

  bool sprites = false;
  uint8_t x_text_offset = 5;
  static bool blink = false;
  static bool change_flushed = false;

  static uint64_t last = 0;

  uint64_t now;
  now = esp_timer_get_time();
  if (now - last > 500000) {
    blink = !blink;
    if (new_message)
      change_flushed = false;
    last = now;
  }

  if (menu->next_item != menu->current_item)
    change_flushed = false;

  if (change_flushed)
    return false;

  if (menu->menu_sprites) {
    sprites = true;
    x_text_offset = 27;
  }

  // current displayed menuItems

  clear_background(BLACK);

  uint8_t en = 0;

  for (int i = menu->scroll_offset;
       i < menu->scroll_offset + MENU_VISIBLE_ITEMS; i++) {

    if (menu->next_item == i) { // highlight next_item
      draw_rect(0, 3 + en * 30, MAX_WIDTH, 30, true, BLUE);

      menu->current_item = i; // current item is next_item
    }

    draw_text(x_text_offset, 13 + en * 30, menu->menu_items[i], &PressStart2P13,
              WHITE, 1, MAX_WIDTH, NULL);
    // draw_sprite(3, 10, &message_mail, false);

    if (sprites) {
      if (i == SHOW_MESSAGE_MITEM) {
        if (!new_message || !blink) {
          draw_sprite(4, 10 + en * 30, menu->menu_sprites[i], false);
        }
      } else {
        draw_sprite(4, 10 + en * 30, menu->menu_sprites[i], false);
      }
    }
    en++;
  }
  change_flushed = true;
  return true;
}

void render_messages(TelegramMessage *message, uint8_t msg_left) {

  const Font *font = &Tiny16;
  clear_background(BLACK);

  char header_buffer[256];
  const char *remaining_text;
  if (msg_left == 1) {
    sprintf(header_buffer, "%d MESSAGE", msg_left);
  } else {
    sprintf(header_buffer, "%d MESSAGES", msg_left);
  }
  // draw_rect(0, 0, MAX_WIDTH, 30, 0, WHITE);

  draw_line(0, 30, MAX_WIDTH, 30, 2, WHITE);
  draw_text(5, 10, header_buffer, &PressStart2P13, WHITE, 1, MAX_WIDTH, NULL);
  draw_sprite(0, 70, &cat_reading_message_menu, true);

  DrawStatus draw_status = draw_text(5, 35, message->text, font, WHITE, 2,
                                     MAX_WIDTH, &remaining_text);
  if (draw_status == DRAW_ERR_TEXT_TOO_LONG) {
    draw_text(45, 35 + font->lineHeight * 2, remaining_text, font, WHITE, 3,
              MAX_WIDTH, NULL);
  }
}

bool render_hud(AppState *app_state, bool animation_changed) {
  if (app_state->hud_rendered == true && !animation_changed) {
    return false;
  }
  // draw time
  time_t now;
  struct tm timeinfo;

  time(&now);
  localtime_r(&now, &timeinfo);
  char time_str[6];
  snprintf(time_str, sizeof(time_str), "%02d:%02d", timeinfo.tm_hour,
           timeinfo.tm_min);

  draw_text(120, 0, time_str, &Tiny16, WHITE, 1, MAX_WIDTH, NULL);

  EventBits_t WifiBits;
  WifiBits = xEventGroupGetBits(wifi_event_group);
  if (WifiBits == WIFI_CONNECTED_BIT) {
    draw_sprite(100, 1, &wifi, false);
  }

  // draw_rect(0, 0, MAX_WIDTH, 15, false, BLUE);
  if (app_state->telegram_msg_memory->currSize > 0) {
    draw_sprite(80, 3, &message_new, 0);
  }
  float hunger = app_state->pet_status.hungry_status;
  float happines = app_state->pet_status.happiness;

  if (happines > 75)
    draw_sprite(5, 0, &hud_heart_100, false);
  else if (happines > 50)
    draw_sprite(5, 0, &hud_heart_75, false);
  else if (happines > 25)
    draw_sprite(5, 0, &hud_heart_50, false);
  else if (happines > 10)
    draw_sprite(5, 0, &hud_heart_25, false);
  else
    draw_sprite(5, 0, &hud_heart_0, false);

  if (hunger > 75)
    draw_sprite(25, 0, &hud_hungry_100, false);
  else if (hunger > 50)
    draw_sprite(25, 0, &hud_hungry_75, false);
  else if (hunger > 25)
    draw_sprite(25, 0, &hud_hungry_50, false);
  else if (hunger > 10)
    draw_sprite(25, 0, &hud_hungry_25, false);
  else
    draw_sprite(25, 0, &hud_hungry_0, false);

  app_state->hud_rendered = true;
  return true;
}
