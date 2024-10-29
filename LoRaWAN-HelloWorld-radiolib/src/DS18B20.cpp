#include "DS18B20.h"

namespace GAIT {

    DS18B20::DS18B20()
        : oneWire(DALLAS_TEMPERATURE_PIN) {
    }

    float DS18B20::getTemperature() {
        DallasTemperature sensors(&oneWire);

        sensors.begin();
        sensors.requestTemperatures();

        return sensors.getTempCByIndex(0);
    }

} // namespace GAIT
