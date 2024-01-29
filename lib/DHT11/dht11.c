#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"

static gpio_num_t dht_gpio;
static int64_t last_read_time = -1000000;
static dht11_reading last_read;

/**
 * @brief Hàm khởi tạo chân DHT11_init()
 */
void DHT11_init(gpio_num_t gpio_num) {
    dht_gpio = gpio_num;
}

/**
 * @brief Hàm đọc dữ liệu cảm biến DHT11_read()
 *        Gồm các hàm con sau: _waitOrTimeout,_checkCRC,_sendStartSignal
 *                             _checkResponse,_timeoutError,_crcError
 */
        
// Hàm đợi hoặc waitortimeout: dùng để đợi đến khi chân GPIO đạt được mức logic mong muốn hoặc hết thời gian timeout    
static int _waitOrTimeout(uint16_t microSeconds, int level) {
    int micros_ticks = 0;
    while(gpio_get_level(dht_gpio) == level) { 
        if(micros_ticks++ > microSeconds) 
            return DHT11_TIMEOUT_ERROR;
        ets_delay_us(1);
    }
    return micros_ticks;
}

// CheckCRC: Kiểm tra tính đúng đắn của dữ liệu bằng cách so sánh giá trị CRC với tổng 4 byte trước
static int _checkCRC(uint8_t data[]) {
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT11_OK;
    else
        return DHT11_CRC_ERROR;
}

// Hàm gửi tín hiệu bắt đầu cho cảm biến DHT11
static void _sendStartSignal() {
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

// Hàm kiểm tra phản hồi từ cảm biến DHT11 sau khi gửi tín hiệu bắt đầu
static int _checkResponse() {
    /* Wait for next step ~80us*/
    if(_waitOrTimeout(80, 0) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    /* Wait for next step ~80us*/
    if(_waitOrTimeout(80, 1) == DHT11_TIMEOUT_ERROR) 
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}

// Trả về 1 cấu trúc dht11_reading cho lỗi timeout
static dht11_reading _timeoutError() {
    dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

// Trả về 1 cấu trúc dht11_reading cho lỗi CRC
static dht11_reading _crcError() {
    dht11_reading crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}

// Hàm đọc cảm biến DHT11
dht11_reading DHT11_read() {
    /* Khai báo và định nghĩa hàm DHT11_read trả về một cấu trúc DHT11_reading
    Kiểm tra xem đã qua ít nhất 1s kể từ lần đọc cuối chưa.*/
    if(esp_timer_get_time() - 1000000 < last_read_time) {
        return last_read;
    }

    //Ghi lại thời điểm hiện tại và khai báo một mảng data với 5 phần tử để lưu dữ liệu đọc từ cảm biến.
    last_read_time = esp_timer_get_time(); 
    uint8_t data[5] = {0,0,0,0,0};

    /* Gửi tín hiệu bắt đầu và kiểm tra phản hồi từ cảm biến. */
    _sendStartSignal();
    // Phần phản hồi (response) sau bước trên
    if(_checkResponse() == DHT11_TIMEOUT_ERROR)
        return last_read = _timeoutError();
    
    /* Sử dụng vòng lặp để đọc 40 bit dữ liệu từ cảm biến */
    for(int i = 0; i < 40; i++) {
        // Chờ tín hiệu thấp, kiểm tra lỗi thời gian chờ và trả về giá trị lỗi nếu có 
        if(_waitOrTimeout(50, 0) == DHT11_TIMEOUT_ERROR)
            return last_read = _timeoutError();
        // Chờ tín hiệu cao, thời gian chờ lớn hơn 28, thiết lập bit tương ứng của data thành 1        
        if(_waitOrTimeout(70, 1) > 28) {
            /* Bit received was a 1 */
            data[i/8] |= (1 << (7-(i%8)));
        }
    }

    // Kiểm tra tính chính xác của dữ liệu bằng cách so sánh với giá trị kiểm tra CRC
    if(_checkCRC(data) != DHT11_CRC_ERROR) {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    } else {
        return last_read = _crcError();
    }
}
