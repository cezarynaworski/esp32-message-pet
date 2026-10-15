
#include "include/max17048.h"
#include "esp_err.h"
#include <string.h>
static i2c_master_dev_handle_t dev_handle;

static max17048_i2c_config_t i2c_config = {
    .slave_address = MAX17048_SENSOR_ADDR,
    .i2c_speed = 400000,
    .i2c_timeout = 1000,
};

static esp_err_t max17048_set_hibernate_active_threshold(uint8_t hibernate, uint8_t active) {
  return max17048_register_write(MAX17048_REG_HIBRT, hibernate, active);
}

static esp_err_t max17048_set_status_voltage_threshold(uint8_t voltage_min, uint8_t voltage_max) {
  return max17048_register_write(MAX17048_REG_VALERT, voltage_min, voltage_max);
}

static esp_err_t max17048_set_status_procentage_threshold(uint8_t soc_min) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  data[1] = (data[1] & ~0x1F) | (soc_min & 0x1F);
  return max17048_register_write(MAX17048_REG_CONFIG, data[0], data[1]);
}

esp_err_t max17048_register_read(uint8_t reg_addr, uint8_t *data, size_t len) {
  return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, i2c_config.i2c_timeout);
}

esp_err_t max17048_register_write(uint8_t reg_addr, uint8_t data0, uint8_t data1) {
  uint8_t write_buf[3] = {reg_addr, data0, data1};
  return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), i2c_config.i2c_timeout);
}

esp_err_t max17048_add_device_to_bus(i2c_master_bus_handle_t *bus_handle) {

  i2c_device_config_t dev_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = i2c_config.slave_address,
      .scl_speed_hz = i2c_config.i2c_speed,
  };
  return i2c_master_bus_add_device(*bus_handle, &dev_config, &dev_handle);
}

void max17048_configure_i2c_slave(uint8_t slave_address, uint32_t i2c_speed, uint32_t i2c_timeout) {
  i2c_config.slave_address = slave_address;
  i2c_config.i2c_speed = i2c_speed;
  i2c_config.i2c_timeout = i2c_timeout;
}
// Result in mV
esp_err_t max17048_read_voltage(float *voltage) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_VCELL, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  uint16_t value = ((uint16_t)data[0] << 8) | data[1];

  *voltage = value * 78.125f / 1000.0f;
  return ESP_OK;
}
// Result in %
esp_err_t max17048_read_soc(float *soc) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_SOC, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  uint16_t value = ((uint16_t)data[0] << 8) | data[1];

  *soc = value / 256.0f;
  return ESP_OK;
}

esp_err_t max17048_read_version(uint16_t *version) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_VERSION, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  *version = ((uint16_t)data[0] << 8) | data[1];
  return ESP_OK;
}

esp_err_t max17048_force_hibernate(void) { return max17048_register_write(MAX17048_REG_HIBRT, 0xFF, 0xFF); }
esp_err_t max17048_disable_hibernate(void) { return max17048_register_write(MAX17048_REG_HIBRT, 0x00, 0x00); }
esp_err_t max17048_read_hibernate_status(bool *is_hibernating) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_MODE, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  *is_hibernating = (bool)(data[0] & 1 << 4);
  return ESP_OK;
}

esp_err_t max17048_read_sensor_id(uint8_t *sensor_id) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_VRESET_ID, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  *sensor_id = (uint16_t)data[1];
  return ESP_OK;
}

esp_err_t max17048_read_status_register(max17048_status_t *status) {

  memset(status, 0, sizeof(max17048_status_t));
  uint8_t data[2];

  esp_err_t err = max17048_register_read(MAX17048_REG_STATUS, data, 2);
  if (err != ESP_OK) {
    return err;
  }

  if (data[0] & (1 << 5)) {
    status->soc_changed_alert = true;
  }
  if (data[0] & (1 << 4)) {
    status->soc_low_alert = true;
  }
  if (data[0] & (1 << 3)) {
    status->voltage_reset_alert = true;
  }
  if (data[0] & (1 << 2)) {
    status->undervoltage_alert = true;
  }
  if (data[0] & (1 << 1)) {
    status->overvoltage_alert = true;
  }
  if (data[0] & (1 << 0)) {
    status->reset_indicator = true;
  }

  return ESP_OK;
}

esp_err_t max17048_reset_status_register(void) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_STATUS, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  if ((data[0] & 0x40) != 0)
    data[0] = 0x40;
  else
    data[0] = 0x00;

  return max17048_register_write(MAX17048_REG_STATUS, data[0], 0x00);
}

esp_err_t max17048_read_SOC_rate(float *rate) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CRATE, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  int16_t value = ((int16_t)data[0] << 8) | data[1];

  *rate = value * 0.208f;
  return ESP_OK;
}

esp_err_t max17048_do_quick_start(void) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_MODE, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  data[0] |= (1 << 6);
  return max17048_register_write(MAX17048_REG_MODE, data[0], data[1]);
}
esp_err_t max17048_sleep_mode_set(void) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  data[1] |= (1 << 7);
  return max17048_register_write(MAX17048_REG_CONFIG, data[0], data[1]);
}
esp_err_t max17048_sleep_mode_clear(void) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  data[1] &= ~(1 << 7);
  return max17048_register_write(MAX17048_REG_CONFIG, data[0], data[1]);
}

esp_err_t max17048_read_ALRT(bool *alert_occured) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  *alert_occured = (data[1] & (1 << 5)) != 0;
  return ESP_OK;
}

esp_err_t max17048_clear_ALRT(void) {
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  data[1] &= ~(1 << 5);
  return max17048_register_write(MAX17048_REG_CONFIG, data[0], data[1]);
}

void max17048_POR(void) {  max17048_register_write(MAX17048_REG_POR, 0x54, 0x00); }

esp_err_t max17048_configure(max17048_config_t *config) {

  // enable or disable sleep mode
  uint8_t data[2];
  esp_err_t err = max17048_register_read(MAX17048_REG_MODE, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  if (config->enable_sleep_mode) {
    data[0] |= (1 << 5);
  } else {
    data[0] &= ~(1 << 5);
  }
  err = max17048_register_write(MAX17048_REG_MODE, data[0], data[1]);
  if (err != ESP_OK) {
    return err;
  }

  // set hibernate and active thresholds
  err = max17048_set_hibernate_active_threshold(config->hibernate_threshold, config->active_threshold);
  if (err != ESP_OK) {
    return err;
  }

  err = max17048_set_status_voltage_threshold(config->undervoltage_threshold, config->overvoltage_threshold);
  if (err != ESP_OK) {
    return err;
  }

  err = max17048_set_status_procentage_threshold(config->soc_min_threshold);
  if (err != ESP_OK) {
    return err;
  }

  // voltage reset alert
  err = max17048_register_read(MAX17048_REG_STATUS, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  if (config->enable_voltage_reset_alert) {
    data[0] |= (1 << 6);
  } else {
    data[0] &= ~(1 << 6);
  }
  err = max17048_register_write(MAX17048_REG_STATUS, data[0], data[1]);
  if (err != ESP_OK) {
    return err;
  }
  // soc change alert
  err = max17048_register_read(MAX17048_REG_CONFIG, data, 2);
  if (err != ESP_OK) {
    return err;
  }
  if (config->enable_SOC_change_alert) {
    data[0] |= (1 << 6);
  } else {
    data[0] &= ~(1 << 6);
  }
  err = max17048_register_write(MAX17048_REG_CONFIG, data[0], data[1]);
  if (err != ESP_OK) {
    return err;
  }
  return ESP_OK;
}