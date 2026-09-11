#ifndef TELEGRAM_CLIENT_H
#define TELEGRAM_CLIENT_H
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>
#include "time.h"
#define MAX_HTTP_OUTPUT_BUFFER 16384
#define MAX_TELEGRAM_MESSAGE_LENGTH 62


typedef struct {
  char text[MAX_TELEGRAM_MESSAGE_LENGTH];
  int64_t chat_id;
  time_t timestamp;
} TelegramMessage;

void telegram_task(void *pvArguments);
esp_err_t send_telegram_message(int64_t chat_id, const char *text);
#endif