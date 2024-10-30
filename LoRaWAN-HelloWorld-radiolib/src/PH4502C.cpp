#include "PH4502C.h"

namespace GAIT {

    PH4502C::PH4502C(uint8_t phPin, uint8_t tempPin)
        : ph4502c(phPin, tempPin, PH4502C_DEFAULT_CALIBRATION, PH4502C_DEFAULT_READING_INTERVAL, PH4502C_DEFAULT_READING_COUNT, 4096) {
    }

    void PH4502C::setup(float calibration) {
        // Initialize the sensor
        //        ph4502c.init();
        //        ph4502c.recalibrate(calibration);
    }

    float PH4502C::getPHLevel() {
        // Read pH value
        return ph4502c.read_ph_level_single();
    }

    float PH4502C::getTemperature() {
        return ph4502c.read_temp();
    }

} // namespace GAIT
