#include "PH4502C.h"

namespace GAIT {

    PH4502C::PH4502C(
        uint16_t ph_level_pin, uint16_t temp_pin, float calibration, int reading_interval, int reading_count, float adc_resolution)
        : _ph_level_pin(ph_level_pin)
        , _temp_pin(temp_pin)
        , _calibration(calibration)
        , _reading_interval(reading_interval)
        , _reading_count(reading_count)
        , _adc_resolution(adc_resolution) {
    }

    void PH4502C::init() {
        pinMode(this->_ph_level_pin, INPUT);
        pinMode(this->_temp_pin, INPUT);
    }

    void PH4502C::setup(float calibration) {
        init();
        this->_calibration = calibration;
    }

    float PH4502C::getPHLevel() {
        float reading = 0.0f;

        for (int i = 0; i < this->_reading_count; i++) {
            reading += analogRead(this->_ph_level_pin);
            delayMicroseconds(this->_reading_interval);
        }

        reading = PH4502C_VOLTAGE / this->_adc_resolution * reading;
        reading /= this->_reading_count;
        reading = this->_calibration + ((PH4502C_MID_VOLTAGE - reading)) / PH4502C_PH_VOLTAGE_PER_PH;

        return reading;
    }

    float PH4502C::getPHLevelSingle() {
        float reading = analogRead(this->_ph_level_pin);

        reading = (PH4502C_VOLTAGE / this->_adc_resolution) * reading;

        return this->_calibration + ((PH4502C_MID_VOLTAGE - reading)) / PH4502C_PH_VOLTAGE_PER_PH;
    }

    int PH4502C::read_temp() {
        return analogRead(this->_temp_pin);
    }

} // namespace GAIT
