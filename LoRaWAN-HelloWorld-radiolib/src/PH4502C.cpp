#include "PH4502C.h"

#include <cmath>
#include <vector>

namespace GAIT {

    void calculateQuadraticFit(const std::vector<DataPoint>& data, double& a, double& b, double& c) {
        int n = data.size();
        double sum_x = 0, sum_y = 0;
        double sum_x2 = 0, sum_x3 = 0, sum_x4 = 0;
        double sum_xy = 0, sum_x2y = 0;

        // Calculate the necessary sums
        for (const auto& point : data) {
            double x = point.x;
            double y = point.y;
            double x2 = x * x;
            double x3 = x2 * x;
            double x4 = x3 * x;

            sum_x += x;
            sum_y += y;
            sum_x2 += x2;
            sum_x3 += x3;
            sum_x4 += x4;
            sum_xy += x * y;
            sum_x2y += x2 * y;
        }

        // Create the system of linear equations
        double D = n * (sum_x2 * sum_x4 - sum_x3 * sum_x3) - sum_x * (sum_x * sum_x4 - sum_x2 * sum_x3) +
                   sum_x2 * (sum_x * sum_x3 - sum_x2 * sum_x2);

        double Da = n * (sum_x2 * sum_x2y - sum_x3 * sum_xy) - sum_x * (sum_x * sum_x2y - sum_x2 * sum_xy) +
                    sum_y * (sum_x * sum_x3 - sum_x2 * sum_x2);

        double Db = n * (sum_xy * sum_x4 - sum_x2y * sum_x3) - sum_y * (sum_x * sum_x4 - sum_x2 * sum_x3) +
                    sum_x2 * (sum_x * sum_x2y - sum_xy * sum_x2);

        double Dc = sum_y * (sum_x2 * sum_x4 - sum_x3 * sum_x3) - sum_x * (sum_xy * sum_x4 - sum_x2y * sum_x3) +
                    sum_x2 * (sum_xy * sum_x3 - sum_x2y * sum_x2);

        // Calculate coefficients a, b, and c
        a = Da / D;
        b = Db / D;
        c = Dc / D;
    }

    // Function to calculate x from y using the quadratic formula
    // ax^2 + bx + (c - y) = 0
    double calculateX(double y, double a, double b, double c) {
        double discriminant = b * b - 4 * a * (c - y);
        if (discriminant < 0) {
            return NAN;
        }
        // Two possible solutions for x
        double x1 = (-b + std::sqrt(discriminant)) / (2 * a);
        double x2 = (-b - std::sqrt(discriminant)) / (2 * a);

        // Return the positive solution for x
        return (x1 >= 0) ? x1 : x2;
    }

    double PH4502C::a;
    double PH4502C::b;
    double PH4502C::c;

    PH4502C::PH4502C(
        uint16_t ph_level_pin, uint16_t temp_pin, const std::vector<DataPoint>& phAdcDataPoints, int reading_interval, int reading_count)
        : _ph_level_pin(ph_level_pin)
        , _temp_pin(temp_pin)
        , _reading_interval(reading_interval)
        , _reading_count(reading_count) {
        calculateQuadraticFit(phAdcDataPoints, a, b, c);
    }

    void PH4502C::init() {
        pinMode(this->_ph_level_pin, INPUT);
        pinMode(this->_temp_pin, INPUT);
    }

    void PH4502C::setup() {
        init();
    }

    float PH4502C::readADC() {
        float reading = 0.0f;

        for (int i = 0; i < this->_reading_count; i++) {
            reading += analogRead(this->_ph_level_pin);
            delayMicroseconds(this->_reading_interval);
        }

        reading /= this->_reading_count;

        return reading;
    }

    float PH4502C::getPHLevel() {
        float reading = 0.0f;

        for (int i = 0; i < this->_reading_count; i++) {
            reading += analogRead(this->_ph_level_pin);
            delayMicroseconds(this->_reading_interval);
        }
        reading /= this->_reading_count;

        return calculateX(reading, a, b, c);
    }

    float PH4502C::getPHLevelSingle() {
        float reading = analogRead(this->_ph_level_pin);

        return calculateX(reading, a, b, c);
    }

    int PH4502C::readTemp() {
        return analogRead(this->_temp_pin);
    }

    // MID  ADC=2048
    // PH7  ADC=2080
    // PH10 ADC=1615 Slope=155
    // PH4  ADC=2503 Slope=140

} // namespace GAIT
