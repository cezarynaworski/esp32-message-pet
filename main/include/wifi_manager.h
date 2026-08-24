#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <stdbool.h>


#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

void wifi_init_sta(void);
bool wifi_is_connected(void);

#endif