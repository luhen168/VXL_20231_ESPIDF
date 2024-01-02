/**
 * @file main.h
 * @author Nhom6_KTVXL_HUST (luan172002@gmail.com)
 *         Lê Thành Luân
 *         Nguyễn Vũ Trọng Tuấn
 *         Giáp Đình Đăng
 *         Khuất Khánh Duy
 * @brief SYSTEM WARNING FIRE IN KITCHEN HOUSE
 * @version 1.0
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "dht11.h"
#include "LCD.h"
#include "wifiesp.h"
#include "MQ135.h"
#include "mqttTCP.h"

#define BUZZER_PIN 32
#define DHT11_PIN 26  

uint8_t connection_status = 0;
float temperature, humidity;
static const char *TAG = "DHT11";

void app_main(void)
{
    MQ135 *mq135 = (MQ135 *)malloc(sizeof(MQ135));
    char temp[10];
    char hum[10];
/* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

/* Initialize LCD1602 */
    lcd_init();
    lcd_clear();
    lcd_send_string("ConnectingWifi..");

/* Initialize Wifi */
    wifi_init_sta(&connection_status);
    if(connection_status == 1){
        lcd_clear();
        lcd_send_string("Connected Wifi!");
    }else{
        lcd_clear();
        lcd_send_string("Timeout ");
        vTaskDelay(500);
        lcd_clear();
        lcd_send_string("Manual Mode");
    }

/* Initialize DHT11 */
    DHT11_init(DHT11_PIN);
 
/* Initialize MQ135 */
    MQ135_init();
    lcd_clear();
    lcd_put_cur(1,0);
    lcd_send_string("PPM GAS:");
/* MQTT*/
    mqtt_app_start();
    vTaskDelay(1000);
/* Initialize Buzzer */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
    }; 
    gpio_config(&io_conf);
/* BEGIN WHILE LOOP */  
    while(1){
        // Read data DHT11
        dht11_reading reading = DHT11_read();
        if (reading.status == DHT11_OK) {
            sprintf(temp,"%d",reading.temperature);
            sprintf(hum,"%d",reading.humidity);
            ESP_LOGI(TAG, "Nhiet do: %d°C, Do am: %d", reading.temperature, reading.humidity);
            lcd_put_cur(0,0);
            lcd_send_string("RH:");
            lcd_send_string(hum);
            lcd_send_string("%");
            lcd_put_cur(0,8);
            lcd_send_string(temp);
            lcd_send_string("oC");
            // Check condition to enable buzzer to warning fire
            if (temperature > 55 || humidity < 40) {
                gpio_set_level(BUZZER_PIN, 1); 
            }
        } else if (reading.status == DHT11_TIMEOUT_ERROR) {
            ESP_LOGE(TAG, "Timeout reading data from DHT11");
        } else if (reading.status == DHT11_CRC_ERROR) {
            ESP_LOGE(TAG, "CRC error reading data from DHT11");
        }
        //Read data MQ135
        MQ135_readData(mq135->ppm);
        lcd_put_cur(1,9);
        lcd_send_string(mq135->ppm);

        vTaskDelay(pdMS_TO_TICKS(1000));  // Đợi 1 giây

    }
}



