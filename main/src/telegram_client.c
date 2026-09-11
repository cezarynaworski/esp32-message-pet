#include "telegram_client.h"
#include "cJSON.h"
#include "esp_attr.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "secrets.h"
#include "system_events.h"
#include "wifi_manager.h"
#include <time.h>

static const char *TAG = "Telegram Client";
RTC_DATA_ATTR static int64_t telegram_offset = 0; // Offset for Telegram updates
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static esp_http_client_handle_t client;

esp_err_t send_telegram_message(int64_t chat_id, const char *text) {
  char url[128];
  char post_data[512];

  snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/sendMessage",
           BOT_TOKEN);

  snprintf(post_data, sizeof(post_data), "{\"chat_id\":%lld,\"text\":\"%s\"}",
           chat_id, text);

  esp_http_client_set_url(client, url);
  esp_http_client_set_method(client, HTTP_METHOD_POST);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, post_data, strlen(post_data));

  esp_err_t err = esp_http_client_perform(client);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
  } else {
    int status = esp_http_client_get_status_code(client);

    ESP_LOGI(TAG, "Telegram HTTP status: %d", status);

    if (status != 200) {
      ESP_LOGE(TAG, "Telegram sendMessage failed");
    }
  }

  esp_http_client_set_post_field(client, NULL, 0);
  esp_http_client_set_method(client, HTTP_METHOD_GET);

  return err;
}

/*
 * @brief Function to parse the Telegram message JSON response.
   It sends the message to the LCD task queue if the message is valid.
 * @param msg The JSON string received from the Telegram API.
 */
static void parse_send_Telegram_msg(const char *msg) {

  // Implementation for parsing JSON message
  cJSON *root = cJSON_Parse(msg);

  if (root == NULL) {
    ESP_LOGE(TAG, "JSON parsing error: %s", cJSON_GetErrorPtr());
  } else {
    cJSON *ok = cJSON_GetObjectItem(root, "ok");

    if (cJSON_IsTrue(ok)) {
      cJSON *result = cJSON_GetObjectItem(root, "result");

      int count = cJSON_GetArraySize(result);

      ESP_LOGI(TAG, "Updates:  %d", count);

      for (int i = 0; i < count; i++) {
        cJSON *update = cJSON_GetArrayItem(result, i);

        cJSON *update_id = cJSON_GetObjectItem(update, "update_id");

        if (cJSON_IsNumber(update_id)) {
          telegram_offset = (int64_t)update_id->valuedouble + 1;
        }

        cJSON *message = cJSON_GetObjectItem(update, "message");

        if (message != NULL) {
          // Take message text
          cJSON *text = cJSON_GetObjectItem(message, "text");

          cJSON *from = cJSON_GetObjectItem(message, "from");

          cJSON *tDate = cJSON_GetObjectItem(message, "date");

          cJSON *from_id = cJSON_GetObjectItem(from, "id");

          if (cJSON_IsString(text) && cJSON_IsNumber(from_id)) {
            // add validation if from_id is correct
            TelegramMessage telegram_msg = {0};

            telegram_msg.chat_id = (int64_t)from_id->valuedouble;
            telegram_msg.timestamp = (time_t)tDate->valuedouble;

            char *text_buffer = text->valuestring;
            size_t text_length = strlen(text_buffer);
            if (text_length > MAX_TELEGRAM_MESSAGE_LENGTH) {
              ESP_LOGW(TAG, "Message too long, truncating to %d characters",
                       MAX_TELEGRAM_MESSAGE_LENGTH - 1);
            }

            strncpy(telegram_msg.text, text_buffer,
                    MAX_TELEGRAM_MESSAGE_LENGTH - 1);
            telegram_msg.text[MAX_TELEGRAM_MESSAGE_LENGTH - 1] = '\0';
            if (telegram_msg.chat_id == CHAT_ID) {
              if (xQueueSend(telegram_lcd_queue, &telegram_msg,
                             portMAX_DELAY) != pdPASS) {
                ESP_LOGE(TAG, "Failed to send message to queue");

              } else {
                ESP_LOGI(TAG, "Message sent to queue: %s", telegram_msg.text);
              }
            }
            else{
                ESP_LOGI(TAG, "Someone sent a message from bad chat ID! ID:%llu", telegram_msg.chat_id);
            }
          }
        }
      }

    } else {
      ESP_LOGE(TAG, "Telegram returns ok=flase, response: %s", msg);
    }
    cJSON_Delete(root);
  }
}
/*
 * @brief HTTP event handler for the Telegram client.
 * @param evt The HTTP client event.
 * @return ESP_OK if successful, otherwise an error code.
 */
static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  static char *output_buffer; // Buffer to store response of http request from
                              // event handler
  static int output_len = 0;  // Stores number of bytes read
  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    if (output_len == 0 && evt->user_data) {
      // we are just starting to copy the output data into the use
      memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
    }
    if (!esp_http_client_is_chunked_response(evt->client)) {
      // If user_data buffer is configured, copy the response into the buffer
      int copy_len = 0;
      if (evt->user_data) {
        // The last byte in evt->user_data is kept for the NULL character in
        // case of out-of-bound access.
        copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
        if (copy_len) {
          memcpy(evt->user_data + output_len, evt->data, copy_len);
        }
      } else {
        int content_len = esp_http_client_get_content_length(evt->client);
        if (output_buffer == NULL) {
          // We initialize output_buffer with 0 because it is used by strlen()
          // and similar functions therefore should be null terminated.
          output_buffer = (char *)calloc(content_len + 1, sizeof(char));
          output_len = 0;
          if (output_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
            return ESP_FAIL;
          }
        }
        copy_len = MIN(evt->data_len, (content_len - output_len));
        if (copy_len) {
          memcpy(output_buffer + output_len, evt->data, copy_len);
        }
      }
      output_len += copy_len;
    }

    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
    if (output_buffer != NULL) {
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
             evt->header_value);
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
    break;

  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  }
  return ESP_OK;
}
/*
 * @brief Task to handle Telegram API requests.
 * @param pvArguments Arguments for the task.
 */
void telegram_task(void *pvArguments) {

  char url[256];
  char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};

  esp_http_client_config_t config = {
      .url = "https://api.telegram.org",
      .user_data = local_response_buffer, // buffer with response
      .crt_bundle_attach = esp_crt_bundle_attach,
      .event_handler = _http_event_handler,
      .timeout_ms = 10000,
  };

  xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE,
                      portMAX_DELAY);

  client = esp_http_client_init(&config);

  while (1) {
    // Make sure we are connected to WiFi before making the request
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE,
                        portMAX_DELAY);

    snprintf(url, sizeof(url),
             "https://api.telegram.org/bot%s/getUpdates?offset=%lld&limit=30",
             BOT_TOKEN, telegram_offset);

    esp_http_client_set_url(client, url);

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {

      ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64,
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));

      parse_send_Telegram_msg(local_response_buffer);

      local_response_buffer[0] = '\0'; // Clear the buffer for the next response

    } else {
      ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
      ESP_LOGI(TAG, "Retrying in 1 second...");
      esp_http_client_cleanup(client);
      client = esp_http_client_init(&config);
    }

    vTaskDelay(pdMS_TO_TICKS(3000));
  }

  esp_http_client_cleanup(client);
}