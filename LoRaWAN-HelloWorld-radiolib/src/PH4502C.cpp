#include "PH4502C.h"

#include <cmath>
#include <vector>

namespace GAIT {

    double PH4502C::a;
    double PH4502C::b;
    double PH4502C::c;

    // Function to solve a 3x3 linear system using Cramer's rule
    //    void quadraticFit(double x1, double y1, double x2, double y2, double x3, double y3) {
    void quadraticFit(const std::vector<DataPoint>& data, double& a, double& b, double& c) {
        // Coefficients for the system of equations
        double A[3][3] = {{data[0].x * data[0].x, data[0].x, 1}, //
                          {data[1].x * data[1].x, data[1].x, 1}, //
                          {data[2].x * data[2].x, data[2].x, 1}};

        double B[3] = {data[0].y, data[1].y, data[2].y};

        // Function to calculate the determinant of a 3x3 matrix
        auto determinant = [](const double matrix[3][3]) -> double {
            return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
                   matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
                   matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
        };

        // Original determinant (D)
        double D = determinant(A);

        if (D == 0) {
            Serial.println("The determinant is zero. The points may be collinear or unsuitable for quadratic fitting.");
            return;
        }

        // Calculate D_a, D_b, D_c by replacing columns of A with B
        double A_a[3][3] = {{B[0], A[0][1], A[0][2]}, {B[1], A[1][1], A[1][2]}, {B[2], A[2][1], A[2][2]}};
        double A_b[3][3] = {{A[0][0], B[0], A[0][2]}, {A[1][0], B[1], A[1][2]}, {A[2][0], B[2], A[2][2]}};
        double A_c[3][3] = {{A[0][0], A[0][1], B[0]}, {A[1][0], A[1][1], B[1]}, {A[2][0], A[2][1], B[2]}};

        double D_a = determinant(A_a);
        double D_b = determinant(A_b);
        double D_c = determinant(A_c);

        // Solve for coefficients a, b, c
        a = D_a / D;
        b = D_b / D;
        c = D_c / D;
    }

    PH4502C::PH4502C(
        uint16_t ph_level_pin, uint16_t temp_pin, const std::vector<DataPoint>& phAdcDataPoints, int reading_interval, int reading_count)
        : _ph_level_pin(ph_level_pin)
        , _temp_pin(temp_pin)
        , _reading_interval(reading_interval)
        , _reading_count(reading_count) {
        //        calculateQuadraticFit(phAdcDataPoints, a, b, c);
        quadraticFit(phAdcDataPoints, a, b, c);
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

        return a * reading * reading + b * reading + c;
    }

    float PH4502C::getPHLevelSingle() {
        float reading = analogRead(this->_ph_level_pin);

        return a * reading * reading + b * reading + c;
    }

    int PH4502C::readTemp() {
        return analogRead(this->_temp_pin);
    }

    // MID  ADC=2048
    // PH7  ADC=2080
    // PH10 ADC=1615 Slope=155
    // PH4  ADC=2503 Slope=140

} // namespace GAIT
