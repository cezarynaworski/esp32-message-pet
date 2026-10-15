#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "wifi_manager.h"

#include "esp_log.h"

#include "nvs_flash.h"

#include "app.h"
#include "buttons.h"
#include "display_manager.h"
#include "driver/ledc.h"
#include "driver/rtc_io.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "menu.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "system_events.h"
#include "telegram_client.h"
#include <stdint.h>
#include "driver/i2c_master.h"
#include "include/max17048.h"
#define I2C_SDA_IO 16
#define I2C_SCL_IO 17
static const char *TAG = "main";
static i2c_master_bus_handle_t i2c_bus;

static esp_err_t i2c_bus_init() {
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_NUM_0,
      .sda_io_num = I2C_SDA_IO,
      .scl_io_num = I2C_SCL_IO,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = false,
  };
  esp_err_t err = i2c_new_master_bus(&bus_config, &i2c_bus);
  if (err != ESP_OK)
    return err;

  return max17048_add_device_to_bus(&i2c_bus);
}
static void set_time() {
  // set time
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org");
  esp_sntp_init();
}
// read settings values from nvs
static esp_err_t read_nvs_memory() {
  esp_err_t err;
  nvs_handle_t nvs_handle;
  err = nvs_open("storage", NVS_READONLY, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_open: %s", esp_err_to_name(err));
    return err;
  }
  size_t data_length = sizeof(SettingsValues);
  err = nvs_get_blob(nvs_handle, "settings_data", &settings_values, &data_length);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Brightness:%d", settings_values.screen_brightness);
    ESP_LOGI(TAG, "Color:%x", settings_values.theme_color);
  } else
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
  nvs_close(nvs_handle);
  return err;
}

static void pwm_init() {
  gpio_hold_dis(PIN_NUM_BCKL);
  ledc_timer_config_t ledc_timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_10_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = 5000, 
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  ledc_channel_config_t ledc_channel = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .timer_sel = LEDC_TIMER_0,
      .gpio_num = PIN_NUM_BCKL,
      .duty = settings_values.screen_brightness, // Set duty to 0%
      .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}
void app_main(void) {

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  /*
  //pm config
  esp_pm_config_t pm_config = {
      .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
      .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
  };
  ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
*/
 
  read_nvs_memory();
  events_init();
  display_init();
  ESP_ERROR_CHECK(i2c_bus_init());
  // Create a queue for Telegram messages to be sent to the LCD task
  telegram_lcd_queue = xQueueGenericCreate(10, sizeof(TelegramMessage), queueQUEUE_TYPE_SET);
  // create a queue for buttons
  app_event_queue = xQueueGenericCreate(10, sizeof(uint32_t), queueQUEUE_TYPE_SET);

  if (telegram_lcd_queue == NULL || app_event_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create xQueue");
    return;
  }

  wifi_init_sta();

  set_time();

  xTaskCreate(&telegram_task, "telegram_task", 20000, NULL, 5, NULL);

  xTaskCreate(&app_task, "robot_task", 12000, NULL, 5, NULL);

  xTaskCreate(&buttons_task, "button_task", 2048, NULL, 5, NULL);

  buttons_init();
  // vTaskDelay(pdMS_TO_TICKS(100)); // Wait for tasks to initialize

   pwm_init();
  // gpio_set_level(PIN_NUM_BCKL, 1);

  vTaskDelete(NULL);
}
