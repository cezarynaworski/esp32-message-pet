#include "menu.h"
#include "esp_netif_types.h"
#include "stdbool.h"
#include <sys/errno.h>
#include "sprites/menu_sprites.h"
static const char *menu_items[END_OF_MENU] = {
    [SHOW_MESSAGE_MITEM] = "MESSAGES",
    [PET_MITEM] = "PET",
    [FEED_MITEM] = "FEED",
    [SETTINGS] = "SETTINGS"
};

static const Sprite* menu_sprites[END_OF_MENU]=
    {
    [SHOW_MESSAGE_MITEM] = &menu_msg_v2,
    [PET_MITEM] = &menu_pet_v2,
    [FEED_MITEM] = &menu_feed_v2,
    [SETTINGS] = &menu_settings_v2
    };

Menu menu = {
    .menu_items = menu_items,
    .menu_count = END_OF_MENU,
    .current_item = END_OF_MENU,
    .next_item = SHOW_MESSAGE_MITEM,
    .menu_sprites = menu_sprites
};
