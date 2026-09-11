#include "buttons.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "system_events.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include <stdint.h>

#include <string.h>

static const char *TAG = "button";
static TaskHandle_t xButtonTaskHandle = NULL;

static esp_timer_handle_t long_press_timer;

static void long_press_callback(void *arg) {
  xTaskNotify(xButtonTaskHandle, NOTIFY_MAIN_BUTTON_LONG_CLICK, eSetBits);
}
void buttons_task(void *arg) {
  configASSERT(xButtonTaskHandle == NULL);
  xButtonTaskHandle = xTaskGetCurrentTaskHandle();
  uint32_t events;

  bool main_button_pressed = false;
  
  while (1) {
    uint32_t queue_event = 0;
    xTaskNotifyWait(0, UINT32_MAX, &events, portMAX_DELAY);
    switch (events) {
    // Regular notification -
    case (NOTIFY_MAIN_BUTTON_PUSH | NOTIFY_MAIN_BUTTON_RELEASE):
      // debounce
      vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
      // take level
      int level = gpio_get_level(MAIN_BUTTON);

      if (!level) {
        main_button_pressed = true;
        esp_timer_stop(long_press_timer);
        esp_timer_start_once(long_press_timer, LONG_PRESS_MS * 1000);

      } else {
        if (main_button_pressed) {
          
          //ESP_LOGE(TAG, "Single Click");
          queue_event = BUTTON1_SINGLE_CLICK;
          main_button_pressed = false;
          esp_timer_stop(long_press_timer);
          
          // }
        }
      }
      break;
    case NOTIFY_MAIN_BUTTON_LONG_CLICK:
      if (main_button_pressed) {
        queue_event = BUTTON1_LONG_PRESS;
        //ESP_LOGE(TAG, "LONG_PRESS");
        main_button_pressed = false; // reset main button press so we wont have
                                     // single click after that
        esp_timer_stop(long_press_timer);
        break;
      }
    }
    if (queue_event != 0)
      xQueueSend(app_event_queue, &queue_event, portMAX_DELAY);
  }

  xTaskNotifyStateClear(xButtonTaskHandle);
}

static void IRAM_ATTR gpio_isr_handler(void *arg) {

  uint32_t gpio_num = (uint32_t)arg;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (gpio_num == MAIN_BUTTON) {
    xTaskNotifyFromISR(xButtonTaskHandle,
                       NOTIFY_MAIN_BUTTON_PUSH | NOTIFY_MAIN_BUTTON_RELEASE,
                       eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void buttons_init() {
  
  gpio_config_t io_conf = {};

  io_conf.mode = GPIO_MODE_INPUT;

  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

  io_conf.pin_bit_mask = 1 << MAIN_BUTTON;

  io_conf.intr_type = GPIO_INTR_ANYEDGE;

  gpio_config(&io_conf);

  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  gpio_isr_handler_add(MAIN_BUTTON, gpio_isr_handler, (void *)MAIN_BUTTON);

  // create timers for detecting long press and very long press
  const esp_timer_create_args_t long_press_timer_args = {
      .callback = &long_press_callback, .name = "long_press"};
  ESP_ERROR_CHECK(esp_timer_create(&long_press_timer_args, &long_press_timer));


  //config waking up from deep sleep on button press
  ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(MAIN_BUTTON, 0));
  ESP_ERROR_CHECK(rtc_gpio_pullup_en(MAIN_BUTTON));
  gpio_deep_sleep_hold_en();
  
 
   
}