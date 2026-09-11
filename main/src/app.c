
#include "app.h"
#include "animation/animation.h"
#include "animation/cat_animation.h"
#include "buttons.h"
#include "display_manager.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "graphic.h"
#include "macro.h"
#include "menu.h"
#include "my_queue.h"
#include "pet.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdint.h>
#include <sys/time.h>
#include "time.h"
#include "secrets.h"

static const char *tag = "robot";

RTC_DATA_ATTR static AppState app_state;
RTC_DATA_ATTR static TelegramMessage
    saved_telegram_messages[TELEGRAM_MESSAGES_IN_RTC];
RTC_DATA_ATTR uint8_t saved_message_count = 0;

static myQueue *telegram_msg_memory_queue;
static myQueue *local_app_event_queue;

SemaphoreHandle_t pet_stat_Mutex;

static esp_timer_handle_t menu_timeout_timer;
static esp_timer_handle_t hud_timeout_timer;
static esp_timer_handle_t sleep_timeout_timer;

static esp_timer_handle_t pet_update_timer;

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

static void sleep_timeout_callback(void *arg) {

  // save message queue to RTC Data
  saved_message_count = 0;

  Node *node = telegram_msg_memory_queue->front;

  while (node && saved_message_count < TELEGRAM_MESSAGES_IN_RTC) {
    memcpy(&saved_telegram_messages[saved_message_count], node->data,
           sizeof(TelegramMessage));

    saved_message_count++;
    node = node->next;
  }
  ESP_LOGI(tag, "saved %d messages", saved_message_count);
  gpio_set_level(PIN_NUM_BCKL, 0);
  gpio_hold_en(PIN_NUM_BCKL);
  gettimeofday(&(app_state.sleep_enter_time), NULL);
  esp_deep_sleep_start();
}
static void pet_update_callback(void *arg) {

  if (xSemaphoreTake(pet_stat_Mutex, portMAX_DELAY) == pdTRUE) {
    update_pet_status(&(app_state.pet_status), 1);
    xSemaphoreGive(pet_stat_Mutex);
  }
}

static void timers_init() {
  // create timer for menu timeout
  const esp_timer_create_args_t menu_timeout_timer_args = {
      .callback = &menu_timeout_callback, .name = "menu_timeout"};
  ESP_ERROR_CHECK(
      esp_timer_create(&menu_timeout_timer_args, &menu_timeout_timer));

  // create timer for hud timeout
  const esp_timer_create_args_t hud_timeout_timer_args = {
      .callback = &hud_timeout_callback, .name = "hud_timeout"};
  ESP_ERROR_CHECK(
      esp_timer_create(&hud_timeout_timer_args, &hud_timeout_timer));

  // create timer for sleep timeout
  const esp_timer_create_args_t sleep_timeout_timer_args = {
      .callback = &sleep_timeout_callback,
      .name = "sleep_timeout",
  };
  ESP_ERROR_CHECK(
      esp_timer_create(&sleep_timeout_timer_args, &sleep_timeout_timer));

  // create timer for sleep timeout
  const esp_timer_create_args_t pet_update_timer_args = {
      .callback = &pet_update_callback,
      .name = "pet_update",
  };
  ESP_ERROR_CHECK(esp_timer_create(&pet_update_timer_args, &pet_update_timer));
}

struct tm  get_current_time(void) {
  time_t now;
  struct tm timeinfo;

  time(&now);
  localtime_r(&now, &timeinfo);

  return timeinfo;
}
// help function so i dont to repeat it every time
static void go_idle_state(AppState *app_state) {
  app_state->current_state = CAT_IDLE_STATE;

  int hour = get_current_time().tm_hour;

  if (app_state->pet_status.hungry_status < 25 ||
      app_state->pet_status.happiness < 25) {
    app_state->current_animation = &cat_sad_animation;

  } else if (hour >= 22 || hour < 8) {
    app_state->current_animation = &cat_sleeping_animation;
  } else {
    uint32_t r = esp_random() % 100;
    if (r < 20) {
      app_state->current_animation =
          (app_state->current_animation != &cat_look_around_animation)
              ? &cat_look_around_animation
              : &cat_blinking_animation;
    } else if (r < 60) {
      app_state->current_animation = &cat_look_left_right_animation;
    } else // default loop animation
      app_state->current_animation = &cat_blinking_animation;
  }

  animation_reset(app_state->current_animation);

  esp_timer_start_once(sleep_timeout_timer, SLEEP_TIMEOUT_MS * 1000ULL);
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
  app_state->menu = &menu;
  app_state->telegram_msg_memory = telegram_msg_memory_queue;

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

  if (cause == ESP_SLEEP_WAKEUP_EXT0) {

    // restore telegram messages:
    for (int i = 0; i < saved_message_count; i++) {

      bool result= enqueue(app_state->telegram_msg_memory, &saved_telegram_messages[i]);
      if (!result)
        ESP_LOGE(tag, "Could not restore telegram message");
    }

    // obsługa wake-up
    struct timeval now;
    gettimeofday(&now, NULL);
    uint64_t sleep_time_ms =
        (now.tv_sec - app_state->sleep_enter_time.tv_sec) * 1000 +
        (now.tv_usec - app_state->sleep_enter_time.tv_usec) / 1000;
    update_pet_status(&app_state->pet_status, sleep_time_ms);
    ESP_LOGI(tag, "Deep sleep wakeup, sleep duration:%llu", sleep_time_ms);

  } else {

    app_state->pet_status.happiness = 100;
    app_state->pet_status.hungry_status = 100;
  }

  go_idle_state(app_state);
  // app_state->settings = &settings;

  esp_timer_start_periodic(pet_update_timer, 60 * 1000000ULL);
}

static void update_app_state(uint32_t app_event, AppState *app_state) {

  Menu *settings = menu_get_submenu_at(app_state->menu, SETTINGS_MITEM);
  switch (app_event) {
  case BUTTON1_SINGLE_CLICK:

    esp_timer_stop(menu_timeout_timer);
    esp_timer_stop(sleep_timeout_timer);
    switch (app_state->current_state) {

    case READING_MESSAGE_STATE:
      if (app_state->telegram_msg_memory->currSize > 0) {
        app_state->current_state = SHOW_NEXT_MESSAGE_STATE;
        dequeue(telegram_msg_memory_queue, &(app_state->current_message));

      } else {
        go_idle_state(app_state);
      }
      break;

    case MENU_STATE:
      app_state->menu->next_item =
          (MenuItem)((app_state->menu->current_item + 1) % END_OF_MENU);

      if (app_state->menu->next_item > 0 &&
          app_state->menu->next_item % MENU_VISIBLE_ITEMS == 0) {
        app_state->menu->scroll_offset += MENU_VISIBLE_ITEMS;
      }
      esp_timer_start_once(menu_timeout_timer, MENU_TIMEOUT_MS * 1000ULL);
      // restart timer during changing item

      break;

    case MENU_SETTINGS_STATE:
      settings->next_item = (settings->current_item + 1) % END_OF_SETTINGS;

      if (settings->next_item > 0 &&
          settings->next_item % MENU_VISIBLE_ITEMS == 0) {
        settings->scroll_offset += MENU_VISIBLE_ITEMS;
      }
      if (settings->next_item == 0) {
        settings->scroll_offset = 0;
      }

      esp_timer_start_once(menu_timeout_timer, MENU_TIMEOUT_MS * 1000ULL);
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
      app_state->menu->scroll_offset = 0;
      esp_timer_stop(hud_timeout_timer);
      break;
    default:
      // open hud
      esp_timer_stop(hud_timeout_timer);
      esp_timer_start_once(hud_timeout_timer, HUD_TIMEOUT_MS * 1000ULL);
      app_state->current_state = HUD_STATE;
      app_state->hud_rendered = false;
      break;
    }
    break;

  case BUTTON1_LONG_PRESS:

    esp_timer_stop(menu_timeout_timer);
    esp_timer_stop(sleep_timeout_timer);
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

        if (xSemaphoreTake(pet_stat_Mutex, portMAX_DELAY) == pdTRUE) {
          app_state->pet_status.happiness =
              MIN(100, app_state->pet_status.happiness + 25);

          xSemaphoreGive(pet_stat_Mutex);
        }

        break;

      case FEED_MITEM:
        app_state->current_state = CAT_ANIMATION_STATE;
        app_state->current_animation = &cat_eating_animation;
        animation_reset(app_state->current_animation);
        if (xSemaphoreTake(pet_stat_Mutex, portMAX_DELAY) == pdTRUE) {
          app_state->pet_status.hungry_status =
              MIN(100, app_state->pet_status.hungry_status + 50);
          xSemaphoreGive(pet_stat_Mutex);
        }
        break;

      case SETTINGS_MITEM:
        esp_timer_start_once(menu_timeout_timer,
                             MENU_TIMEOUT_MS *
                                 1000ULL); // start timer during menu entry
        app_state->current_state = MENU_SETTINGS_STATE;
        settings->current_item = END_OF_SETTINGS;
        settings->next_item = BRIGHTNESS_SITEM;
        settings->scroll_offset = 0;
        break;
      default:
        break;
      }
      break;

    case MENU_SETTINGS_STATE:

      switch (settings->current_item) {

      case BRIGHTNESS_SITEM:

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
    // esp_timer_stop(hud_timeout_timer);
    break;

  case NEW_TELEGRAM_MESSAGE:

    switch (app_state->current_state) {
    case HUD_STATE:
    case CAT_IDLE_STATE: 
    
      if (app_state->current_animation != &cat_sad_animation) {
        app_state->current_animation = &cat_new_message_animation;
        animation_reset(app_state->current_animation);
        app_state->current_state = CAT_ANIMATION_STATE;
      }
      app_state->pending_telegram_message = false;
      app_state->hud_rendered = false;
      esp_timer_stop(sleep_timeout_timer);
      esp_timer_start_once(sleep_timeout_timer, SLEEP_TIMEOUT_MS * 1000ULL);

      break;

    default:
      if (app_state->current_animation != &cat_new_message_animation) {
        app_state->pending_telegram_message = true;
      } else {
        app_state->pending_telegram_message = false;
      }
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
    if (render_menu(menu_get_submenu_at(app_state->menu, SETTINGS_MITEM),
                    false)) {
      should_flush = true;
    }
    break;
  case SHOW_NEXT_MESSAGE_STATE:
    render_messages(&(app_state->current_message),
                    app_state->telegram_msg_memory->currSize + 1);
    uint32_t app_event = NEW_MESSAGE_SHOWN;
    enqueue(local_app_event_queue, &app_event);
    should_flush = true;
    esp_timer_stop(menu_timeout_timer);
    esp_timer_start_once(menu_timeout_timer,
                         READING_MESSAGE_TIMEOUT_MS * 1000ULL);
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

void app_task(void *pvParameters) {

  uint32_t app_event = -1;
  uint32_t telegram_event = NEW_TELEGRAM_MESSAGE;

  // init local queues
  telegram_msg_memory_queue = createQueue(sizeof(TelegramMessage), TELEGRAM_MESSAGES_IN_RTC);
  local_app_event_queue = createQueue(sizeof(uint32_t), 50);

  pet_stat_Mutex = xSemaphoreCreateMutex();

  timers_init();

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

      if (enqueue(telegram_msg_memory_queue, (void *)&msg))
        enqueue(local_app_event_queue, (void *)&telegram_event);
      else{
        send_telegram_message(CHAT_ID, "Maximum message limit in memory reached");
      }

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

    // ESP_LOGI(tag, "state: %d", app_state.current_state);
    vTaskDelay(pdMS_TO_TICKS(16)); // Adjust the delay as needed
  }
}

// TODO - zrobic ustawienia? PWM sterowanie podswietleniem? :)
//
