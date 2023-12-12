#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include <math.h>
#define MQ135_PIN       3  // Chân GPIO kết nối cảm biến MQ135
#define V_REF           1100.0  // Điện áp tham chiếu của ADC (mV)
#define ADC_RESOLUTION  ADC_WIDTH_BIT_12  // Độ phân giải của ADC

void setup() {
    adc1_config_width(ADC_RESOLUTION);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

float calculatePPM(float voltage) {
    // Tham số từ datasheet của cảm biến MQ135
    float Ro = 33.0;  // Giá trị trở kháng của cảm biến trong không khí là 33 Ohms
    float b = 0.6;    // Hệ số b là 0.6 (lấy từ datasheet)

    // Tính toán trở kháng của cảm biến (Rs) từ điện áp đọc được và điện áp tham chiếu
    float Rs = (V_REF - voltage) / voltage;

    // Tính toán giá trị PPM dựa trên công thức chuyển đổi từ datasheet
    float ppm = Ro * pow((Rs / Ro), b);

    return ppm;
}

void loop() {
    // Đọc giá trị analog từ cảm biến MQ135
    uint32_t adc_value = adc1_get_raw(ADC1_CHANNEL_0);

    // Chuyển đổi giá trị analog sang giá trị PPM
    float voltage = (adc_value / (float)(1 << ADC_RESOLUTION)) * V_REF;
    float ppm = calculatePPM(voltage);

    printf("Giá trị PPM: %.2f\n", ppm);

    vTaskDelay(pdMS_TO_TICKS(1000));  // Đợi 1 giây trước khi đọc lại giá trị
}

void app_main() {
    setup();

    while (1) {
        loop();
    }
}
