#ifndef MENU_H
#define MENU_H
#include "my_cololors.h"
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
  CONFIG_WIFI_SITEM,
  END_OF_SETTINGS
} SettingsItem;


typedef enum {
  TWOFIVE_BITEM,
  FIVEZERO_BITEM,
  SEVENFIVE_BITEM,
  TENZERO_BITEM,
  END_OF_BIRGHTNESS
} BrightnessItem;


typedef enum {
  BLUE_CITEM,
  RED_CITEM,
  GREEN_CITEM,
  PINK_CITEM,
  VIOLET_CITEM,
  ORANGE_CITEM,
  LIME_CITEM,
  PURPLE_CITEM,
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
  uint16_t screen_brightness;
  uint16_t theme_color;
  char bot_token[256];
  char wifi_password[256];
  char wifi_ssid[256];
  int64_t chat_id;
} SettingsValues;


extern Menu menu;
extern Menu settings;
extern Menu brightness_menu;
extern Menu colors_menu;

extern SettingsValues settings_values;

Menu* menu_get_submenu_at(Menu *menu, uint8_t item);
void menu_scroll_to_next_item(Menu *menu);
void menu_reset(Menu *menu);

#endif
