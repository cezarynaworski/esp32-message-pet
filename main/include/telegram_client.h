#ifndef TELEGRAM_CLIENT_H
#define TELEGRAM_CLIENT_H
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>
#define MAX_HTTP_OUTPUT_BUFFER 8192
#define MAX_TELEGRAM_MESSAGE_LENGTH 128


typedef struct {
  char text[MAX_TELEGRAM_MESSAGE_LENGTH];
  int64_t chat_id;
  //add time stamp
} TelegramMessage;

void telegram_task(void *pvArguments);

#endif