#ifndef ROBOT_H
#define ROBOT_H
#include "animation/animation.h"
#include "menu.h"
#include "my_queue.h"
#include "pet.h"
#include "telegram_client.h"
#include <sys/time.h>
#include <time.h>

#define MENU_TIMEOUT_MS 4000
#define HUD_TIMEOUT_MS 3000
#define SLEEP_TIMEOUT_MS 15000
#define READING_MESSAGE_TIMEOUT_MS 30000
#define TELEGRAM_MESSAGES_IN_RTC 60
typedef enum {
  MENU_STATE,
  HUD_STATE,
  READING_MESSAGE_STATE,
  SHOW_NEXT_MESSAGE_STATE,
  CAT_IDLE_STATE,
  CAT_ANIMATION_STATE,
} CurrentState;

//AppState structure is kept in RTC memory
typedef struct {
  Animation *current_animation;
  CurrentState current_state;
  Menu *current_menu;
  myQueue *telegram_msg_memory; // we copy memory from that pointer to RTC table
                                // (we are not saving it to nvm so after power
                                // lose all messages will be lost)
  bool pending_telegram_message;
  bool hud_rendered;
  TelegramMessage current_message;
  PetStatus pet_status;         //we lose PetStatus after Power lose
  struct timeval sleep_enter_time;
  SettingsValues *settings_values; // we init that in main from nvm
} AppState;

void app_task(void *pvParameters);

#endif