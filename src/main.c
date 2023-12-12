#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

// Định nghĩa SSID và mật khẩu của mạng WiFi của bạn
#define WIFI_SSID "HUAWEI-qV76"
#define WIFI_PASS "Chuabavang2023"

// Định nghĩa tag để sử dụng với ESP_LOGI
static const char *TAG = "wifi_example";

// Định nghĩa các sự kiện để theo dõi trạng thái kết nối WiFi
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// Hàm xử lý sự kiện kết nối WiFi
static esp_err_t event_handler(void *ctx, system_event_t *event);

// Hàm khởi tạo WiFi
void wifi_init_sta()
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Waiting for WiFi to be connected...");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to WiFi");
}

// Hàm xử lý sự kiện kết nối WiFi
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "Connecting to WiFi...");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "Connected to WiFi. Got IP: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "Disconnected from WiFi. Reconnecting...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void app_main()
{
    ESP_LOGI(TAG, "ESP32 WiFi Station Example");

    // Khởi tạo và kết nối WiFi
    wifi_init_sta();

    while (1)
    {
        // Thực hiện các công việc khác ở đây
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
