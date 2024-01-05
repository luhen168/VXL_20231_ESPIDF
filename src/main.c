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
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "DHT11.h"
#include "WIFI.h"
#include "MQ135.h"
// #include "i2c-lcd1602.h"
#include "lcd1602_app.h"
// LCD1602
// #define LCD_NUM_ROWS               2
// #define LCD_NUM_COLUMNS            32
// #define LCD_NUM_VISIBLE_COLUMNS    16

// #define I2C_MASTER_NUM           I2C_NUM_0
// #define I2C_MASTER_TX_BUF_LEN    0                     // disabled
// #define I2C_MASTER_RX_BUF_LEN    0                     // disabled
// #define I2C_MASTER_FREQ_HZ       100000
// #define I2C_MASTER_SDA_IO        21
// #define I2C_MASTER_SCL_IO        22
// #define LCD1602_I2C_ADDRESS      0x27


// #define BUZZER_PIN 32
#define DHT11_PIN 26
// #define TAG "ESP32_LOG"
// uint8_t connection_status = 0;
// int temperature, humidity;

// static void i2c_master_init(void);
// void n6_task(void * pvParameter);
// char temp[10];
// char hum[10];
// char sensorData[50];

/************ Function Protptype ***********/
void updateScreen_task(void *parameters);

/*********** MAIN ************/
void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    /* Initialize DHT11 */
    DHT11_init(DHT11_PIN);
    /* Initialize MQ135 */
    MQ135_init();
    /* Initialize LCD1602 */
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd1602_init());
    xTaskCreate(updateScreen_task,"updateScreen",4096, NULL, 5, NULL);
    // xTaskCreate(&n6_task, "n6", 4096, NULL, 5, NULL);
}

void updateScreen_task(void *parameters){
    MQ135 *mq135 = (MQ135 *)malloc(sizeof(MQ135));
    MQ135_readData(mq135->ppm);
    // Read data DHT11
    dht11_reading dht11 = DHT11_read();
    lcd1602_updateScreen(&dht11,mq135);
    vTaskDelay(pdTICKS_TO_MS(100));
}

// static void log_error_if_nonzero(const char *message, int error_code)
// {
//     if (error_code != 0) {
//         ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
//     }
// }


// /**
//  * @brief Event handler registered to receive MQTT events
//  *
//  *  This function is called by the MQTT client event loop.
//  *
//  * @param handler_args user data registered to the event.
//  * @param base Event base for the handler(always MQTT Base in this example).
//  * @param event_id The id for the received event.
//  * @param event_data The data for the event, esp_mqtt_event_handle_t.
//  */
// static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data,dht11_reading dht11, MQ135 mq135 )
// {
//     ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
//     esp_mqtt_event_handle_t event = event_data;
//     esp_mqtt_client_handle_t client = event->client;
//     int msg_id;
//     switch ((esp_mqtt_event_id_t)event_id) {
//     case MQTT_EVENT_CONNECTED:
//         ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
//         msg_id = esp_mqtt_client_publish(client, "VXL20231/LUAN", sensorData, strlen(sensorData), 0, 0);
//         break;

//     case MQTT_EVENT_DISCONNECTED:
//         ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
//         break;

//     case MQTT_EVENT_SUBSCRIBED:
//         ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//         msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
//         ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
//         break;
//     case MQTT_EVENT_UNSUBSCRIBED:
//         ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
//         break;
//     case MQTT_EVENT_PUBLISHED:
//         ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
//         break;
//     case MQTT_EVENT_DATA:
//         ESP_LOGI(TAG, "MQTT_EVENT_DATA");
//         printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
//         printf("DATA=%.*s\r\n", event->data_len, event->data);
//         break;
//     case MQTT_EVENT_ERROR:
//         ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
//         if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
//             log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
//             log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
//             log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
//             ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

//         }
//         break;
//     default:
//         ESP_LOGI(TAG, "Other event id:%d", event->event_id);
//         break;
//     }
// }

// void mqtt_app_start(void)
// {
//     esp_mqtt_client_config_t mqtt_cfg = {
//         .broker.address.uri = CONFIG_BROKER_URL,
//     };
// #if CONFIG_BROKER_URL_FROM_STDIN
//     char line[128];

//     if (strcmp(mqtt_cfg.broker.address.uri, "FROM_STDIN") == 0) {
//         int count = 0;
//         printf("Please enter url of mqtt broker\n");
//         while (count < 128) {
//             int c = fgetc(stdin);
//             if (c == '\n') {
//                 line[count] = '\0';
//                 break;
//             } else if (c > 0 && c < 127) {
//                 line[count] = c;
//                 ++count;
//             }
//             vTaskDelay(10 / portTICK_PERIOD_MS);
//         }
//         mqtt_cfg.broker.address.uri = line;
//         printf("Broker url: %s\n", line);
//     } else {
//         ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
//         abort();
//     }
// #endif /* CONFIG_BROKER_URL_FROM_STDIN */

//     esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
//     /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
//     esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
//     esp_mqtt_client_start(client);
// }

// void n6_task(void * pvParameter)
// {
//     MQ135 *mq135 = (MQ135 *)malloc(sizeof(MQ135));
//     // Set up I2C
//     i2c_master_init();
//     i2c_port_t i2c_num = I2C_MASTER_NUM;
//     uint8_t address = LCD1602_I2C_ADDRESS;

//     // Set up the SMBus
//     smbus_info_t * smbus_info = smbus_malloc();
//     ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
//     ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_PERIOD_MS));

//     // Set up the LCD1602 device with backlight off
//     i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc();
//     ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
//                                      LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VISIBLE_COLUMNS));

//     ESP_ERROR_CHECK(i2c_lcd1602_reset(lcd_info));

//     //turn off backlight
//     ESP_LOGI(TAG, "backlight off");
//     i2c_lcd1602_set_backlight(lcd_info, false);

//     //turn on backlight
//     ESP_LOGI(TAG, "backlight on");
//     i2c_lcd1602_set_backlight(lcd_info, true);


//     ESP_LOGI(TAG, "cursor on");
//     i2c_lcd1602_set_cursor(lcd_info, true);

//     ESP_LOGI(TAG, "Connecting Wifi");
//     i2c_lcd1602_move_cursor(lcd_info, 0, 0);
//     i2c_lcd1602_write_string(lcd_info, "Connecting Wifi");

//     i2c_lcd1602_set_cursor(lcd_info, false);

//     /* Initialize Wifi */
//     wifi_init_sta(&connection_status);
//     if(connection_status == 1){
//         i2c_lcd1602_clear(lcd_info);
//         i2c_lcd1602_write_string(lcd_info,"Connected Wifi!");
//     }else{
//         i2c_lcd1602_clear(lcd_info);
//         i2c_lcd1602_write_string(lcd_info,"Timeout ");
//         vTaskDelay(500);
//         i2c_lcd1602_clear(lcd_info);
//         i2c_lcd1602_write_string(lcd_info,"Manual Mode");
//     }
//     vTaskDelay(500);
//     i2c_lcd1602_clear(lcd_info);

//     /* Initialize DHT11 */
//     DHT11_init(DHT11_PIN);
//     /* Initialize MQ135 */
//     MQ135_init();
//     /* Initialize Buzzer */
//     gpio_config_t io_conf = {
//         .pin_bit_mask = (1ULL << BUZZER_PIN),
//         .mode = GPIO_MODE_OUTPUT,
//         .intr_type = GPIO_INTR_DISABLE,
//         .pull_down_en = 0,
//         .pull_up_en = 0,
//     }; 
//     gpio_config(&io_conf);
//     /* BEGIN WHILE LOOP */  
//     while(1){
//         // Read data DHT11
//         dht11_reading reading = DHT11_read();
//         if (reading.status == DHT11_OK) {
//             sprintf(temp,"%d",reading.temperature);
//             sprintf(hum,"%d",reading.humidity);
//             ESP_LOGI(TAG, "Nhiet do: %d°C, Do am: %d", reading.temperature, reading.humidity);
//             i2c_lcd1602_move_cursor(lcd_info, 0, 0);
//             i2c_lcd1602_write_string(lcd_info,"RH:");
//             i2c_lcd1602_write_string(lcd_info,hum);
//             i2c_lcd1602_write_char(lcd_info,'%');
//             i2c_lcd1602_move_cursor(lcd_info,8,0);
//             i2c_lcd1602_write_string(lcd_info,temp);
//             i2c_lcd1602_write_string(lcd_info,"oC");
//             // Check condition to enable buzzer to warning fire
//             if (temperature > 55 || humidity < 40) {
//                 gpio_set_level(BUZZER_PIN, 1); 
//             }
//         } else if (reading.status == DHT11_TIMEOUT_ERROR) {
//             ESP_LOGE(TAG, "Timeout reading data from DHT11");
//         } else if (reading.status == DHT11_CRC_ERROR) {
//             ESP_LOGE(TAG, "CRC error reading data from DHT11");
//         }
//         MQ135_readData(mq135->ppm);
//         sprintf(sensorData,"%d,%d,%s", reading.temperature, reading.humidity,mq135->ppm);
//         i2c_lcd1602_move_cursor(lcd_info,0,1);
//         i2c_lcd1602_write_string(lcd_info,"PPM GAS:");
//         i2c_lcd1602_move_cursor(lcd_info,10,1);
//         i2c_lcd1602_write_string(lcd_info,mq135->ppm);
//         /* MQTT*/
//         mqtt_app_start();
//         vTaskDelay(1000);
//     }
// }



