#include "PH4502C.h"

#include <cmath>

namespace ph {

static bool quadraticFit(const std::vector<DataPoint>& data, double& a, double& b, double& c) {
    if (data.size() < 3) {
        return false;
    }

    const double A[3][3] = {
        {data[0].x * data[0].x, data[0].x, 1.0},
        {data[1].x * data[1].x, data[1].x, 1.0},
        {data[2].x * data[2].x, data[2].x, 1.0},
    };

    const double B[3] = {data[0].y, data[1].y, data[2].y};

    auto determinant = [](const double matrix[3][3]) -> double {
        return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
               matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
               matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    };

    const double D = determinant(A);
    if (std::fabs(D) < 1e-12) {
        return false;
    }

    const double Aa[3][3] = {{B[0], A[0][1], A[0][2]}, {B[1], A[1][1], A[1][2]}, {B[2], A[2][1], A[2][2]}};
    const double Ab[3][3] = {{A[0][0], B[0], A[0][2]}, {A[1][0], B[1], A[1][2]}, {A[2][0], B[2], A[2][2]}};
    const double Ac[3][3] = {{A[0][0], A[0][1], B[0]}, {A[1][0], A[1][1], B[1]}, {A[2][0], A[2][1], B[2]}};

    a = determinant(Aa) / D;
    b = determinant(Ab) / D;
    c = determinant(Ac) / D;
    return true;
}

PH4502C::PH4502C(uint16_t phLevelPin,
                 uint16_t temperaturePin,
                 const std::vector<DataPoint>& phAdcDataPoints,
                 float temperatureLowAdc,
                 float temperatureLowC,
                 float temperatureHighAdc,
                 float temperatureHighC,
                 int readingInterval,
                 int readingCount)
    : phLevelPin(phLevelPin),
      temperaturePin(temperaturePin),
      readingInterval(readingInterval),
      readingCount(readingCount),
      temperatureLowAdc(temperatureLowAdc),
      temperatureLowC(temperatureLowC),
      temperatureHighAdc(temperatureHighAdc),
      temperatureHighC(temperatureHighC) {
    if (!quadraticFit(phAdcDataPoints, a, b, c)) {
        Serial.println(F("[PH] Calibration fit failed; sensor readings may be invalid"));
    }

    const float dx = temperatureHighAdc - temperatureLowAdc;
    if (std::fabs(dx) > 0.0001f) {
        temperatureA = (temperatureHighC - temperatureLowC) / dx;
        temperatureB = temperatureLowC - temperatureA * temperatureLowAdc;
    } else {
        Serial.println(F("[PH] Temperature calibration fit failed; board temperature readings may be invalid"));
    }
}

void PH4502C::setup() {
    pinMode(phLevelPin, INPUT);
    pinMode(temperaturePin, INPUT);
}

float PH4502C::readADC() {
    float reading = 0.0f;

    for (int i = 0; i < readingCount; ++i) {
        reading += analogRead(phLevelPin);
        delayMicroseconds(readingInterval);
    }

    return reading / static_cast<float>(readingCount);
}

float PH4502C::getPHLevelFromADC(float adc) const {
    return static_cast<float>(a * adc * adc + b * adc + c);
}

float PH4502C::getPHLevel() {
    const float reading = readADC();
    const float phValue = getPHLevelFromADC(reading);

    Serial.println(F("[PH] ############### PH ###############"));
    Serial.print(F("[PH] Raw ADC = "));
    Serial.println(reading);
    Serial.print(F("[PH] Calibrated pH value = "));
    Serial.println(phValue);

    return phValue;
}

float PH4502C::getPHLevelSingle() {
    const float reading = analogRead(phLevelPin);
    const float phValue = getPHLevelFromADC(reading);

    Serial.println(F("[PH] ############### PH ###############"));
    Serial.print(F("[PH] Raw ADC = "));
    Serial.println(reading);
    Serial.print(F("[PH] Calibrated pH value = "));
    Serial.println(phValue);

    return phValue;
}

float PH4502C::readTemperatureADC() {
    float reading = 0.0f;

    for (int i = 0; i < readingCount; ++i) {
        reading += analogRead(temperaturePin);
        delayMicroseconds(readingInterval);
    }

    return reading / static_cast<float>(readingCount);
}

float PH4502C::getBoardTemperatureCFromADC(float adc) const {
    return temperatureA * adc + temperatureB;
}

float PH4502C::getBoardTemperatureC() {
    const float adc = readTemperatureADC();
    const float temperatureC = getBoardTemperatureCFromADC(adc);

    Serial.println(F("[PH-TEMP] ############### PH BOARD TEMPERATURE ###############"));
    Serial.print(F("[PH-TEMP] Raw ADC = "));
    Serial.println(adc);
    Serial.print(F("[PH-TEMP] Calibrated temperature C = "));
    Serial.println(temperatureC);

    return temperatureC;
}

} // namespace ph
