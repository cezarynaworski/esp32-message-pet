
#include "app.h"
#include "animation/animation.h"
#include "animation/cat_animation.h"
#include "display_manager.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "graphic.h"
#include "menu.h"
#include "my_cololors.h"
#include "my_queue.h"
#include "pet.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdint.h>
static const char *tag = "robot";

static esp_timer_handle_t menu_timeout_timer;
static esp_timer_handle_t hud_timeout_timer;
static esp_timer_handle_t sleep_timeout;


static void menu_timeout_callback(void *arg) {
  BaseType_t higher_priority_task_woken = pdFALSE;
  uint32_t app_event = MENU_TIMEOUT;
  xQueueSendFromISR(app_event_queue, &app_event, &higher_priority_task_woken);
}

static void hud_timeout_callback(void *arg) {
  BaseType_t higher_priority_task_woken = pdFALSE;
  uint32_t app_event = HUD_TIMEOUT;
  xQueueSendFromISR(app_event_queue, &app_event, &higher_priority_task_woken);
}

static myQueue *telegram_msg_memory_queue;
static myQueue *local_app_event_queue;

// help function so i dont to repeat it every time
static void go_idle_state(AppState *app_state) {
  app_state->current_state = CAT_IDLE_STATE;
  app_state->current_animation = &cat_blinking_animation;
  animation_reset(app_state->current_animation);
  // esp_timer_stop(menu_timeout_timer);
}

static void check_pending_telegram_msg(AppState *app_state) {
  if (app_state->pending_telegram_message) {
    uint32_t app_event_to_send = NEW_TELEGRAM_MESSAGE;
    enqueue(local_app_event_queue, (void *)&app_event_to_send);
  }
}

static void render_default(AppState *app_state, bool *should_flush,
                           uint64_t dt) {
  animation_update(app_state->current_animation, dt);
  if (app_state->current_animation->finished == true) {
    uint32_t app_event = ANIMATION_FINISHED;
    enqueue(local_app_event_queue, &app_event);
  }
  if (render_animation(app_state->current_animation)) {
    *should_flush = true;
  }
}

static void init_app_state(AppState *app_state) {

  app_state->current_animation = &cat_blinking_animation;
  app_state->current_state = CAT_IDLE_STATE;
  app_state->menu = &menu;
  app_state->telegram_msg_memory = telegram_msg_memory_queue;
  app_state->settings = &settings;

  app_state->cat_state.happiness = 100;
  app_state->cat_state.hungry_status = 100;
}

static void update_app_state(uint32_t app_event, AppState *app_state) {
  switch (app_event) {
  case BUTTON1_SINGLE_CLICK:

    esp_timer_stop(menu_timeout_timer);
    
    switch (app_state->current_state) {

    case READING_MESSAGE_STATE:
      if (app_state->telegram_msg_memory->currSize > 0) {
        app_state->current_state = SHOW_NEXT_MESSAGE_STATE;
        dequeue(telegram_msg_memory_queue, &(app_state->current_message));
      } else {
        go_idle_state(app_state);
      }
      break;
      break;

    case MENU_STATE:
      app_state->menu->next_item =
          (MenuItem)((app_state->menu->current_item + 1) % END_OF_MENU);
      esp_timer_start_once(menu_timeout_timer, MENU_TIMEOUT_MS * 1000);
      // restart timer during changing item

      break;

    case MENU_SETTINGS_STATE:
      app_state->settings->next_item =
          (SettingsItem)((app_state->settings->current_item + 1) %
                         END_OF_SETTINGS);
      esp_timer_start_once(menu_timeout_timer, MENU_TIMEOUT_MS * 1000);
      break;
    case CAT_ANIMATION_STATE:
      break;

    case HUD_STATE:
      // go to menu and start menu_timeout timer
      esp_timer_start_once(menu_timeout_timer,
                           MENU_TIMEOUT_MS *
                               1000); // start timer during menu entry
      app_state->current_state = MENU_STATE;
      app_state->menu->current_item = END_OF_MENU;
      app_state->menu->next_item = SHOW_MESSAGE_MITEM;
      esp_timer_stop(hud_timeout_timer);
      break;
    default:
      // open hud
      esp_timer_stop(hud_timeout_timer);
      esp_timer_start_once(hud_timeout_timer,
                           HUD_TIMEOUT_MS *
                               1000);
      app_state->current_state = HUD_STATE;
      app_state->hud_rendered = false;
      break;
    }
    break;

  case BUTTON1_LONG_PRESS:

    esp_timer_stop(menu_timeout_timer);

    switch (app_state->current_state) {

    case MENU_STATE:

      switch (app_state->menu->current_item) {

      case SHOW_MESSAGE_MITEM:
        if (app_state->telegram_msg_memory->currSize > 0) {
          app_state->current_state = SHOW_NEXT_MESSAGE_STATE;
          dequeue(telegram_msg_memory_queue, &(app_state->current_message));
          app_state->pending_telegram_message =
              false; // we dont want to do animation if its pending
        } else {
          go_idle_state(app_state);
        }
        break;

      case PET_MITEM:
        app_state->current_state = CAT_ANIMATION_STATE;
        app_state->current_animation = &cat_petting_animation;
        animation_reset(app_state->current_animation);
          
        app_state->cat_state.happiness=MIN(100,app_state->cat_state.happiness+25);
        
        break;

      case FEED_MITEM:
        app_state->current_state = CAT_ANIMATION_STATE;
        app_state->current_animation = &cat_eating_animation;
        animation_reset(app_state->current_animation);

        app_state->cat_state.hungry_status=MIN(100,app_state->cat_state.happiness+50);

        break;

      case SETTINGS_MITEM:
        esp_timer_start_once(menu_timeout_timer,
                             MENU_TIMEOUT_MS *
                                 1000); // start timer during menu entry
        app_state->current_state = MENU_SETTINGS_STATE;
        app_state->settings->current_item = END_OF_SETTINGS;
        app_state->settings->next_item = SCREEN_LIGHT_SITEM;
        break;
      default:
        break;
      }
      break;

    case MENU_SETTINGS_STATE:

      switch (app_state->settings->current_item) {

      case SCREEN_LIGHT_SITEM:
        break;
      case THEME_COLOR_SITEM:
        break;
      case SCREEN_TIMEOUT_SITEM:
        break;
      case CONFIG_WIFI_SITEM:
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    break;

  case MENU_TIMEOUT:

    check_pending_telegram_msg(app_state);
    go_idle_state(app_state);
    break;

  case HUD_TIMEOUT:
    go_idle_state(app_state);
    esp_timer_stop(hud_timeout_timer);
    break;

  case NEW_TELEGRAM_MESSAGE:

    switch (app_state->current_state) {
    case HUD_STATE:
    case CAT_IDLE_STATE: // we want to play animation before we can dequeue msg
                         // and show it. LWe will wait untill we are in
                         // CAT_IDLE_STATE
      app_state->current_animation = &cat_new_message_animation;
      animation_reset(app_state->current_animation);
      app_state->current_state = CAT_ANIMATION_STATE;
      app_state->pending_telegram_message = false;
      app_state->hud_rendered = false;
      break;
    default:
      app_state->pending_telegram_message = true;
    }
    break;

  case ANIMATION_FINISHED:
    check_pending_telegram_msg(app_state);
    go_idle_state(app_state);
    break;

  case NEW_MESSAGE_SHOWN:
    app_state->current_state = READING_MESSAGE_STATE;
    break;
  }
}

static void render_app(AppState *app_state, uint64_t dt) {
  bool should_flush = false;

  switch (app_state->current_state) {
  case MENU_STATE:
    if (render_menu(app_state->menu,
                    (app_state->telegram_msg_memory->currSize > 0))) {
      should_flush = true;
    }
    break;

  case MENU_SETTINGS_STATE:
    if (render_menu(app_state->settings, false)) {
      should_flush = true;
    }
    break;
  case SHOW_NEXT_MESSAGE_STATE:
    render_messages(&(app_state->current_message),
                    app_state->telegram_msg_memory->currSize + 1);
    uint32_t app_event = NEW_MESSAGE_SHOWN;
    enqueue(local_app_event_queue, &app_event);
    should_flush = true;
    break;
  case READING_MESSAGE_STATE:
    break;

  case HUD_STATE:
    // show hud and render animation
    render_default(app_state, &should_flush, dt);
    should_flush |=
        render_hud(app_state, should_flush); // if the animation changed, then
                                             // we must render once again
    break;
  default: // render animation
    render_default(app_state, &should_flush, dt);
    break;
  }
  if (should_flush)
    display_flush();
}



void update_cat_state(CatState cat_state)
  {

  }
void app_task(void *pvParameters) {
  AppState app_state = {};
  uint32_t app_event = -1;
  uint32_t telegram_event = NEW_TELEGRAM_MESSAGE;

  // init local queues
  telegram_msg_memory_queue = createQueue(sizeof(TelegramMessage));
  local_app_event_queue = createQueue(sizeof(uint32_t));

  // create timer for menu timeout
  const esp_timer_create_args_t menu_timeout_timer_args = {
      .callback = &menu_timeout_callback, .name = "menu_timeout"};
  ESP_ERROR_CHECK(
      esp_timer_create(&menu_timeout_timer_args, &menu_timeout_timer));

  const esp_timer_create_args_t hud_timeout_timer_args = {
      .callback = &hud_timeout_callback, .name = "hud_timeout"};
  ESP_ERROR_CHECK(
      esp_timer_create(&hud_timeout_timer_args, &hud_timeout_timer));

  init_app_state(&app_state);
  // at the begginign we should also take from nvm queue with telegram messages

  uint64_t last_time = esp_timer_get_time() / 1000;
  while (1) {
    // Take loop time
    uint64_t now = esp_timer_get_time() / 1000;
    uint64_t dt = now - last_time;
    last_time = now;

    // Check Telegram Queue
    TelegramMessage msg = {0};
    while (xQueueReceive(telegram_lcd_queue, (void *)&msg, 0) == pdTRUE) {

      enqueue(telegram_msg_memory_queue, (void *)&msg); // send to MyQueue
                                                        // buffor
      // xQueueSend(app_event_queue, &telegram_event,
      //          0); // send event to xQueue (RTOS)
      enqueue(local_app_event_queue, (void *)&telegram_event);

      msg.text[0] = '\0';
      msg.chat_id = 0;
    }
    // Check App Queue
    while ((xQueueReceive(app_event_queue, (void *)&app_event, 0) == pdTRUE)) {

      enqueue(local_app_event_queue, (void *)&app_event);
      // update_app_state(app_event, &app_state);
    }

    while (dequeue(local_app_event_queue, (void *)&app_event)) {
      update_app_state(app_event, &app_state);
    }

    render_app(&app_state, dt);

    //ESP_LOGI(tag, "state: %d", app_state.current_state);
    vTaskDelay(pdMS_TO_TICKS(16)); // Adjust the delay as needed
  }
}

// TODO - zrobic ustawienia? PWM sterowanie podswietleniem? :)
//  w ustawieniach moze tez byc timeout bo jakim przygasza sie wyswietlacz

// TODO na jutro:
/*

2. dorobienie animacji feed i pet - akywacja podczas wcisniecia


*/