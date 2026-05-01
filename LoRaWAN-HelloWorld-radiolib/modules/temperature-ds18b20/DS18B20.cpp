#include "DS18B20.h"

#include <Arduino.h>
#include <cmath>

namespace temperature {

DS18B20::DS18B20(int8_t pin)
    : oneWire(pin), sensors(&oneWire) {
}

void DS18B20::setup() {
    sensors.begin();
    sensors.requestTemperatures();
    delay(750);
    temperatureC = sensors.getTempCByIndex(0);
}

bool DS18B20::isValid() const {
    return temperatureC != DEVICE_DISCONNECTED_C &&
           temperatureC > -55.0f &&
           temperatureC < 125.0f &&
           !std::isnan(temperatureC);
}

float DS18B20::getTemperature() const {
    Serial.println(F("[TEMP] ############### TEMP ###############"));
    Serial.print(F("[TEMP] Temperature = "));
    Serial.println(temperatureC);
    return temperatureC;
}

} // namespace temperature
