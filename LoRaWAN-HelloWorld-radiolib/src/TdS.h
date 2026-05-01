#ifndef GRAVITYDTS_H
#define GRAVITYDTS_H

#include <GravityTDS.h>
#include <cstdint>

namespace tds {

    class TdS {
    public:
        TdS(uint8_t pin, float vcc, uint16_t adcResoluton);
        void setup();
        float getValue(float temperature);

    private:
        uint8_t pin;
        float vcc;
        uint16_t adcResolution;
        GravityTDS gravityTds;
    };

} // namespace tds

#endif // GRAVITYDTS_H
