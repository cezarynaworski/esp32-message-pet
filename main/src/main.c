#include "freertos/idf_additions.h"
#include "wifi_manager.h"

#include "esp_log.h"

#include "nvs_flash.h"

#include "buttons.h"
#include "display_manager.h"
#include "app.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdint.h>
static const char *TAG = "main";


// konfiguracja WIFI przy 30 nieudanych polaczeniach - esp32 jako router wifi

// dodanie zeby tylko od jednego id odbieralo - validacja id

void app_main(void) {

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Take values from NVS...

  //...

  events_init();
  display_init();
  buttons_init();

  // Create a queue for Telegram messages to be sent to the LCD task
  telegram_lcd_queue =
      xQueueGenericCreate(10, sizeof(TelegramMessage), queueQUEUE_TYPE_SET);
  // create a queue for buttons
  app_event_queue = xQueueGenericCreate(10, sizeof(uint32_t), queueQUEUE_TYPE_SET);

  if (telegram_lcd_queue == NULL || app_event_queue ==NULL ) {
    ESP_LOGE(TAG, "Failed to create xQueue");
    return;
  }
  // Init WIFI
  wifi_init_sta();

  xTaskCreate(&telegram_task, "telegram_task", 12000, NULL, 5, NULL);
  // create button task
  //...

  xTaskCreate(&app_task, "robot_task", 12000, NULL, 5, NULL);

  xTaskCreate(&buttons_task, "button_task", 2048, NULL, 5, NULL);

  vTaskDelete(NULL);
}
