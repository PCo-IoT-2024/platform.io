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
            float temperatureLowAdc,
            float temperatureLowC,
            float temperatureHighAdc,
            float temperatureHighC,
            int readingInterval = PH4502C_DEFAULT_READING_INTERVAL,
            int readingCount = PH4502C_DEFAULT_READING_COUNT);

    void setup();
    float readADC();
    float getPHLevel();
    float getPHLevelSingle();
    float getPHLevelFromADC(float adc) const;
    float readTemperatureADC();
    float getBoardTemperatureC();
    float getBoardTemperatureCFromADC(float adc) const;

private:
    uint16_t phLevelPin;
    uint16_t temperaturePin;
    int readingInterval;
    int readingCount;

    double a = 0.0;
    double b = 0.0;
    double c = 0.0;

    float temperatureLowAdc;
    float temperatureLowC;
    float temperatureHighAdc;
    float temperatureHighC;
    float temperatureA = 0.0f;
    float temperatureB = 0.0f;
};

} // namespace ph

#endif // PH4502C_H
