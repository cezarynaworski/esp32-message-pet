#ifndef MENU_H
#define MENU_H
#include "sprites/sprite.h"
#include "stdbool.h"
#include <stdint.h>

#define MENU_VISIBLE_ITEMS 4


typedef enum {
  SHOW_MESSAGE_MITEM,
  PET_MITEM,
  FEED_MITEM,
  SETTINGS_MITEM,
  END_OF_MENU
} MenuItem;

typedef enum {
  BRIGHTNESS_SITEM,
  THEME_COLOR_SITEM,
  SCREEN_TIMEOUT_SITEM,
  CONFIG_WIFI_SITEM,
  TEST_SITEM,
  TEST2_SITEM,
  END_OF_SETTINGS
} SettingsItem;


typedef enum {
  TWOFIVE,
  FIVEZERO,
  SEVENFIVE,
  TENZERO,
  END_OF_BIRGHTNESS
} BrightnessItem;

typedef enum {
  BLUE,
  RED,
  GREEN,
  PINK,
  END_OF_COLORS
} ColorItem;




typedef struct Menu{
  const char **menu_items;
  uint8_t menu_count;
  uint8_t current_item;
  uint8_t next_item;
  uint8_t scroll_offset;  

  const Sprite **menu_sprites;
  struct Menu** submenu;
} Menu;

typedef struct {
  uint8_t screen_brightness;
  uint16_t theme_color;
  uint32_t sleep_timeout_ms;

} settings_values;

extern Menu menu;
extern Menu settings;




Menu* menu_get_submenu_at(Menu *menu, uint8_t item);
#endif
