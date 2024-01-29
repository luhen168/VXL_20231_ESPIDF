#ifndef _DHT11_H_
#define _DHT11_H_

#include "driver/gpio.h"

//Định nghĩa kiểu liệt kê với tên là dht11_status, biểu diễn các trạng thái của DHT11
enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

//Định nghĩa 1 cấu trúc có tên là dht11_reading, chứa thông tin đọc từ cảm biến
typedef struct dht11_reading {
    int status;
    int temperature;
    int humidity;
} dht11_reading;

//Hàm khởi tạo DHT11
void DHT11_init(gpio_num_t gpio_num);

//Hàm đọc dữ liệu cảm biến
dht11_reading DHT11_read();

#endif