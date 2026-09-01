#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum {
  BUTTON1_SINGLE_CLICK = (1 << 0),
  BUTTON1_LONG_PRESS = (1 << 1),
  MENU_TIMEOUT = (1 << 2),
  NEW_TELEGRAM_MESSAGE = (1<<3),
  ANIMATION_FINISHED = (1<<4),
  NEW_MESSAGE_SHOWN = (1<<5),
  HUD_TIMEOUT = (1<<6)
} AppEventFlags;

typedef enum {
  WIFI_CONNECTED_BIT = 1 << 0,
  WIFI_FAIL_BIT = 1 << 1,
} WifiFlags;

extern EventGroupHandle_t wifi_event_group;

extern QueueHandle_t telegram_lcd_queue;
extern QueueHandle_t app_event_queue;

void events_init(void);

#endif