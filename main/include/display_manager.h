#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "esp_lcd_io_spi.h"
#include "esp_lcd_st7735.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "graphic.h"
#define LCD_HOST    SPI3_HOST

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

#define PIN_NUM_DC   22
#define PIN_NUM_RST  21
#define PIN_NUM_BCKL 4


typedef enum {
  DISPLAY_STATE_IDLE_NEUTRAL,
  DISPLAY_STATE_IDLE_SLEEPING,
  DISPLAY_STATE_HAPPY,
  DISPLAY_STATE_SAD,
  DISPLAY_STATE_ANGRY,
  DISPLAY_STATE_THINKING,
  DISPLAY_STATE_PETTING,
  DISPLAY_STATE_FEEDING,
  DISPLAY_STATE_NEW_MESSAGE_RECEIVED,
  DISPLAY_STATE_NEW_MESSAGE_UNREAD,
  DISPLAY_STATE_DISPLAYING_MESSAGE,
} DisplayState;

extern esp_lcd_panel_handle_t panel_handle;
void display_init(void);
void display_flush(void);


#endif