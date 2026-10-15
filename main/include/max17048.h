#ifndef MAX17048_H
#define MAX17048_H
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stdio.h"
#include <stdint.h>


#define MAX17048_SENSOR_ADDR 0x36

typedef enum {
  MAX17048_REG_VCELL = 0x02,
  MAX17048_REG_SOC = 0x04,
  MAX17048_REG_MODE = 0x06,
  MAX17048_REG_VERSION = 0x08,
  MAX17048_REG_HIBRT = 0x0A,
  MAX17048_REG_CONFIG = 0x0C,
  MAX17048_REG_VALERT = 0x14,
  MAX17048_REG_CRATE = 0x16,
  MAX17048_REG_VRESET_ID = 0x18,
  MAX17048_REG_STATUS = 0x1A,
  MAX17048_REG_POR= 0xFE
} max17048_register_t;

typedef struct {
  bool voltage_reset_alert;
  bool undervoltage_alert;
  bool overvoltage_alert;
  bool soc_changed_alert;
  bool soc_low_alert;
  bool reset_indicator;
}max17048_status_t;

typedef struct{
uint8_t hibernate_threshold;
uint8_t active_threshold;
uint8_t undervoltage_threshold;
uint8_t overvoltage_threshold;
uint8_t soc_min_threshold;
bool enable_voltage_reset_alert;
bool enable_SOC_change_alert;
bool enable_sleep_mode;
}max17048_config_t;

typedef struct {
  uint8_t slave_address;
  uint32_t i2c_speed;
  uint32_t i2c_timeout;
} max17048_i2c_config_t;


esp_err_t max17048_register_read(uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t max17048_register_write(uint8_t reg_addr, uint8_t data0, uint8_t data1);
esp_err_t max17048_add_device_to_bus(i2c_master_bus_handle_t* bus_handle);
// Result in mV
esp_err_t max17048_read_voltage(float *voltage);
// Result in %
esp_err_t max17048_read_soc(float *soc);
esp_err_t max17048_read_version(uint16_t *version);
esp_err_t max17048_read_sensor_id(uint8_t *sensor_id);
esp_err_t max17048_read_status_register(max17048_status_t *status);
esp_err_t max17048_reset_status_register(void);
// Result in %/h
esp_err_t max17048_read_SOC_rate(float *rate);

//stays in hibernate mode all the time
esp_err_t max17048_force_hibernate(void);
esp_err_t max17048_disable_hibernate(void);
esp_err_t max17048_read_hibernate_status(bool *is_hibernating);

esp_err_t max17048_do_quick_start(void);

esp_err_t max17048_sleep_mode_set(void);
esp_err_t max17048_sleep_mode_clear(void);

esp_err_t max17048_read_ALRT(bool* alert_occured);
esp_err_t max17048_clear_ALRT(void);

void max17048_POR(void);

esp_err_t max17048_configure(max17048_config_t* config);

void max17048_configure_i2c_slave(uint8_t slave_address, uint32_t i2c_speed, uint32_t i2c_timeout);

#endif