/*

This demonstrates how to save the join information in to permanent memory
so that if the power fails, batteries run out or are changed, the rejoin
is more efficient & happens sooner due to the way that LoRaWAN secures
the join process - see the wiki for more details.

This is typically useful for devices that need more power than a battery
driven sensor - something like a air quality monitor or GPS based device that
is likely to use up it's power source resulting in loss of the session.

The relevant code is flagged with a ##### comment

Saving the entire session is possible but not demonstrated here - it has
implications for flash wearing and complications with which parts of the
session may have changed after an uplink. So it is assumed that the device
is going in to deep-sleep, as below, between normal uplinks.

Once you understand what happens, feel free to delete the comments and
Serial.prints - we promise the final result isn't that many lines.

*/

#if !defined(ESP32)
#pragma error("This is not the example your device is looking for - ESP32 only")
#endif

// ##### load the ESP32 preferences facilites
#include <Preferences.h>

#define RADIOLIB_SPI_PARANOID
#include <RadioLib.h>
#include <TinyGPS++.h>

// LoRaWAN config, credentials & pinmap
#include "config.h"

// utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
// puts these in to the RTC memory which is preserved during deep-sleep
RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin = 0;
RTC_DATA_ATTR uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];
RTC_DATA_ATTR bool isFirstFix = true;

HardwareSerial gpsSerial(2);

//----------------------------------GPS unit functions------------------------------------------------
// Send a byte array of UBX protocol to the GPS
void sendUBX(const uint8_t* MSG, uint32_t len, long timeout = 3000) {
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

    Serial.print("Checksum 1 = ");
    Serial.println(sum1, HEX);

    Serial.print("Checksum 2 = ");
    Serial.println(sum2, HEX);

    Serial.print("fullPacket is: ");

    for (int i = 0; i < (len + 4); i++) {
        Serial.print(fullPacket[i], HEX); // Print out a byt of the UBX data packet to the serial monitor
        Serial.print(", ");
        gpsSerial.write(fullPacket[i]); // Send a byte of the UBX data packet to the GPS unit
    }
    Serial.println();
} // end function

// Calculate expected UBX ACK packet and parse UBX response from GPS--------------------------
boolean getUBX_ACK(const uint8_t* MSG, uint32_t len) {
    uint8_t b;
    uint8_t ackByteID = 0;
    uint8_t ackPacket[10];
    unsigned long startTime = millis();
    uint32_t CK_A = 0, CK_B = 0;
    boolean notAcknowledged = false;

    Serial.print("Reading ACK response: ");
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

    Serial.println("Searching for UBX ACK response:");
    Serial.print("  Target data packet: ");

    for (int i = 0; i < 10; i++) {
        Serial.print(ackPacket[i], HEX);
        Serial.print(", ");
    }

    Serial.println();
    Serial.print("  Candidate   packet: ");

    while (1) {
        // Test for success
        if (ackByteID > 9) {
            // All packets in order!
            Serial.println(" (Response received from GPS unit:)");
            if (notAcknowledged) {
                Serial.println("ACK-NAK!");
            } else {
                Serial.println("ACK-ACK!");
                return true;
            }
        }

        // Timeout if no valid response in 5 seconds
        if (millis() - startTime > 5000) {
            Serial.println("<<<Response timed out!>>>");
            return false;
        }

        // Make sure data is available to read
        if (gpsSerial.available()) {
            b = gpsSerial.read();

            // Check that bytes arrive in sequence as per expected ACK packet
            if (b == ackPacket[ackByteID]) {
                ackByteID++;
                Serial.print(b, HEX);
                Serial.print(", ");
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
                Serial.println(" -->NOPE!");
                Serial.print("Candidate   packet: ");
            }
        }
    } // end while
} // end function

// abbreviated version from the Arduino-ESP32 package, see
// https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/deepsleep.html
// for the complete set of options
void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println(F("Wake from sleep"));
    } else {
        Serial.print(F("Wake not caused by deep sleep: "));
        Serial.println(wakeup_reason);
    }

    Serial.print(F("Boot count: "));
    Serial.println(++bootCount); // increment before printing
}

void addChecksum(uint8_t* msg, size_t len) {
    uint8_t ckA = 0, ckB = 0;
    for (size_t i = 2; i < len - 2; i++) { // Skip the 2-byte header
        ckA += msg[i];
        ckB += ckA;
    }
    msg[len - 2] = ckA;
    msg[len - 1] = ckB;
}

void gpsSetPPSDutyCycle() {
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

bool gpsCheckIfGPSActive() {
    while (gpsSerial.available() > 0) {
        gpsSerial.read();
    }

    // Send a message to check if GPS is active (responsive)
    byte ackRequest[] = {0xB5, 0x62, 0x0A, 0x04, 0x00, 0x00, 0x0E, 0x34}; // Poll navigation status message

    gpsSerial.write(ackRequest, sizeof(ackRequest));

    delay(100); // Small delay for response

    return gpsSerial.available() > 0;
}

bool gpsPowerSaving() {
    byte deepSleepCmd[] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92};

    gpsSerial.write(deepSleepCmd, sizeof(deepSleepCmd));

    delay(100); // Small delay for response

    Serial.println("[GPS] Power save mode: ON");

    return !gpsCheckIfGPSActive();
}

bool gpsMaxPerformance() {
    byte wakeCmd[] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91};
    gpsSerial.write(wakeCmd, sizeof(wakeCmd));

    delay(100); // Small delay for response

    bool gpsIsActive = gpsCheckIfGPSActive();
    if (gpsIsActive) {
        delay(5000); // Wait for GPS to collect data
    }

    Serial.println("[GPS] Max performance mode: ON");

    return gpsIsActive;
}

// put device in to lowest power deep-sleep mode
void gotoSleep(uint32_t seconds) {
    esp_sleep_enable_timer_wakeup(seconds * 1000UL * 1000UL); // function uses uS

    int16_t result = radio.sleep();
    Serial.print("[LoRaWAN] Set sleep: ");
    Serial.println(result == 0 ? "SUCCESS" : "ERROR");

    Serial.println(F("Sleeping\n"));
    Serial.flush();

    esp_deep_sleep_start();

    // if this appears in the serial debug, we didn't go to sleep!
    // so take defensive action so we don't continually uplink
    Serial.println(F("\n\n### Sleep failed, delay of 5 minutes & then restart ###\n"));
    delay(5UL * 60UL * 1000UL);
    ESP.restart();
}

int16_t lwActivate() {
    int16_t state = RADIOLIB_ERR_UNKNOWN;

    // setup the OTAA session information
    node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

    Serial.println(F("Recalling LoRaWAN nonces & session"));

    // ##### setup the flash storage
    Preferences store;

    store.begin("radiolib");

    // ##### if we have previously saved nonces, restore them and try to restore
    // session as well
    if (store.isKey("nonces")) {
        uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE]; // create somewhere to
                                                          // store nonces
        store.getBytes("nonces", buffer,
                       RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // get them from the store
        state = node.setBufferNonces(buffer);             // send them to LoRaWAN
        debug(state != RADIOLIB_ERR_NONE, F("Restoring nonces buffer failed"), state, false);

        // recall session from RTC deep-sleep preserved variable
        state = node.setBufferSession(LWsession); // send them to LoRaWAN stack

        // if we have booted more than once we should have a session to restore, so
        // report any failure otherwise no point saying there's been a failure when
        // it was bound to fail with an empty LWsession var.
        debug((state != RADIOLIB_ERR_NONE) && (bootCount > 1), F("Restoring session buffer failed"), state, false);

        // if Nonces and Session restored successfully, activation is just a
        // formality moreover, Nonces didn't change so no need to re-save them
        if (state == RADIOLIB_ERR_NONE) {
            Serial.println(F("Succesfully restored session - now activating"));
            state = node.activateOTAA();
            debug((state != RADIOLIB_LORAWAN_SESSION_RESTORED), F("Failed to activate restored session"), state, true);

            // ##### close the store before returning
            store.end();
            return (state);
        }

    } else { // store has no key "nonces"
        Serial.println(F("No Nonces saved - starting fresh."));
    }

    // if we got here, there was no session to restore, so start trying to join
    state = RADIOLIB_ERR_NETWORK_NOT_JOINED;
    while (state != RADIOLIB_LORAWAN_NEW_SESSION) { // Original code
        Serial.println(F("Join ('login') to the LoRaWAN Network"));
        state = node.activateOTAA();

        // ##### save the join counters (nonces) to permanent store
        Serial.println(F("Saving nonces to flash"));
        uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE]; // create somewhere to
                                                          // store nonces
        const uint8_t* persist = node.getBufferNonces();  // get pointer to nonces
        memcpy(buffer, persist,
               RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // copy in to buffer
        store.putBytes("nonces", buffer,
                       RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // send them to the store

        // we'll save the session after an uplink

        if (state != RADIOLIB_LORAWAN_NEW_SESSION) {
            Serial.print(F("Join failed: "));
            Serial.println(state);

            // how long to wait before join attempts. This is an interim solution
            // pending implementation of TS001 LoRaWAN Specification section #7 - this
            // doc applies to v1.0.4 & v1.1 it sleeps for longer & longer durations to
            // give time for any gateway issues to resolve or whatever is interfering
            // with the device <-> gateway airwaves.
            uint32_t sleepForSeconds = min((bootCountSinceUnsuccessfulJoin++ + 1UL) * 60UL, 3UL * 60UL);
            Serial.print(F("Boots since unsuccessful join: "));
            Serial.println(bootCountSinceUnsuccessfulJoin);
            Serial.print(F("Retrying join in "));
            Serial.print(sleepForSeconds);
            Serial.println(F(" seconds"));

            gotoSleep(sleepForSeconds);
        }
    } // while join

    Serial.println(F("Joined"));

    // reset the failed join count
    bootCountSinceUnsuccessfulJoin = 0;

    delay(1000); // hold off off hitting the airwaves again too soon - an issue in
                 // the US

    // ##### close the store
    store.end();
    return (state);
}

bool gpsIsValid(const TinyGPSPlus& gps) {
    return gps.location.isValid() && gps.date.isValid() && gps.time.isValid() && gps.satellites.isValid() && gps.altitude.isValid() &&
           gps.speed.isValid() && gps.course.isValid() && gps.hdop.isValid();
}

bool gpsIsUpdated(const TinyGPSPlus& gps) {
    return gps.location.isUpdated() && gps.date.isUpdated() && gps.time.isUpdated() && gps.satellites.isUpdated() &&
           gps.altitude.isUpdated() && gps.speed.isUpdated() && gps.course.isUpdated() && gps.hdop.isUpdated();
}

std::string uplinkPayload = RADIOLIB_LORAWAN_PAYLOAD;
uint8_t fPort = 1; // For application use: 1 ... 223, reserved for further use: 224 ... 255, reserved for mac commands: 0
// Here 223 is used for error/info/message

// setup & execute all device functions ...
void setup() {
    // utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
    // puts these in to the RTC memory which is preserved during deep-sleep
    //    bootCount = 0;
    //    bootCountSinceUnsuccessfulJoin = 0;
    //    for (int i = 0; i < RADIOLIB_LORAWAN_SESSION_BUF_SIZE; i++) {
    //        LWsession[i] = 0;
    //    }

    Serial.begin(9600);
    while (!Serial)
        ;        // wait for serial to be initalised
    delay(2000); // give time to switch to the serial monitor

    Serial.println(F("\nSetup"));

    print_wakeup_reason();

    // setup the radio based on the pinmap (connections) in config.h
    Serial.println(F("Initalise the radio"));

    int16_t state = radio.begin();
    debug(state != RADIOLIB_ERR_NONE, F("Initalise radio failed"), state, true);

    // activate node by restoring session or otherwise joining the network
    state = lwActivate();

    if (state == RADIOLIB_LORAWAN_NEW_SESSION || state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
        // ----- and now for the main event -----
        Serial.println(F("Aquire data"));

        // this is the place to gather the sensor inputs

        // Declare the Hardware Serial to be used by the GPS
        gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
        while (!gpsSerial)
            ; // wait for serial to be initalised

        TinyGPSPlus gps;

        uint8_t disableGLL[] = {0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00};
        uint32_t len = sizeof(disableGLL) / sizeof(uint8_t);

        while (gpsSerial.available()) {
            gpsSerial.read();
        }

        sendUBX(disableGLL, len);
        getUBX_ACK(disableGLL, len);

        unsigned long start = millis();
        while (millis() - start < 2000 && !gpsIsValid(gps)) {
            while (gpsSerial.available() > 0 && !gpsIsValid(gps)) {
                int data = gpsSerial.read();
                gps.encode(data);
                if (std::isprint(data) || std::isspace(data)) {
                    Serial.print((char) data);
                }
            }
        }
        Serial.println();

        if (gpsIsValid(gps)) {
            Serial.println("[GPS] ############### GPS ###############");
            Serial.print("[GPS] LAT = ");
            Serial.println(gps.location.lat(), 6);
            Serial.print("[GPS] LONG = ");
            Serial.println(gps.location.lng(), 6);
            Serial.print("[GPS] Date in UTC = ");
            Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()));
            Serial.print("[GPS] Time in UTC = ");
            Serial.println(String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
            Serial.print("[GPS] Satellites = ");
            Serial.println(gps.satellites.value());
            Serial.print("[GPS] ALT (min) = ");
            Serial.println(gps.altitude.meters());
            Serial.print("[GPS] SPEED (km/h) = ");
            Serial.println(gps.speed.kmph());
            Serial.print("[GPS] COURSE = ");
            Serial.println(gps.course.deg());
            Serial.print("[GPS] HDOP = ");
            Serial.println(gps.hdop.value() / 100.0);
            Serial.println("[GPS] -----------------------------------");

            if (isFirstFix) {
                isFirstFix = false;

                gpsPowerSaving();
            }
        } else {
            Serial.println("GPS positioning data not valid");
        }

        // build uplinkPayload byte array
        Serial.println(F("[LoRaWAN] Constructing uplink"));

        if (gpsIsValid(gps)) {
            fPort = 1; // 1 is location
            uplinkPayload = std::to_string(gps.location.lat()) + "," + std::to_string(gps.location.lng()) + "," +
                            std::to_string(gps.altitude.meters()) + "," + std::to_string(gps.hdop.value() / 100.0);
        } else {
            fPort = 222; // 222 is warning. 223 is error, 222 warning, 221 info message
            uplinkPayload = RADIOLIB_LORAWAN_PAYLOAD;
        }
    } else {
        Serial.println(F("LoRaWAN not activated"));

        // now save session to RTC memory
        const uint8_t* persist = node.getBufferSession();
        memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

        // wait until next uplink - observing legal & TTN FUP constraints
        gotoSleep(RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS);
    }
}

void loop() {
    // create downlinkPayload byte array
    uint8_t downlinkPayload[255]; // Make sure this fits your plans!
    size_t downlinkSize;          // To hold the actual payload size received

    // you can also retrieve additional information about an uplink or
    // downlink by passing a reference to LoRaWANEvent_t structure
    static LoRaWANEvent_t uplinkDetails{};
    static LoRaWANEvent_t downlinkDetails{};

    int16_t state = 0;
    if (downlinkDetails.frmPending) { // At first run this is false due to initialization
        Serial.println(F("[LoRaWAN] Sending request for pending frame:"));
        state = node.sendReceive((uint8_t*) (""), // cppcheck-suppress cstyleCast
                                 0,
                                 fPort,
                                 downlinkPayload,
                                 &downlinkSize,
                                 false,
                                 &uplinkDetails,
                                 &downlinkDetails);
    } else if (node.getFCntUp() == 1) {
        Serial.print(F("[LoRaWAN] Sending: "));
        Serial.println(uplinkPayload.c_str());
        Serial.println(F("[LoRaWAN]   and requesting LinkCheck and DeviceTime"));

        node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
        node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);

        state = node.sendReceive((uint8_t*) uplinkPayload.c_str(), // cppcheck-suppress cstyleCast
                                 uplinkPayload.length(),
                                 fPort,
                                 downlinkPayload,
                                 &downlinkSize,
                                 true,
                                 &uplinkDetails,
                                 &downlinkDetails);
    } else {
        Serial.print("[LoRaWAN] Sending: ");
        Serial.println(uplinkPayload.c_str());
        state = node.sendReceive((uint8_t*) uplinkPayload.c_str(), // cppcheck-suppress cstyleCast
                                 uplinkPayload.length(),
                                 fPort,
                                 downlinkPayload,
                                 &downlinkSize,
                                 false,
                                 &uplinkDetails,
                                 &downlinkDetails);
    }

    //  debug((state == RADIOLIB_LORAWAN_DOWNLINK) && (state !=
    //  RADIOLIB_ERR_NONE), F("Error in sendReceive"), state, false); // wrong
    //  condition
    debug((state < RADIOLIB_ERR_NONE), F("Error in sendReceive"), state, false); // This is correct

    if (state > 0) {
        Serial.println(F("[LoRaWAN] Downlink received"));

        if (downlinkSize > 0) {
            Serial.print(F("[LoRaWAN] Payload:\t"));
            arrayDump(downlinkPayload, downlinkSize);
        } else {
            Serial.println(F("[LoRaWAN] <MAC commands only>"));
        }

        Serial.println(F("[LoRaWan] Signal:"));
        Serial.print(F("[LoRaWAN]     RSSI:               "));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[LoRaWAN]     SNR:                "));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        // print extra information about the event
        Serial.println(F("[LoRaWAN] Event information:"));
        Serial.print(F("[LoRaWAN]     Confirmed:          "));
        Serial.println(downlinkDetails.confirmed);
        Serial.print(F("[LoRaWAN]     Confirming:         "));
        Serial.println(downlinkDetails.confirming);
        Serial.print(F("[LoRaWAN]     FrmPending:         "));
        Serial.println(downlinkDetails.frmPending);
        Serial.print(F("[LoRaWAN]     Datarate:           "));
        Serial.println(downlinkDetails.datarate);
        Serial.print(F("[LoRaWAN]     Frequency:          "));
        Serial.print(downlinkDetails.freq, 3);
        Serial.println(F(" MHz"));
        Serial.print(F("[LoRaWAN]     Frame count:        "));
        Serial.println(downlinkDetails.fCnt);
        Serial.print(F("[LoRaWAN]     Port:               "));
        Serial.println(downlinkDetails.fPort);
        Serial.print(F("[LoRaWAN]     Time-on-air:        "));
        Serial.print(node.getLastToA());
        Serial.println(F(" ms"));
        Serial.print(F("[LoRaWAN]     Rx window:          "));
        Serial.println(state);

        uint8_t margin = 0;
        uint8_t gwCnt = 0;
        if (node.getMacLinkCheckAns(&margin, &gwCnt) == RADIOLIB_ERR_NONE) {
            Serial.println(F("[LoRaWAN] Link check:"));
            Serial.print(F("[LoRaWAN]     LinkCheck margin:   "));
            Serial.println(margin);
            Serial.print(F("[LoRaWAN]     LinkCheck count:    "));
            Serial.println(gwCnt);
        }

        uint32_t networkTime = 0;
        uint8_t fracSecond = 0;
        if (node.getMacDeviceTimeAns(&networkTime, &fracSecond, true) == RADIOLIB_ERR_NONE) {
            Serial.println(F("[LoRaWAN] Timing:"));
            Serial.print(F("[LoRaWAN]     DeviceTime Unix:    "));
            Serial.println(networkTime);
            Serial.print(F("[LoRaWAN]     DeviceTime second:  1/"));
            Serial.println(fracSecond);
        }
    } else {
        Serial.println(F("[LoRaWAN] No downlink received"));
    }

    if (state <= 0 || !downlinkDetails.frmPending) {
        // now save session to RTC memory
        const uint8_t* persist = node.getBufferSession();
        memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

        // wait until next uplink - observing legal & TTN FUP constraints
        gotoSleep(RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS);
    }
}

// Does it respond to a UBX-MON-VER request?
// uint8_t ubx_mon_ver[] = { 0xB5,0x62,0x0A,0x04,0x00,0x00,0x0E,0x34 };
