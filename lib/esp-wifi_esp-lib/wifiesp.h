/**
 * @file wifi_esp.h
 * @author LeThanhLuan_HUST (luan172002@gmail.com)
 * @brief wifi esp32wroom32
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#ifndef _wifiesp_H
#define _wifiesp_H

#pragma once

/**
 * @brief Function: Station mode (aka STA mode or Wi-Fi client mode). ESP32 connects to an access point.
 *         Chế độ này sử dụng ESP32 là trạm (station) kết nối wifi 
 */
void wifi_init_sta(uint8_t *check);

/**
 * @brief Function: AP mode (aka Soft-AP mode or Access Point mode). Stations connect to the ESP32.
 *        Chế độ này sử dụng ESP32 như một trạm phát wifi
 */
void wifi_init_softap(void);

/**
 * @brief Function: Scanning for access points (active & passive scanning).
 *        Hàm này sử dụng để tìm kiếm điểm kết nối wifi chủ động hoặc bị động
 */
void wifi_scan(void);

#endif