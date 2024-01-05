#ifndef __DEVICEMANAGER_H__
#define __DEVICEMANAGER_H__

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "string.h"
#include "time.h"


typedef enum {
    DISCONNECTED = 0,
    CONNECTED,
    CONNECTING,
    NOT_FOUND,
} status_t;

struct statusDevice_st
{
    status_t wifi;
    // status_t sdcard;
    // status_t ds3231Module;
    status_t dht11Sensor;
    status_t mq135Sensor;
    status_t mhz14a;
    //status_t mqttClient;
};

struct moduleError_st
{
    // uint64_t timestamp;
    // esp_err_t sdError;
    // esp_err_t ds3231Error;
    esp_err_t dht11Error;
    esp_err_t mq135Error;
    // esp_err_t mhz14aError;
};


#endif
