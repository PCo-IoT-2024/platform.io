#ifndef PH4502C_H
#define PH4502C_H

#include <Arduino.h>

namespace GAIT {

/// Default calibration value for the PH4502C sensor.
#ifndef PH4502C_DEFAULT_CALIBRATION
#define PH4502C_DEFAULT_CALIBRATION 7.f // 14.8f
#endif

/// Default reading interval (in milliseconds) between pH readings.
#ifndef PH4502C_DEFAULT_READING_INTERVAL
#define PH4502C_DEFAULT_READING_INTERVAL 100
#endif

/// Default number of pH readings to average.
#ifndef PH4502C_DEFAULT_READING_COUNT
#define PH4502C_DEFAULT_READING_COUNT 10
#endif

    /// Default ADC resolution for the PH4502C sensor.
#ifndef PH4502C_DEFAULT_ADC_RESOLUTION
#define PH4502C_DEFAULT_ADC_RESOLUTION 4096.0f
#endif

    /// Operating voltage for the PH4502C sensor.
#ifndef PH4502C_VOLTAGE
#define PH4502C_VOLTAGE 3.3f
#endif

    /// Voltage that represents a neutral pH reading (pH = 7).
#ifndef PH4502C_MID_VOLTAGE
#define PH4502C_MID_VOLTAGE 1.575f
#endif

    /// Rate of change in voltage per unit change in pH.
#ifndef PH4502C_PH_VOLTAGE_PER_PH
#define PH4502C_PH_VOLTAGE_PER_PH 0.12f
#endif

    /**
     *
     * @class PH4502C_Sensor
     * @brief Class for interfacing with the PH4502C pH sensor.
     *      * This class provides the necessary functions to communicate with the PH4502C pH sensor.
     * You can connect the pH level sensor and the temperature sensor to your Arduino using
     * the constructor's parameters.
     *
     */
    class PH4502C {
    public:
        /**
         *
         * @brief Construct a new PH4502C_Sensor object.
         *
         * @param ph_level_pin Analog pin connected to the pH level output of the sensor.
         * @param temp_pin Analog pin connected to the temperature output of the sensor.
         * @param calibration Calibration value to adjust pH readings (default is PH4502C_DEFAULT_CALIBRATION).
         * @param reading_interval Time interval between consecutive pH level readings in milliseconds (default is
         * PH4502C_DEFAULT_READING_INTERVAL).
         * @param reading_count Number of pH level readings to average when using read_ph_level() (default is
         * PH4502C_DEFAULT_READING_COUNT).
         * @param adc_resolution ADC resolution used for voltage calculation from the analog read (default is
         * PH4502C_DEFAULT_ADC_RESOLUTION).
         *
         */
        PH4502C(uint16_t ph_level_pin,
                uint16_t temp_pin,
                float calibration = PH4502C_DEFAULT_CALIBRATION,
                int reading_interval = PH4502C_DEFAULT_READING_INTERVAL,
                int reading_count = PH4502C_DEFAULT_READING_COUNT,
                float adc_resolution = PH4502C_DEFAULT_ADC_RESOLUTION);

        /**
         *
         * @brief Initialize the PH4502C sensor.
         *          * This function sets up the necessary configurations for the sensor. It should be
         * called in your setup() function.
         *
         */
        void init();

        /**
         *
         * @brief Recalibrate the sensor with a new calibration value.
         *          * @param calibration The new pH calibration value. This function allows you to
         * fine-tune the pH readings if your sensor's accuracy changes over time.
         *
         */
        void setup(float calibration);

        /**
         *
         * @brief Read and calculate the pH level.
         *          * @return The calculated pH level. This function reads the analog signal from the
         * pH sensor, converts it into a pH value, and returns the result.
         *
         */
        float getPHLevel();

        /**
         *
         * @brief Read and calculate the pH level without averaging multiple readings.
         *          * @return The calculated pH level. This function is used when you require a single
         * pH reading without the benefit of averaging multiple readings.
         *
         */
        float getPHLevelSingle();

        /**
         *
         * @brief Read the temperature from the sensor.
         *          * @return The temperature reading in degrees Celsius. This function reads the analog
         * signal from the temperature sensor and returns the temperature in degrees Celsius.
         *
         */
        int read_temp();

        float readVoltage();

    private:
        uint16_t _ph_level_pin; ///< The analog pin connected to the pH level sensor.
        uint16_t _temp_pin;     ///< The analog pin connected to the temperature sensor.
        int _reading_interval;  ///< The interval between pH readings in milliseconds.
        int _reading_count;     ///< The number of readings to average.
        float _calibration;     ///< The pH calibration value.
        float _adc_resolution;  ///< The ADC resolution for analog-to-digital conversion.

        RTC_DATA_ATTR static double a;
        RTC_DATA_ATTR static double b;
        RTC_DATA_ATTR static double c;
    };

} // namespace GAIT

#endif // PH4502C_H
