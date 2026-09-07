#ifndef ROBOT_H
#define ROBOT_H
#include "animation/animation.h"
#include "menu.h"
#include "my_queue.h"
#include "telegram_client.h"
#include "pet.h"
#include <time.h>
#include <sys/time.h>


#define MENU_TIMEOUT_MS 4000
#define HUD_TIMEOUT_MS 3000
#define SLEEP_TIMEOUT_MS 10000
typedef enum {
  MENU_STATE,
  MENU_SETTINGS_STATE,
  HUD_STATE,
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
  bool hud_rendered;
  TelegramMessage current_message;
  PetStatus pet_status;
  struct timeval sleep_enter_time;
} AppState;



void app_task(void *pvParameters);


#endif