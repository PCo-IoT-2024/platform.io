#ifndef PH4502C_H
#define PH4502C_H

#include <Arduino.h>
#include <vector>

namespace ph {

struct DataPoint {
    double x;
    double y;
};

#ifndef PH4502C_DEFAULT_READING_INTERVAL
#define PH4502C_DEFAULT_READING_INTERVAL 100
#endif

#ifndef PH4502C_DEFAULT_READING_COUNT
#define PH4502C_DEFAULT_READING_COUNT 10
#endif

class PH4502C {
public:
    PH4502C(uint16_t phLevelPin,
            uint16_t temperaturePin,
            const std::vector<DataPoint>& phAdcDataPoints,
            int readingInterval = PH4502C_DEFAULT_READING_INTERVAL,
            int readingCount = PH4502C_DEFAULT_READING_COUNT);

    void setup();
    float getPHLevel();
    float getPHLevelSingle();
    int readTemp();
    float readADC();

private:
    uint16_t phLevelPin;
    uint16_t temperaturePin;
    int readingInterval;
    int readingCount;

    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
};

} // namespace ph

#endif // PH4502C_H
