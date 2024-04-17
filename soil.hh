#pragma once

#include <esp_log.h>
#include <esp_err.h>

// Adafruit Soil Moisture sensor via SeeSaw i2c device
class Soil
{
private:
    I2CMaster*                  _i2c_master;
    uint16_t                    _i2c_addr = 0x36;   // i2c address
    i2c_master_dev_handle_t     _i2c_dev_handle;

    esp_err_t   _err = ESP_OK;
    char*       _tag = (char *) "SOIL";

public:
    Soil(I2CMaster *i2c, int addr);

    int16_t     get_moisture();
    float       get_tempc();
    float       get_tempf();

    esp_err_t   get_error() { return _err; }
    void        reset_error() { _err = ESP_OK; }
};
