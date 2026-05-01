#ifndef GRAVITY_TDS_MODULE_H
#define GRAVITY_TDS_MODULE_H

#include <GravityTDS.h>
#include <cstdint>

namespace tds {

class TdS {
public:
    TdS(uint8_t pin, float vcc, uint16_t adcResolution);

    void setup();
    float getValue(float temperatureC);

private:
    uint8_t pin;
    float vcc;
    uint16_t adcResolution;
    GravityTDS gravityTds;
};

} // namespace tds

#endif // GRAVITY_TDS_MODULE_H
