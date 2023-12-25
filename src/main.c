/**
 * @file main.h
 * @author Nhom6_KTVXL_HUST (luan172002@gmail.com)
 *         Lê Thành Luân
 *         Nguyễn Vũ Trọng Tuấn
 *         Giáp Đình Đăng
 *         Khuất Khánh Duy
 * @brief LCD1602 with module i2c
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "freertos/event_groups.h"
#include "blynk.h"
#include "esp_system.h"
#include "dht11.h"
#include "LCD.h"
#include "wifiesp.h"
// char buffer[20]="Anh nho em ";
// char buffer1[20]="Thu Hien ak";
// char buffer2[20]="Chuc vo iu";
#define DHT_PIN 1  // Sử dụng GPIO 1 làm chân kết nối DATA cho cảm biến DHT11
#define BUZZER_PIN 2 // GPIO kết nối với còi
#define BLYNK_TOKEN "SjdMAHWAm8VwtpG9HE3_GtCcTAZLDvv_"
#define BLYNK_SERVER "blynk.cloud"

char msg0[20]="ConnectingWifi..";
char msg1[20]="Connected Wifi!";
char msg2[20]="Timeout ";
char msg3[20]="Manual Mode";
char msg4[20]="Connnected to Blynk";
uint8_t connection_status = 0;
float temperature, humidity;

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Initialize LCD1602 
    lcd_init();
    lcd_clear();
    lcd_send_string(msg0);

    //Initialize DHT11
    DHT11_init(DHT_PIN);

    //Initialize Buzzer
        // Khởi tạo GPIO cho còi
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
    }; 
    gpio_config(&io_conf);

    //Initialize Wifi
    wifi_init_sta(&connection_status);
    if(connection_status == 1){
        lcd_clear();
        lcd_send_string(msg1);
    }else{
        lcd_clear();
        lcd_send_string(msg2);
        vTaskDelay(500);
        lcd_clear();
        lcd_send_string(msg3);
    }

    //Initialize Blynk
	// ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	// blynk_client_t *client = malloc(sizeof(blynk_client_t));
	// blynk_init(client);

	// blynk_options_t opt = {
	// 	.token = BLYNK_TOKEN,
	// 	/* Use default timeouts */
	// };

	// blynk_set_options(client, &opt);
    // lcd_clear();
    // lcd_send_string(msg4);
	// /* Subscribe to state changes and errors */
	// blynk_set_state_handler(client, state_handler, NULL);

	/* blynk_set_handler sets hardware (BLYNK_CMD_HARDWARE) command handler */
	// blynk_set_handler(client, "vw", vw_handler, NULL);
	// blynk_set_handler(client, "vr", vr_handler, NULL);

	/* Start Blynk client task */
	// blynk_start(client);

    while(1){
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



