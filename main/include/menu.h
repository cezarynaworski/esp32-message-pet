#ifndef MENU_H
#define MENU_H
#include "stdbool.h"
#include <stdint.h>
#include "sprites/sprite.h"
typedef enum {
  SHOW_MESSAGE_MITEM,
  PET_MITEM,
  FEED_MITEM,
  SETTINGS_MITEM,
  END_OF_MENU
} MenuItem;

typedef enum {
  SCREEN_LIGHT_SITEM,
  THEME_COLOR_SITEM,
  SCREEN_TIMEOUT_SITEM,
  CONFIG_WIFI_SITEM,
  END_OF_SETTINGS
} SettingsItem;

typedef struct {
  const char **menu_items;
  uint8_t menu_count;
  MenuItem current_item;
  MenuItem next_item;
  const Sprite **menu_sprites;
} Menu;

typedef struct {
  uint8_t screen_brightness;
  uint16_t theme_color;
  uint32_t sleep_timeout_ms;

}settings_values;


extern Menu menu;
extern Menu settings;

#endif
