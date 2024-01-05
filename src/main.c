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
#include "wifiesp.h"
#include "MQ135.h"
#include "lcd1602_app.h"
#include "mqttTCP.h"

#define BUZZER_PIN 32
#define DHT11_PIN 26  

static esp_mqtt_client_handle_t client;
uint8_t connection_status = 0;
i2c_lcd1602_info_t * lcd_info;
dht11_reading dht11;
MQ135 mq135;

void sendData(void *parameter) {
    char dataConvertedToString[20];
    while(1){
        // Read data DHT11
        dht11 = DHT11_read();
        mq135 = MQ135_readData();
        lcd1602_updateScreen(lcd_info,&dht11,&mq135);
        if(connection_status == 0){
            wifi_init_sta(&connection_status);
        }
        if((mq135.ppm)>100||(dht11.humidity<40&&dht11.temperature>40)){
            gpio_set_level(BUZZER_PIN, 1); // Kích thích còi
        }else{
            gpio_set_level(BUZZER_PIN, 0); // Kích thích còi
        }
        sprintf(dataConvertedToString,"%d,%d,%d",dht11.humidity,dht11.temperature,mq135.ppm);
        esp_mqtt_client_publish(client, "VXL20231/LUAN",dataConvertedToString,sizeof(dataConvertedToString), 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Đợi 5 giây
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(__func__, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(__func__, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        // MQ135 *mq135 = (MQ135*)malloc(sizeof(MQ135));
        ESP_LOGI(__func__, "MQTT_EVENT_CONNECTED");
        // char dataConvertedToString[20];
        // sprintf(dataConvertedToString,"%d,%d,%s",dht11->humidity,dht11->temperature,mq135->ppm);
        // msg_id = esp_mqtt_client_publish(client, "VXL20231/LUAN", "dataConvertedToString",sizeof(dataConvertedToString), 0, 0);
        // ESP_LOGI(__func__, "sent publish successful, msg_id=%d", msg_id);
        // free(mq135);
        xTaskCreate(sendData,"Send data to MQTT broker",4096,NULL,5,0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(__func__, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(__func__, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(__func__, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(__func__, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(__func__, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(__func__, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(__func__, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(__func__, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(__func__, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize LCD1602 */
    lcd_info = i2c_lcd1602_malloc();
    lcd1602_init(lcd_info);
    
    /* Initialize Wifi */
    i2c_lcd1602_write_string(lcd_info,"ConnectingWifi..");
    wifi_init_sta(&connection_status);
    if(connection_status == 1){
        i2c_lcd1602_clear(lcd_info);
        i2c_lcd1602_write_string(lcd_info,"Connected Wifi!");
    }else{
        i2c_lcd1602_clear(lcd_info);
        i2c_lcd1602_write_string(lcd_info,"Timeout ");
        vTaskDelay(500);
        i2c_lcd1602_clear(lcd_info);
        i2c_lcd1602_write_string(lcd_info,"Manual Mode");
    }
    vTaskDelay(500);
    i2c_lcd1602_clear(lcd_info);

    /* Initialize DHT11 */
    DHT11_init(DHT11_PIN);

    /* Initialize MQ135 */
    MQ135_init();   //use pin function

    /* Initialize Buzzer */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
    }; 
    gpio_config(&io_conf);
    gpio_set_level(BUZZER_PIN, 0); 

    /* MQTT */
    mqtt_app_start();
}



