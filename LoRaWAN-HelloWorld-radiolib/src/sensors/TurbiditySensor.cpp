#include "TurbiditySensor.h"

namespace turbidity {

TurbiditySensor::TurbiditySensor(uint8_t pin,
                                 float vcc,
                                 float adcMax,
                                 float clearVoltage,
                                 float clearNTU,
                                 float turbidVoltage,
                                 float turbidNTU)
    : pin(pin),
      vcc(vcc),
      adcMax(adcMax),
      clearVoltage(clearVoltage),
      clearNTU(clearNTU),
      turbidVoltage(turbidVoltage),
      turbidNTU(turbidNTU),
      a(0.0f),
      b(0.0f) {
    a = (turbidNTU - clearNTU) / (turbidVoltage - clearVoltage);
    b = clearNTU - a * clearVoltage;
}

void TurbiditySensor::setup() {
    pinMode(pin, INPUT);

    Serial.println(F("[TURBIDITY] ############### TURBIDITY ###############"));
    Serial.print(F("[TURBIDITY] Pin: "));
    Serial.println(pin);
    Serial.print(F("[TURBIDITY] Clear water voltage = "));
    Serial.println(clearVoltage, 3);
    Serial.print(F("[TURBIDITY] Turbid water voltage = "));
    Serial.println(turbidVoltage, 3);
}

float TurbiditySensor::readVoltage() {
    constexpr int samples = 20;
    long sum = 0;

    for (int i = 0; i < samples; ++i) {
        sum += analogRead(pin);
    }

    const float average = static_cast<float>(sum) / static_cast<float>(samples);
    return average * (vcc / adcMax);
}

float TurbiditySensor::getNTU(float temperatureC) {
    (void)temperatureC;

    const float voltage = readVoltage();
    float ntu = a * voltage + b;

    if (ntu < 0.0f) {
        ntu = 0.0f;
    }

    Serial.print(F("[TURBIDITY] Raw voltage = "));
    Serial.print(voltage, 3);
    Serial.print(F(" V, NTU = "));
    Serial.println(ntu, 2);

    return ntu;
}

} // namespace turbidity
