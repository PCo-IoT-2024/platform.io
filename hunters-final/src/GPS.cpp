#include "GPS.h"

namespace GAIT {

    RTC_DATA_ATTR bool isFirstFix = true;

    GPS::GPS(uint8_t portNumber, unsigned long baud, enum SerialConfig config, int8_t rx, int8_t tx)
        : gpsSerial(portNumber) {
        gpsSerial.begin(baud, config, rx, tx);
    }

    void GPS::setup() {
        while (!gpsSerial)
            ; // wait for serial to be initalised

        while (gpsSerial.available()) {
            gpsSerial.read();
        }

        unsigned long start = millis();
        while (millis() - start < 2000 && !isValid()) {
            while (gpsSerial.available() > 0 && !isValid()) {
                gps.encode(gpsSerial.read());
            }
        }

        if (isValid()) {
            Serial.println(F("[GPS] ############### GPS ###############"));
            Serial.print(F("[GPS] LAT = "));
            Serial.println(gps.location.lat(), 6);
            Serial.print(F("[GPS] LONG = "));
            Serial.println(gps.location.lng(), 6);
            Serial.print(F("[GPS] Date in UTC = "));
            Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()));
            Serial.print(F("[GPS] Time in UTC = "));
            Serial.println(String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
            Serial.print(F("[GPS] Satellites = "));
            Serial.println(gps.satellites.value());
            Serial.print(F("[GPS] ALT (min) = "));
            Serial.println(gps.altitude.meters());
            Serial.print(F("[GPS] SPEED (km/h) = "));
            Serial.println(gps.speed.kmph());
            Serial.print(F("[GPS] COURSE = "));
            Serial.println(gps.course.deg());
            Serial.print(F("[GPS] HDOP = "));
            Serial.println(gps.hdop.value() / 100.0);
            Serial.println(F("[GPS] -----------------------------------"));

            if (isFirstFix) {
                isFirstFix = false;

                gpsPowerSaving();
            }
        } else {
            Serial.println(F("GPS positioning data not valid"));
        }
    }

    void GPS::goToSleep() {
        // Needs to be filled
    }

    double GPS::getLatitude() {
        return gps.location.lat();
    }

    double GPS::getLongitude() {
        return gps.location.lng();
    }

    double GPS::getAltitude() {
        return gps.altitude.meters();
    }

    double GPS::getHdop() {
        return gps.hdop.value() / 100.0;
    }

    //----------------------------------GPS unit functions------------------------------------------------
    // Send a byte array of UBX protocol to the GPS
    void GPS::sendUBX(const uint8_t* MSG, uint32_t len, long timeout) {
        uint32_t CK_A = 0, CK_B = 0;
        uint8_t sum1 = 0x00, sum2 = 0x00;
        uint8_t fullPacket[len + 4];

        for (int i = 0; i < len; i++) {
            fullPacket[i + 2] = MSG[i];
        }

        Serial.println();
        fullPacket[0] = 0xB5;
        fullPacket[1] = 0x62;

        // Calculate checksum
        for (int i = 0; i < len; i++) {
            CK_A = CK_A + MSG[i];
            CK_B = CK_B + CK_A;
        }

        sum1 = CK_A & 0xff; // Mask the checksums to be one byte
        sum2 = CK_B & 0xff;

        fullPacket[len + 2] = sum1; // Add the checksums to the end of the UBX packet
        fullPacket[len + 3] = sum2;

        Serial.print(F("Checksum 1 = "));
        Serial.println(sum1, HEX);

        Serial.print(F("Checksum 2 = "));
        Serial.println(sum2, HEX);

        Serial.print(F("fullPacket is: "));

        for (int i = 0; i < (len + 4); i++) {
            Serial.print(fullPacket[i], HEX); // Print out a byt of the UBX data packet to the serial monitor
            Serial.print(", ");
            gpsSerial.write(fullPacket[i]); // Send a byte of the UBX data packet to the GPS unit
        }
        Serial.println();
    } // end function

    // Calculate expected UBX ACK packet and parse UBX response from GPS--------------------------
    boolean GPS::getUBX_ACK(const uint8_t* MSG, uint32_t len) {
        uint8_t b;
        uint8_t ackByteID = 0;
        uint8_t ackPacket[10];
        unsigned long startTime = millis();
        uint32_t CK_A = 0, CK_B = 0;
        boolean notAcknowledged = false;

        Serial.print(F("Reading ACK response: "));
        // Construct the expected ACK packet
        ackPacket[0] = 0xB5; // header
        ackPacket[1] = 0x62; // header
        ackPacket[2] = 0x05; // class
        ackPacket[3] = 0x01; // id
        ackPacket[4] = 0x02; // length
        ackPacket[5] = 0x00;
        ackPacket[6] = MSG[0]; // MGS class
        ackPacket[7] = MSG[1]; // MSG id
        ackPacket[8] = 0;      // CK_A
        ackPacket[9] = 0;      // CK_B

        // Calculate the checksums
        for (uint8_t i = 2; i < 8; i++) {
            CK_A = CK_A + ackPacket[i];
            CK_B = CK_B + CK_A;
        }

        ackPacket[8] = CK_A & 0xff; // Mask the checksums to be one byte
        ackPacket[9] = CK_B & 0xff;

        Serial.println(F("Searching for UBX ACK response:"));
        Serial.print(F("  Target data packet: "));

        for (int i = 0; i < 10; i++) {
            Serial.print(ackPacket[i], HEX);
            Serial.print(", ");
        }

        Serial.println();
        Serial.print(F("  Candidate   packet: "));

        while (1) {
            // Test for success
            if (ackByteID > 9) {
                // All packets in order!
                Serial.println(F(" (Response received from GPS unit:)"));
                if (notAcknowledged) {
                    Serial.println(F("ACK-NAK!"));
                } else {
                    Serial.println(F("ACK-ACK!"));
                    return true;
                }
            }

            // Timeout if no valid response in 5 seconds
            if (millis() - startTime > 5000) {
                Serial.println(F("<<<Response timed out!>>>"));
                return false;
            }

            // Make sure data is available to read
            if (gpsSerial.available()) {
                b = gpsSerial.read();

                // Check that bytes arrive in sequence as per expected ACK packet
                if (b == ackPacket[ackByteID]) {
                    ackByteID++;
                    Serial.print(b, HEX);
                    Serial.print(F(", "));
                    // Check if message was not acknowledged
                    if (ackByteID == 3) {
                        b = gpsSerial.read();
                        if (b == 0x00) {
                            notAcknowledged = true;
                            ackByteID++;
                        }
                    }
                } else if (ackByteID > 0) {
                    ackByteID = 0; // Reset and look again, invalid order
                    Serial.print(b, HEX);
                    Serial.println(F(" -->NOPE!"));
                    Serial.print(F("Candidate   packet: "));
                }
            }
        } // end while
    } // end function

    void GPS::addChecksum(uint8_t* msg, size_t len) {
        uint8_t ckA = 0, ckB = 0;
        for (size_t i = 2; i < len - 2; i++) { // Skip the 2-byte header
            ckA += msg[i];
            ckB += ckA;
        }
        msg[len - 2] = ckA;
        msg[len - 1] = ckB;
    }

    void GPS::gpsSetPPSDutyCycle() {
        byte ackRequest[] = {
            0xB5, 0x62, 0x06, 0x07, // CFG TP
            0x14, 0x00,             // Payload size (20 Bytes)
            0x40, 0x42, 0x0F, 0x00, // Time interval for time pulse (1 000 0000 micro seconds)
            0x20, 0xA1, 0x07, 0x00, // Length of time pulse (500 000 micro seconds)
            0xFF,                   // Time pulse config setting: +1 = positive, 0 = inactive, -1 = negative
            0x01,                   // Alignment to reference time: 0 = UTC, 1 = GPS, 2, Local Time
            0x00,                   // Bitmask (blink only when synced (0x00) blink always (0x01)
            0x00,                   // Reserved1
            0x32, 0x00,             // Antenna cable delay [ns]
            0x00, 0x00,             // Receiver RF groupe delay [ns]
            0x00, 0x00, 0x00, 0x00, // User defined delay [ns]
            0x00, 0x00              // Check sum

        };

        addChecksum(ackRequest, sizeof(ackRequest));

        gpsSerial.write(ackRequest, sizeof(ackRequest));

        delay(300);
    }

    bool GPS::gpsCheckIfGPSActive() {
        while (gpsSerial.available() > 0) {
            gpsSerial.read();
        }

        // Send a message to check if GPS is active (responsive)
        byte ackRequest[] = {0xB5, 0x62, 0x0A, 0x04, 0x00, 0x00, 0x0E, 0x34}; // Poll navigation status message

        gpsSerial.write(ackRequest, sizeof(ackRequest));

        delay(100); // Small delay for response

        return gpsSerial.available() > 0;
    }

    bool GPS::gpsPowerSaving() {
        /*
        uint8_t disableGLL[] = {0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00};
        uint32_t len = sizeof(disableGLL) / sizeof(uint8_t);

         while (gpsSerial.available()) {
             gpsSerial.read();
         }

          sendUBX(disableGLL, len);
          getUBX_ACK(disableGLL, len);
          */
        byte deepSleepCmd[] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92};

        gpsSerial.write(deepSleepCmd, sizeof(deepSleepCmd));

        delay(100); // Small delay for response

        Serial.println(F("[GPS] Power save mode: ON"));

        return !gpsCheckIfGPSActive();
    }

    bool GPS::gpsMaxPerformance() {
        byte wakeCmd[] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91};
        gpsSerial.write(wakeCmd, sizeof(wakeCmd));

        delay(100); // Small delay for response

        bool gpsIsActive = gpsCheckIfGPSActive();
        if (gpsIsActive) {
            delay(5000); // Wait for GPS to collect data
        }

        Serial.println(F("[GPS] Max performance mode: ON"));

        return gpsIsActive;
    }

    bool GPS::isValid() {
        return gps.location.isValid() && gps.date.isValid() && gps.time.isValid() && gps.satellites.isValid() && gps.altitude.isValid() &&
               gps.speed.isValid() && gps.course.isValid() && gps.hdop.isValid();
    }

    bool GPS::isUpdated() {
        return gps.location.isUpdated() && gps.date.isUpdated() && gps.time.isUpdated() && gps.satellites.isUpdated() &&
               gps.altitude.isUpdated() && gps.speed.isUpdated() && gps.course.isUpdated() && gps.hdop.isUpdated();
    }

} // namespace GAIT
