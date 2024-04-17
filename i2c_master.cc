#include "i2c_master.hh"

I2CMaster::I2CMaster(int clk, int dat)
{
    _i2c_scl = (gpio_num_t) clk;
    _i2c_sda = (gpio_num_t) dat;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = _i2c_port; // make port a variable
    i2c_mst_config.scl_io_num = _i2c_scl;
    i2c_mst_config.sda_io_num = _i2c_sda;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = _i2c_enable_pullup;

    _err = i2c_new_master_bus(&i2c_mst_config, &_bus_handle);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "error adding new master bus: %d", _err);
        return;
    }
    printf("0 BUS Handle %p\n", _bus_handle);
}

esp_err_t I2CMaster::add_device(uint16_t addr, i2c_master_dev_handle_t *dev_handle)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };

    _err = i2c_master_bus_add_device(_bus_handle, &dev_cfg, dev_handle);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "master bus add device: %d", _err);
        return _err;
    }
    _err = probe(addr);
    if (_err != ESP_OK) {
        ESP_LOGE(_tag, "probe is %0x", _err);        
    }
    return _err;
}

esp_err_t I2CMaster::probe(int addr)
{
    return i2c_master_probe(_bus_handle, addr, -1);
}



