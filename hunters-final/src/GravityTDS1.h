#ifndef GRAVITYDTS_H
#define GRAVITYDTS_H

#include <GravityTDS.h>
#include <cstdint>

namespace GAIT {

    class GravityTDS {
    public:
        GravityTDS(uint8_t pin, float vcc, uint16_t adcResoluton);
        void setup();
        float getValue(float temperature);

    private:
        uint8_t pin;
        float vcc;
        uint16_t adcResolution;
        ::GravityTDS gravityTds;
    };

} // namespace GAIT
#endif // GRAVITYDTS_H