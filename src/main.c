#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht11.h"
#include "driver/gpio.h"

#define DHT_PIN 1  // Sử dụng GPIO 1 làm chân kết nối DATA cho cảm biến DHT11
#define BUZZER_PIN 2 // GPIO kết nối với còi

static const char *TAG = "DHT11";

void app_main(void) {
    // Khởi tạo thư viện DHT11
    DHT11_init(DHT_PIN);

    // Khởi tạo GPIO cho còi
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
    }; 
    gpio_config(&io_conf);

    float temperature, humidity;
    while (1) {
        // Đọc dữ liệu từ cảm biến DHT11
        struct dht11_reading reading = DHT11_read();
        if (reading.status == DHT11_OK) {
            temperature = (float)reading.temperature;
            humidity = (float)reading.humidity;
            ESP_LOGI(TAG, "Nhiet do: %.2f°C, Do am: %.2f%%", temperature, humidity);

            // Kiểm tra điều kiện và phát còi nếu ít nhất một trong hai điều kiện được thỏa mãn
            if (temperature > 55.0 || humidity < 40.0) {
                gpio_set_level(BUZZER_PIN, 1); // Kích thích còi
            }
        } else if (reading.status == DHT11_TIMEOUT_ERROR) {
            ESP_LOGE(TAG, "Timeout reading data from DHT11");
        } else if (reading.status == DHT11_CRC_ERROR) {
            ESP_LOGE(TAG, "CRC error reading data from DHT11");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Đợi 1 giây
    }
}