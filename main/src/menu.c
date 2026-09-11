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
    [SCREEN_TIMEOUT_SITEM] = &menu_feed_v2,
    [SETTINGS_MITEM] = &menu_settings_v2};

static const char *settings_items[END_OF_SETTINGS] = {
    [BRIGHTNESS_SITEM] = "BRIGHTNESS",
    [THEME_COLOR_SITEM] = "THEME COLOR",
    [SCREEN_TIMEOUT_SITEM] = "SLEEP TIME",
    [CONFIG_WIFI_SITEM] = "CONFIG WIFI",
    [TEST_SITEM] = "TEST",
    [TEST2_SITEM] = "TEST2"};

Menu settings = {.menu_items = settings_items,
                 .menu_count = END_OF_SETTINGS,
                 .current_item = END_OF_SETTINGS,
                 .next_item = BRIGHTNESS_SITEM,
                 .menu_sprites = NULL

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