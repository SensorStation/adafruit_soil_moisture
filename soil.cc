#include <unistd.h>

#include <esp_log.h>

#include "i2c_master.hh"
#include "soil.hh"

Soil::Soil(I2CMaster *i2c, int addr)
{
    _i2c_addr = addr;
    _i2c_master = i2c;

    _err = i2c->add_device(_i2c_addr, &_i2c_dev_handle);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "failed to initialize i2c soil device");
    }
}

int16_t Soil::get_moisture()
{
    int dlen = 2;
    uint8_t rbuf[2], wbuf[2];

    wbuf[0] = { 0x0f };         // capacitive touch base
    wbuf[1] = { 0x10 };         // touch register offset

    _err = i2c_master_transmit(_i2c_dev_handle, wbuf, dlen, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to transmit data: %d", _err);
        return -1;
    }

    usleep(3000);

    _err = i2c_master_receive(_i2c_dev_handle, rbuf, dlen, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to recv data: %d", _err);
        return -1;
    }
    uint16_t val = ((uint16_t) rbuf[0] << 8) | rbuf[1];
    return val;
}

float Soil::get_tempc()
{
    int dlen = 4;
    uint8_t rbuf[4], wbuf[2];

    wbuf[0] = { 0x00 };         // seesaw status base
    wbuf[1] = { 0x04 };         // temp offset

    _err = i2c_master_transmit(_i2c_dev_handle, wbuf, 2, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to transmit data: %d", _err);
        return -1;
    }

    usleep(1000);               // give the device a chance to respond

    _err = i2c_master_receive(_i2c_dev_handle, rbuf, dlen, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to recv data: %d", _err);
        return -1;
    }

    uint32_t val =
        ((uint32_t) rbuf[0] << 24) |
        ((uint32_t) rbuf[1] << 16) |
        ((uint32_t) rbuf[2] << 8) |
        (uint32_t)  rbuf[3];
    return (1.0 / (1UL << 16)) * val;
}

float Soil::get_tempf()
{
    float c = get_tempc();
    float f = (c * (9/5)) + 32;
    return f;
}
