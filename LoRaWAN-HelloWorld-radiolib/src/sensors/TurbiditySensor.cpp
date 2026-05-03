#include "TurbiditySensor.h"

#include <cmath>

namespace turbidity {

TurbiditySensor::TurbiditySensor(uint8_t pin,
                                 float clearAdc,
                                 float clearNTU,
                                 float turbidAdc,
                                 float turbidNTU)
    : pin(pin),
      clearAdc(clearAdc),
      clearNTU(clearNTU),
      turbidAdc(turbidAdc),
      turbidNTU(turbidNTU),
      a(0.0f),
      b(0.0f) {
    const float dx = turbidAdc - clearAdc;

    if (std::fabs(dx) > 0.0001f) {
        a = (turbidNTU - clearNTU) / dx;
        b = clearNTU - a * clearAdc;
    }
}

void TurbiditySensor::setup() {
    pinMode(pin, INPUT);

    Serial.println(F("[TURBIDITY] ############### TURBIDITY ###############"));
    Serial.print(F("[TURBIDITY] Pin: "));
    Serial.println(pin);
    Serial.print(F("[TURBIDITY] Clear water ADC = "));
    Serial.println(clearAdc, 2);
    Serial.print(F("[TURBIDITY] Turbid water ADC = "));
    Serial.println(turbidAdc, 2);
}

float TurbiditySensor::readADC() {
    constexpr int samples = 20;
    long sum = 0;

    for (int i = 0; i < samples; ++i) {
        sum += analogRead(pin);
        delay(5);
    }

    return static_cast<float>(sum) / static_cast<float>(samples);
}

float TurbiditySensor::getNTUFromADC(float adc, float temperatureC) {
    (void)temperatureC;

    float ntu = a * adc + b;

    if (ntu < 0.0f) {
        ntu = 0.0f;
    }

    return ntu;
}

float TurbiditySensor::getNTU(float temperatureC) {
    const float adc = readADC();
    const float ntu = getNTUFromADC(adc, temperatureC);

    Serial.print(F("[TURBIDITY] Raw ADC = "));
    Serial.print(adc, 2);
    Serial.print(F(", NTU = "));
    Serial.println(ntu, 2);

    return ntu;
}

} // namespace turbidity
