#ifndef MENU_H
#define MENU_H
#include "stdbool.h"
#include <stdint.h>
#include "sprites/sprite.h"
typedef enum {
  SHOW_MESSAGE_MITEM,
  PET_MITEM,
  FEED_MITEM,
  SETTINGS,
  END_OF_MENU
} MenuItem;

typedef struct {
  const char **menu_items;
  uint8_t menu_count;
  MenuItem current_item;
  MenuItem next_item;
  const Sprite **menu_sprites;

} Menu;

extern Menu menu;

#endif
