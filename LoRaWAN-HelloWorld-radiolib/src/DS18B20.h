#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <DallasTemperature.h>
#include <OneWire.h>

namespace temperature {

    class DS18B20 {
    public:
        DS18B20(int8_t pin);

        void setup();

        bool isValid();

        float getTemperature();

    private:
        OneWire oneWire;
        DallasTemperature sensors;

        float temperatureC;
    };

} // namespace temperature

#endif // TEMPERATURE_H
