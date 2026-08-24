#include "wifi_manager.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

#include "esp_timer.h"

#include <string.h>
#include "secrets.h"
#include "system_events.h"
static const char *TAG = "wifi station";

static volatile bool s_connected = false; 

static unsigned char wifi_ssid[32];
static unsigned char wifi_password[64];

//Periodic try to connect to WiFi is its disconnected
static esp_timer_handle_t wifi_retry_timer;

bool wifi_is_connected(void) {
    return s_connected;
}

static void wifi_retry_timer_callback(void* arg) {
    if (!s_connected) {
        esp_wifi_connect();
    }
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    //Start WIFI (First call)
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    //Next Calls - if previous esp_wifi_connect failed 
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {

        wifi_event_sta_disconnected_t *event =
        (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW(TAG, "WiFi disconnected, reason=%d", event->reason);
        s_connected = false;
        ESP_LOGW(TAG, "Connecting to WiFi failed, retry in 10s");
        
        esp_timer_stop(wifi_retry_timer);
        esp_timer_start_once(wifi_retry_timer, 10000000);

        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_connected=true;
        esp_timer_stop(wifi_retry_timer);
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        
        xEventGroupClearBits(wifi_event_group, WIFI_FAIL_BIT);
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/*
@brief First WiFi Initialization. Here we create wifi retry timer.
*/
void wifi_init_sta(void)
{   
    //Create WiFi retry timer
    const esp_timer_create_args_t retry_timer_args = {
        .callback = &wifi_retry_timer_callback,
        .name = "wifi_retry"
    };
    ESP_ERROR_CHECK(esp_timer_create(&retry_timer_args, &wifi_retry_timer));
    

    //tcp ip stack init
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = "",
            .ssid = WIFI_SSID,
            .password=WIFI_PASSWORD
        },
    };
    //strcpy((char*)wifi_config.sta.ssid, (const char*)wifi_ssid);
    //strcpy((char*)wifi_config.sta.password, (const char*)wifi_password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");


}
