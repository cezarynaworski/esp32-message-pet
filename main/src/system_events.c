#include "system_events.h"
#include "freertos/idf_additions.h"

EventGroupHandle_t wifi_event_group;
//Queue
QueueHandle_t telegram_lcd_queue;
QueueHandle_t app_event_queue;



void events_init(void) {
  wifi_event_group = xEventGroupCreate();
  

}