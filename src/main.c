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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "LCD.h"

char buffer[20]="Hello World!";


void app_main(void)
{
    lcd_init();
    lcd_clear();
    lcd_send_string(buffer);
}
