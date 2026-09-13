#include "menu.h"
#include "esp_netif_types.h"
#include "sprites/menu_sprites.h"
#include "stdbool.h"
#include <sys/errno.h>
static const char *menu_items[END_OF_MENU] = {[SHOW_MESSAGE_MITEM] = "MESSAGES",
                                              [PET_MITEM] = "PET",
                                              [FEED_MITEM] = "FEED",
                                              [SETTINGS_MITEM] = "SETTINGS"};

static const Sprite *menu_sprites[END_OF_MENU] = {
    [SHOW_MESSAGE_MITEM] = &menu_msg_v2,
    [PET_MITEM] = &menu_pet_v2,
    [FEED_MITEM] = &menu_feed_v2,
    [SETTINGS_MITEM] = &menu_settings_v2};

static const char *settings_items[END_OF_SETTINGS] = {
    [BRIGHTNESS_SITEM] = "BRIGHTNESS",
    [THEME_COLOR_SITEM] = "THEME COLOR",
    [CONFIG_WIFI_SITEM] = "CONFIG WIFI"};

static const char *brigtness_items[END_OF_BIRGHTNESS] = {
    [TWOFIVE_BITEM] = "25%",
    [FIVEZERO_BITEM] = "50%",
    [SEVENFIVE_BITEM] = "75%",
    [TENZERO_BITEM] = "100%"};

static const char *color_items[END_OF_COLORS] = {
    [BLUE_CITEM] = "BLUE", [RED_CITEM] = "RED",       [GREEN_CITEM] = "GREEN",
    [PINK_CITEM] = "PINK", [VIOLET_CITEM] = "VIOLET", [ORANGE_CITEM] = "ORANGE",
    [LIME_CITEM] = "LIME", [PURPLE_CITEM] = "PURPLE",
};

Menu brightness_menu = {.menu_items = brigtness_items,
                        .menu_count = END_OF_BIRGHTNESS,
                        .current_item = END_OF_BIRGHTNESS,
                        .next_item = TWOFIVE_BITEM,
                        .menu_sprites = NULL

};
Menu colors_menu = {.menu_items = color_items,
                    .menu_count = END_OF_COLORS,
                    .current_item = END_OF_COLORS,
                    .next_item = BLUE_CITEM,
                    .menu_sprites = NULL

};

static Menu *settings_submenus[END_OF_SETTINGS] = {&brightness_menu,
                                                   &colors_menu, NULL};

Menu settings = {.menu_items = settings_items,
                 .menu_count = END_OF_SETTINGS,
                 .current_item = END_OF_SETTINGS,
                 .next_item = BRIGHTNESS_SITEM,
                 .menu_sprites = NULL,
                 .submenu = settings_submenus

};

static Menu *menu_submenus[END_OF_MENU] = {NULL, NULL, NULL, &settings};

Menu menu = {.menu_items = menu_items,
             .menu_count = END_OF_MENU,
             .current_item = END_OF_MENU,
             .next_item = SHOW_MESSAGE_MITEM,
             .menu_sprites = menu_sprites,
             .submenu = menu_submenus};

Menu *menu_get_submenu_at(Menu *menu, uint8_t item) {
  if (!menu || !menu->submenu)
    return NULL;

  if (item >= menu->menu_count)
    return NULL;

  return menu->submenu[item];
}

SettingsValues settings_values = {
    .screen_brightness = 1024,
    .theme_color = BLUE,
};

void menu_scroll_to_next_item(Menu *menu) {

  menu->next_item = (menu->current_item + 1) % menu->menu_count;
  if (menu->next_item > 0 && menu->next_item % MENU_VISIBLE_ITEMS == 0) {
    menu->scroll_offset += MENU_VISIBLE_ITEMS;
  }
  if (menu->next_item == 0) {
    menu->scroll_offset = 0;
  }
}
void menu_reset(Menu *menu) {
  menu->current_item = menu->menu_count;
  menu->next_item = 0;
  menu->scroll_offset = 0;
}
