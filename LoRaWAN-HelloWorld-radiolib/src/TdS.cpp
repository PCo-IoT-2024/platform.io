#include "TdS.h"

namespace tds {

    TdS::TdS(uint8_t pin, float vcc, uint16_t adcResolution)
        : pin(pin)
        , vcc(vcc)
        , adcResolution(adcResolution) {
    }

    void TdS::setup() {
        gravityTds.setPin(pin);
        gravityTds.setAref(vcc);               // reference voltage on ADC, default 5.0V on Arduino UNO
        gravityTds.setAdcRange(adcResolution); // 1024 for 10bit ADC;4096 for 12bit ADC
        gravityTds.begin();                    // initialization
    }

    float TdS::getValue(float temperature) {
        gravityTds.setTemperature(temperature); // set the temperature and execute temperature compensation
        gravityTds.update();                    // sample and calculate

        const float tdsValue = gravityTds.getTdsValue();

        Serial.println(F("[TDS] ############### TDS ###############"));
        Serial.println("[TDS] TDS value = " + String(tdsValue));

        return tdsValue;
    }

} // namespace tds
