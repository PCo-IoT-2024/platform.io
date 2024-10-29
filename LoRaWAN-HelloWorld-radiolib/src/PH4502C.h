#ifndef PH4502C_H
#define PH4502C_H

#include <cstdint>
#include <ph4502c_sensor.h>

namespace GAIT {

    class PH4502C {
    public:
        PH4502C(uint8_t phPin, uint8_t tempPin);

        void setup();

        float getPHLevel();

    private:
        PH4502C_Sensor ph4502c;
    };

} // namespace GAIT

#endif // PH4502C_H
