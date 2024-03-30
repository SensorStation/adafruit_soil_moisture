#include <unistd.h>

#include "soil.hh"

Soil::Soil(gpio_num_t clk, gpio_num_t dat, int addr)
{
    _i2c_scl = clk;
    _i2c_sda = dat;

    if (addr != 0) {
        _i2c_addr = addr;
    }

    _init();
}

void Soil::_init()
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = 0;
    i2c_mst_config.scl_io_num = _i2c_scl;
    i2c_mst_config.sda_io_num = _i2c_sda;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = true;

    _err = i2c_new_master_bus(&i2c_mst_config, &_bus_handle);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "error adding new master bus: %d", _err);
        return;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = _i2c_addr,
        .scl_speed_hz = 100000,
    };

    _err = i2c_master_bus_add_device(_bus_handle, &dev_cfg, &_dev_handle);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "master bus add device: %d", _err);
        return;
    }
}

bool Soil::probe()
{
    _err = i2c_master_probe(_bus_handle, _i2c_addr, -1);
    return (_err == ESP_OK);
}

int16_t Soil::get_moisture()
{
    int dlen = 2;
    uint8_t rbuf[2], wbuf[2];

    wbuf[0] = { 0x0f };         // capacitive touch base
    wbuf[1] = { 0x10 };         // touch register offset

    _err = i2c_master_transmit(_dev_handle, wbuf, dlen, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to transmit data: %d", _err);
        return -1;
    }

    usleep(3000);

    _err = i2c_master_receive(_dev_handle, rbuf, dlen, -1);
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

    _err = i2c_master_transmit(_dev_handle, wbuf, 2, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to transmit data: %d", _err);
        return -1;
    }

    usleep(1000);               // give the device a chance to respond

    _err = i2c_master_receive(_dev_handle, rbuf, dlen, -1);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "Failed to recv data: %d", _err);
        return -1;
    }

    // for (int i = 0; i < dlen; i++) {
    //     printf("buf[%d] => %02x\n", i, rbuf[i]);
    // }

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
