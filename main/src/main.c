#include "freertos/idf_additions.h"
#include "wifi_manager.h"

#include "esp_log.h"

#include "nvs_flash.h"

#include "app.h"
#include "buttons.h"
#include "display_manager.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdint.h>

static const char *TAG = "main";

static void set_time() {
  // set time
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org");
  esp_sntp_init();
}
void app_main(void) {

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  events_init();
  display_init();
  buttons_init();

  // Create a queue for Telegram messages to be sent to the LCD task
  telegram_lcd_queue =
      xQueueGenericCreate(10, sizeof(TelegramMessage), queueQUEUE_TYPE_SET);
  // create a queue for buttons
  app_event_queue =
      xQueueGenericCreate(10, sizeof(uint32_t), queueQUEUE_TYPE_SET);

  if (telegram_lcd_queue == NULL || app_event_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create xQueue");
    return;
  }

  wifi_init_sta();

  set_time();

  xTaskCreate(&telegram_task, "telegram_task", 20000, NULL, 5, NULL);

  xTaskCreate(&app_task, "robot_task", 12000, NULL, 5, NULL);

  xTaskCreate(&buttons_task, "button_task", 2048, NULL, 5, NULL);

  // vTaskDelay(pdMS_TO_TICKS(100)); // Wait for tasks to initialize
  gpio_set_level(PIN_NUM_BCKL, 1);

  vTaskDelete(NULL);
}
