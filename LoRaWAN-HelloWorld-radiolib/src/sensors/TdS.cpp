#include "TdS.h"

#include <Arduino.h>
#include <cmath>

namespace tds {

TdS::TdS(uint8_t pin,
         float lowAdc,
         float lowPpm,
         float highAdc,
         float highPpm)
    : pin(pin),
      lowAdc(lowAdc),
      lowPpm(lowPpm),
      highAdc(highAdc),
      highPpm(highPpm),
      a(0.0f),
      b(0.0f) {
    const float dx = highAdc - lowAdc;

    if (std::fabs(dx) > 0.0001f) {
        a = (highPpm - lowPpm) / dx;
        b = lowPpm - a * lowAdc;
    }
}

void TdS::setup() {
    pinMode(pin, INPUT);
}

float TdS::readADC() {
    constexpr int samples = 20;
    long sum = 0;

    for (int i = 0; i < samples; ++i) {
        sum += analogRead(pin);
        delay(5);
    }

    return static_cast<float>(sum) / static_cast<float>(samples);
}

float TdS::getValueFromADC(float adc, float temperatureC) {
    (void)temperatureC;

    float ppm = a * adc + b;

    if (ppm < 0.0f) {
        ppm = 0.0f;
    }

    return ppm;
}

float TdS::getValue(float temperatureC) {
    const float adc = readADC();
    const float tdsValue = getValueFromADC(adc, temperatureC);

    Serial.println(F("[TDS] ############### TDS ###############"));
    Serial.print(F("[TDS] Raw ADC = "));
    Serial.println(adc, 2);
    Serial.print(F("[TDS] TDS value = "));
    Serial.println(tdsValue, 2);

    return tdsValue;
}

} // namespace tds
