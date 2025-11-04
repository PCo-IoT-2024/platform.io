#include "TurbiditySensor.h"

namespace GAIT {

TurbiditySensor::TurbiditySensor(
    uint8_t pin,
    float vcc,
    float adcMax,
    float clearVoltage,
    float clearNTU,
    float turbidVoltage,
    float turbidNTU
)
    : pin(pin)
    , vcc(vcc)
    , adcMax(adcMax)
    , V1(clearVoltage)
    , NTU1(clearNTU)
    , V2(turbidVoltage)
    , NTU2(turbidNTU)
    , a(0.0f)
    , b(0.0f)
{
    // Compute linear coefficients
    a = (NTU2 - NTU1) / (V2 - V1);
    b = NTU1 - a * V1;
}

void TurbiditySensor::setup() {
    pinMode(pin, INPUT);

    Serial.println(F("[Turbidity] ############### Turbidity ###############"));
    Serial.print(F("[Turbidity] Pin: ")); Serial.println(pin);
    Serial.print(F("[Turbidity] VCC: ")); Serial.println(vcc, 2);
    Serial.print(F("[Turbidity] ADC Max: ")); Serial.println(adcMax, 1);
    Serial.print(F("[Turbidity] Clear water voltage = ")); Serial.println(V1, 3);
    Serial.print(F("[Turbidity] Turbid water voltage = ")); Serial.println(V2, 3);
    Serial.print(F("[Turbidity] NTU at turbid voltage = ")); Serial.println(NTU2, 1);
    Serial.println(F("[Turbidity] ------------------------------------------"));
}

float TurbiditySensor::readVoltage() {
    const int N = 20;
    long sum = 0;

    for (int i = 0; i < N; ++i) {
        sum += analogRead(pin);
    }

    float average = sum / (float)N;
    return average * (vcc / adcMax);
}

float TurbiditySensor::getNTU(float temperature) {
    (void)temperature; // placeholder for future compensation

    float voltage = readVoltage();
    float ntu = a * voltage + b;

    if (ntu < 0)
        ntu = 0;

    Serial.print(F("[Turbidity] Raw Voltage = "));
    Serial.print(voltage, 3);
    Serial.print(F(" V  |  NTU = "));
    Serial.println(ntu, 2);

    return ntu;
}

} // namespace GAIT
