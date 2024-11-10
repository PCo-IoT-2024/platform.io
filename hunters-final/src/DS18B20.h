#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <DallasTemperature.h>
#include <OneWire.h>

namespace GAIT {

    class DS18B20 {
    public:
        DS18B20();

        float getTemperature();

    private:
        OneWire oneWire;
    };

} // namespace GAIT

#endif // TEMPERATURE_H
