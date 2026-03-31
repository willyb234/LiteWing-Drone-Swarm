/**
 * ESP-Drone Firmware
 *
 * Copyright 2019-2020  Espressif Systems (Shanghai)
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * adc.c - Analog Digital Conversion
 *
 * Ported to ESP-IDF v6.0 ADC oneshot API
 */

#include "esp_idf_version.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_esp32.h"
#include "config.h"
#include "pm_esplane.h"
#include "stm32_legacy.h"
#define DEBUG_MODULE "ADC"
#include "debug_cf.h"

static bool isInit;

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc_cali_handle;
static bool cali_enabled = false;

#ifdef CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_7; //GPIO35 if ADC1
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
static const adc_channel_t channel = ADC_CHANNEL_1;     // GPIO2 if ADC1
#endif

#define NO_OF_SAMPLES   30          //Multisampling

float analogReadVoltage(uint32_t pin)
{
    int adc_reading = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        int raw;
        adc_oneshot_read(adc1_handle, channel, &raw);
        adc_reading += raw;
    }
    adc_reading /= NO_OF_SAMPLES;

    int voltage_mv = 0;
    if (cali_enabled) {
        adc_cali_raw_to_voltage(adc_cali_handle, adc_reading, &voltage_mv);
    } else {
        // Rough estimate if calibration unavailable
        voltage_mv = adc_reading * 2500 / 4095;
    }
    return voltage_mv / 1000.0f;
}

void adcInit(void)
{
    if (isInit) {
        return;
    }

    // Configure ADC1 unit
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1_handle));

    // Configure channel
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel, &chan_cfg));

    // Try to set up calibration
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .chan = channel,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &adc_cali_handle) == ESP_OK) {
        cali_enabled = true;
        printf("ADC calibration: curve fitting\n");
    }
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_line_fitting(&cali_cfg, &adc_cali_handle) == ESP_OK) {
        cali_enabled = true;
        printf("ADC calibration: line fitting\n");
    }
#endif

    if (!cali_enabled) {
        printf("ADC calibration not available, using raw values\n");
    }

    isInit = true;
}

bool adcTest(void)
{
    return isInit;
}
