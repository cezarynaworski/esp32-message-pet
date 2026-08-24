#ifndef ROBOT_H
#define ROBOT_H
#include "animation/animation.h"
#include "menu.h"
#include "my_queue.h"
#include "telegram_client.h"
#define MENU_TIMEOUT_MS 4000

typedef enum {
  MENU_STATE,
  MENU_SETTINGS_STATE,
  READING_MESSAGE_STATE,
  SHOW_NEXT_MESSAGE_STATE,
  CAT_IDLE_STATE,
  CAT_ANIMATION_STATE,
} CurrentState;

typedef struct {
  Animation *current_animation;
  CurrentState current_state;
  Menu *menu;
  myQueue *telegram_msg_memory;
  bool pending_telegram_message;
  TelegramMessage current_message;
} AppState;

typedef struct {
  uint8_t love_status; // happines
  uint8_t hungry_status;
  uint8_t energy;
} CatState;


void app_task(void *pvParameters);


#endif