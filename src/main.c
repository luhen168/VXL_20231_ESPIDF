#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

// Định nghĩa SSID và mật khẩu của mạng WiFi của bạn
#define WIFI_SSID "VNPT THAO TOAN"
#define WIFI_PASS "Baolam2410"

// Định nghĩa tag để sử dụng với ESP_LOGI
static const char *TAG = "wifi_example";

// Định nghĩa các sự kiện để theo dõi trạng thái kết nối WiFi
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// Hàm xử lý sự kiện kết nối WiFi
static esp_err_t event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data);

// Hàm khởi tạo WiFi
void wifi_init_sta()
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Tạo và cài đặt interface mạng mặc định cho wifi
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Đăng ký sự kiện IP_EVENT_STA_GOT_IP với hàm xử lý event_handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_LOGI(TAG, "Đang cài đặt cấu hình WiFi SSID %s...", wifi_config.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Đang đợi kết nối WiFi...");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Đã kết nối với WiFi");
}

// Hàm xử lý sự kiện kết nối WiFi
static esp_err_t event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        // In địa chỉ IP khi kết nối thành công
        char ip4addr_str[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, ip4addr_str, sizeof(ip4addr_str));
        ESP_LOGI(TAG, "Kết nối với WiFi. Đã nhận IP: %s", ip4addr_str);
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "Ngắt kết nối WiFi. Đang kết nối lại...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
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
