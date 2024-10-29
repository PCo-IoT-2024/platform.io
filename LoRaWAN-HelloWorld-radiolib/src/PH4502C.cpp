#include "PH4502C.h"

namespace GAIT {

    PH4502C::PH4502C(uint8_t phPin, uint8_t tempPin)
        : ph4502c(phPin, tempPin) {
    }

    void PH4502C::setup() {
        // Initialize the sensor
        ph4502c.init();
    }

    float PH4502C::getPHLevel() {
        // Read pH value
        return ph4502c.read_ph_level();
    }

} // namespace GAIT
