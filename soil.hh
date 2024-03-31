#pragma once

#include <esp_log.h>

#include "driver/i2c.h"
#include "driver/i2c_master.h"

// Adafruit Soil Moisture sensor via SeeSaw i2c device
class Soil
{
private:
    gpio_num_t  _i2c_scl;       // i2c clock pin
    gpio_num_t  _i2c_sda;       // i2c data pin
    uint16_t    _i2c_addr = 0x36;   // i2c address
    esp_err_t   _err = ESP_OK;

    i2c_master_bus_handle_t _bus_handle;
    i2c_master_dev_handle_t _dev_handle;

    char *_tag = (char *) "SOIL";

    void _init();

public:
    Soil(gpio_num_t clk, gpio_num_t dat, int addr = 0);
    Soil(int clk, int dat, int addr = 0) : Soil((gpio_num_t) clk, (gpio_num_t) dat, addr) {}

    bool probe();

    int16_t     get_moisture();
    float       get_tempc();
    float       get_tempf();

    esp_err_t   get_error() { return _err; }
    void        reset_error() { _err = ESP_OK; }
};
