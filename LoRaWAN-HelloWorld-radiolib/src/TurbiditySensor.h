#ifndef TURBIDITY_SENSOR_H
#define TURBIDITY_SENSOR_H

#include <Arduino.h>

namespace GAIT {

    class TurbiditySensor {
    public:
        TurbiditySensor(uint8_t pin, float vcc, float adcMax, float clearVoltage, float clearNTU, float turbidVoltage, float turbidNTU);

        void setup();
        float readVoltage();
        float getNTU(float temperature = 25.0f);

    private:
        uint8_t pin;
        float vcc;
        float adcMax;

        // Calibration values
        float V1;
        float NTU1;
        float V2;
        float NTU2;

        // Linear coefficients
        float a;
        float b;
    };

} // namespace GAIT

#endif // TURBIDITY_SENSOR_H
