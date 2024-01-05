#ifndef _MQ135_H
#define _MQ135_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
/// For details about the parameters below, see:
/// http://davidegironi.blogspot.com/2014/01/cheap-co2-meter-using-mq135-sensor-with.html
/// https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library

/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

/// Parameters to model temperature and humidity dependence
#define CORA .00035
#define CORB .02718
#define CORC 1.39538
#define CORD .0018
#define CORE -.003333333
#define CORF -.001923077
#define CORG 1.130128205

/// Atmospheric CO2 level for calibration purposes,
/// from "Globally averaged marine surface monthly mean data"
/// available at https://gml.noaa.gov/ccgg/trends/gl_data.html
#define ATMOCO2 416.83 // Global CO2 September 2023

typedef struct{
    bool cali;
    char ppm[10];
}MQ135;

void MQ135_init(void);
void MQ135_readData(char *read);
float getResistance();
float getPPM();

#endif
