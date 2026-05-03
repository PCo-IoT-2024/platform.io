#ifndef TURBIDITY_SENSOR_MODULE_H
#define TURBIDITY_SENSOR_MODULE_H

#include <Arduino.h>

namespace turbidity {

class TurbiditySensor {
public:
    TurbiditySensor(uint8_t pin,
                    float vcc,
                    float adcMax,
                    float clearVoltage,
                    float clearNTU,
                    float turbidVoltage,
                    float turbidNTU);

    void setup();
    float readADC();
    float readVoltage();
    float getNTU(float temperatureC = 25.0f);
    float getNTUFromADC(float adc, float temperatureC = 25.0f);

private:
    uint8_t pin;
    float vcc;
    float adcMax;

    float clearVoltage;
    float clearNTU;
    float turbidVoltage;
    float turbidNTU;

    float a;
    float b;
};

} // namespace turbidity

#endif // TURBIDITY_SENSOR_MODULE_H
