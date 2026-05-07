#ifndef TEMPERATURE_DS18B20_H
#define TEMPERATURE_DS18B20_H

#include <DallasTemperature.h>
#include <OneWire.h>

namespace temperature {

class DS18B20 {
public:
    explicit DS18B20(int8_t pin);

    void setup();
    bool isValid() const;
    float getTemperature() const;

private:
    OneWire oneWire;
    DallasTemperature sensors;
    float temperatureC = DEVICE_DISCONNECTED_C;
};

} // namespace temperature

#endif // TEMPERATURE_DS18B20_H
