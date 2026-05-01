#include "TdS.h"

#include <Arduino.h>

namespace tds {

TdS::TdS(uint8_t pin, float vcc, uint16_t adcResolution)
    : pin(pin), vcc(vcc), adcResolution(adcResolution) {
}

void TdS::setup() {
    gravityTds.setPin(pin);
    gravityTds.setAref(vcc);
    gravityTds.setAdcRange(adcResolution);
    gravityTds.begin();
}

float TdS::getValue(float temperatureC) {
    gravityTds.setTemperature(temperatureC);
    gravityTds.update();

    const float tdsValue = gravityTds.getTdsValue();

    Serial.println(F("[TDS] ############### TDS ###############"));
    Serial.print(F("[TDS] Temperature compensation = "));
    Serial.println(temperatureC);
    Serial.print(F("[TDS] TDS value = "));
    Serial.println(tdsValue);

    return tdsValue;
}

} // namespace tds
