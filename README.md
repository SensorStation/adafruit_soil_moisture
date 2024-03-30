# adafruit_soilmoisture
Adafruit Capacitive Soil Moisture Sensor for esp32 as an esp-idf component. 

This Adafruit Capacitive Soil Moisture sensor, unlike most others on the market is an I2C peripherial (and it works!).

The I2C interface is based on the Adafruit I2C SeeSaw module. The code here takes just enough of the SeeSaw library code / algorithm really to implement the capacitive touch sensor and the temprature directly as an esp-idf component vs. the Arduino version supplied directly by the wonderful people at Adafruit.

## Example Usage

```c++
/*
 * main.cc example for the Adafruit Soil Moisture sensor
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <iostream>
#include <thread>
#include <string.h>
#include <esp_log.h>

#include "soil.hh"

using namespace std::chrono;

const char *TAG = "MAIN";

const int i2c_sdc = 19;         // i2c clock on pin 19
const int i2c_sda = 18;         // i2c clock on pin 18
const int i2c_addr = 0x36;      // i2c default address of soil moisture sensor

const auto sleep_time = seconds
{
    5
};

extern "C" void app_main(void)
{
    // Initialize the sensor
    Soil *soil = new Soil(i2c_sdc, i2c_sda, i2c_addr);

    // Keep probing until we find the sensor on the i2c bus
    for (bool found = false; found; found = soil->probe()) {
        ESP_LOGD(TAG, "soil probe NOT found");
        sleep(1);
    }
    ESP_LOGD(TAG, "soil probe found");

    // Loop around grabbing moisture and temprature until the cows come home 
    while (true) {

        int16_t m = soil->get_moisture();
        if (m < 0)  {
            ESP_LOGE(TAG, "Failed to get moisture");
            continue;
        }
        ESP_LOGI(TAG, "Soil: %d", m);

        float t = soil->get_tempf();
        if (t < 0)  {
            ESP_LOGE(TAG, "Failed to get temprature");
            continue;
        }
        ESP_LOGI(TAG, "Temp: %5.2f", t);
        std::this_thread::sleep_for(sleep_time);
    }
}
```
