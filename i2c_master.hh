#pragma once

#include <string>
#include <esp_log.h>

#include "driver/i2c.h"
#include "driver/i2c_master.h"

using namespace std;

class I2CMaster
{
private:
    gpio_num_t  _i2c_scl;       // i2c clock pin
    gpio_num_t  _i2c_sda;       // i2c data pin
    int         _i2c_port = 0;
    bool        _i2c_enable_pullup = true;

    i2c_master_bus_handle_t _bus_handle;
    
    char *_tag = (char *) "I2C-Master";
    esp_err_t   _err;

    void _init();

public:
    I2CMaster(int clk, int dat);

    esp_err_t   add_device(uint16_t addr, i2c_master_dev_handle_t *_dev_handle);
    esp_err_t   probe(int addr);
};
