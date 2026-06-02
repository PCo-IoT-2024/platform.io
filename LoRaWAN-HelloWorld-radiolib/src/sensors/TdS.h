#ifndef GRAVITY_TDS_MODULE_H
#define GRAVITY_TDS_MODULE_H

#include <cstdint>

namespace tds {

class TdS {
public:
    TdS(uint8_t pin,
        float lowAdc,
        float lowPpm,
        float highAdc,
        float highPpm);

    void setup();
    float readADC();
    float getValue(float temperatureC);
    float getValueFromADC(float adc, float temperatureC);

private:
    uint8_t pin;
    float lowAdc;
    float lowPpm;
    float highAdc;
    float highPpm;
    float a;
    float b;
};

} // namespace tds

#endif // GRAVITY_TDS_MODULE_H
