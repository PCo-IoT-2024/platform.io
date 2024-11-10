#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>

namespace GAIT {

    class GPS {
    public:
        GPS(uint8_t portNumber, unsigned long baud, enum SerialConfig config, int8_t rx, int8_t tx);

        void setup();
        void goToSleep();

        bool isValid();
        bool isUpdated();

        double getLatitude();
        double getLongitude();
        double getAltitude();
        double getHdop();

    private:
        void sendUBX(const uint8_t* MSG, uint32_t len, long timeout = 3000);
        boolean getUBX_ACK(const uint8_t* MSG, uint32_t len);

        void addChecksum(uint8_t* msg, size_t len);
        void gpsSetPPSDutyCycle();
        bool gpsCheckIfGPSActive();
        bool gpsPowerSaving();
        bool gpsMaxPerformance();

    private:
        HardwareSerial gpsSerial;
        TinyGPSPlus gps;
    };

} // namespace GAIT

#endif // GPS_H
