#ifndef BUTTONS_H
#define BUTTONS_H

#define MAIN_BUTTON 25
#define TOUCH_BUTTON 20

#define ESP_INTR_FLAG_DEFAULT 0
//FLAGS for Button xTaskNotify - do not mistake them with ButtonFlags
//from system_events!
#define NOTIFY_MAIN_BUTTON_PUSH (1<<0)
#define NOTIFY_MAIN_BUTTON_RELEASE (1<<1)
#define NOTIFY_MAIN_BUTTON_LONG_CLICK (1<<2)
#define NOTIFY_TOUCH_BUTTON_CLICK (1 << 3)
//TO QUEUE (robot task) FLAGS (UINT32t)


#define DEBOUNCE_MS 40
#define LONG_PRESS_MS 500


void buttons_init();
void buttons_task(void *arg);
#endif