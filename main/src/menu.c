#include "menu.h"
#include "esp_netif_types.h"
#include "stdbool.h"
#include <sys/errno.h>

static const char *menu_items[END_OF_MENU] = {
    [SHOW_MESSAGE_MITEM] = "MESSAGES",
    [PET_MITEM] = "PET",
    [FEED_MITEM] = "FEED",
    [SETTINGS] = "SETTINGS"
};

Menu menu = {
    .menu_items = menu_items,
    .menu_count = END_OF_MENU,
    .current_item = END_OF_MENU,
    .next_item = SHOW_MESSAGE_MITEM
};
