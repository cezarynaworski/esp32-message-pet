#include "display_manager.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "graphic.h"
#include "hal/lcd_types.h"
#include <machine/endian.h>
#include <string.h>

static const char *TAG = "Display Manager";


static const st7735_lcd_init_cmd_t waveshare_init_cmds[] = {

    {0xB1, (uint8_t[]){0x01, 0x2C, 0x2D}, 3},
    {0xB2, (uint8_t[]){0x01, 0x2C, 0x2D}, 3},
    {0xB3, (uint8_t[]){0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}, 6},

    {0xB4, (uint8_t[]){0x07}, 1},

    {0xC0, (uint8_t[]){0xA2, 0x02, 0x84}, 3},
    {0xC1, (uint8_t[]){0xC5}, 1},
    {0xC2, (uint8_t[]){0x0A, 0x00}, 2},
    {0xC3, (uint8_t[]){0x8A, 0x2A}, 2},
    {0xC4, (uint8_t[]){0x8A, 0xEE}, 2},
    {0xC5, (uint8_t[]){0x0E}, 1},

    {0xE0,
     (uint8_t[]){0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B,
                 0x23, 0x37, 0x00, 0x07, 0x02, 0x10},
     16},

    {0xE1,
     (uint8_t[]){0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30,
                 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10},
     16},

    {0xF0, (uint8_t[]){0x01}, 1},
    {0xF6, (uint8_t[]){0x00}, 1},

    {0x3A, (uint8_t[]){0x05}, 1},
};
esp_lcd_panel_handle_t panel_handle;

void display_init(void) {

  gpio_reset_pin(PIN_NUM_BCKL);
  gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_NUM_BCKL, 0); // Turn off backlight
  gpio_hold_dis(PIN_NUM_BCKL);

  spi_bus_config_t buscfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = -1,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz =
          LCD_WIDTH * 80 * sizeof(uint16_t), // transfer 40 lines at a time
  };

  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  esp_lcd_panel_io_handle_t io_handle = NULL;

  esp_lcd_panel_io_spi_config_t io_config = {
      .dc_gpio_num = PIN_NUM_DC,
      .cs_gpio_num = PIN_NUM_CS,

      .pclk_hz = 20 * 1000 * 1000,

      .spi_mode = 0,

      .trans_queue_depth = 10,

      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
  };

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle));

  panel_handle = NULL;

  st7735_vendor_config_t vendor_config = {
      .init_cmds = waveshare_init_cmds,
      .init_cmds_size =
          sizeof(waveshare_init_cmds) / sizeof(waveshare_init_cmds[0]),
  };

  esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_NUM_RST,

      
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,

      .bits_per_pixel = 16,

      .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,

      .vendor_config = &vendor_config,
  };
  ESP_ERROR_CHECK(
      esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 2, 1));
  

  //clear display
  clear_background(0x0000); // Clear the screen to black
  display_flush();
}

void display_flush() {

  esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_WIDTH, LCD_HEIGHT,
                            framebuffer);
}


