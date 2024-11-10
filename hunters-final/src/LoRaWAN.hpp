#include "LoRaWAN.h"

// ##### load the ESP32 preferences facilites
#include <Preferences.h>

void gotoSleep(uint32_t seconds);

namespace GAIT {

    static void debug(bool isFail, const __FlashStringHelper* message, int state, bool Freeze);
    static void arrayDump(uint8_t* buffer, uint16_t len);

    uint16_t bootCountSinceUnsuccessfulJoin = 0;
    uint8_t session[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

    template <typename LoRaModule>
    LoRaWAN<LoRaModule>::LoRaWAN(const LoRaWANBand_t& region,
                                 const uint64_t joinEUI,
                                 const uint64_t devEUI,
                                 uint8_t appKey[16],
                                 uint8_t nwkKey[16],
                                 uint8_t pin1,
                                 uint8_t pin2,
                                 uint8_t pin3,
                                 uint8_t pin4,
                                 const uint8_t subBand)
        : radio(new Module(pin1, pin2, pin3, pin4))
        , node(&radio, &region, subBand) {
        node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
    }

    template <typename LoRaModule>
    void LoRaWAN<LoRaModule>::goToSleep() {
        Serial.print(F("[LoRaWAN] Set sleep: "));

        int16_t result = radio.sleep();

        Serial.println(result == 0 ? F("SUCCESS") : F("ERROR"));
    }

    template <typename LoRaModule>
    int16_t LoRaWAN<LoRaModule>::activate(uint16_t bootCount) {
        int16_t state = RADIOLIB_ERR_UNKNOWN;

        Serial.println(F("Recalling LoRaWAN nonces & session"));

        // ##### setup the flash storage
        Preferences store;
        store.begin("radiolib");

        LoRaWANJoinEvent_t joinEvent;

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
            state = node.setBufferSession(session); // send them to LoRaWAN stack

            // if we have booted more than once we should have a session to restore, so
            // report any failure otherwise no point saying there's been a failure when
            // it was bound to fail with an empty LWsession var.
            debug((state != RADIOLIB_ERR_NONE) && (bootCount > 1), F("Restoring session buffer failed"), state, false);

            // if Nonces and Session restored successfully, activation is just a
            // formality moreover, Nonces didn't change so no need to re-save them
            if (state == RADIOLIB_ERR_NONE) {
                Serial.println(F("Succesfully restored session - now activating"));
                state = node.activateOTAA(RADIOLIB_LORAWAN_DATA_RATE_UNUSED, &joinEvent);
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
            state = node.activateOTAA(RADIOLIB_LORAWAN_DATA_RATE_UNUSED, &joinEvent);

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
        Serial.print(F("JoinNonce: "));
        Serial.println(joinEvent.joinNonce);
        Serial.print(F("DevNonce: "));
        Serial.println(joinEvent.devNonce);
        Serial.print(F("NewSession: "));
        Serial.println(joinEvent.newSession);

        // reset the failed join count
        bootCountSinceUnsuccessfulJoin = 0;

        delay(1000); // hold off off hitting the airwaves again too soon - an issue in
                     // the US

        // ##### close the store
        store.end();

        return (state);
    }

    template <typename LoRaModule>
    void LoRaWAN<LoRaModule>::setDownlinkCB(std::function<void(uint8_t, uint8_t*, std::size_t)> downlinkCB) {
        this->downlinkCB = downlinkCB;
    }

    template <typename LoRaModule>
    void LoRaWAN<LoRaModule>::setup(uint16_t bootCount) {
        Serial.println(F("Initalise the radio"));

        int16_t state = radio.begin();
        debug(state != RADIOLIB_ERR_NONE, F("Initalise radio failed"), state, true);

        if (state == RADIOLIB_ERR_NONE) {
            // activate node by restoring session or otherwise joining the network
            state = activate(bootCount);

            if (state != RADIOLIB_LORAWAN_NEW_SESSION && state != RADIOLIB_LORAWAN_SESSION_RESTORED) {
                Serial.println(F("LoRaWAN not activated"));

                // now save session to RTC memory
                const uint8_t* persist = node.getBufferSession();
                memcpy(session, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

                // wait until next uplink - observing legal & TTN FUP constraints
                gotoSleep(RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS);
            }
        }
    }

    template <typename LoRaModule>
    void LoRaWAN<LoRaModule>::setUplinkPayload(uint8_t fPort, const std::string& uplinkPayload) {
        this->fPort = fPort;
        this->uplinkPayload = uplinkPayload;
    }

    template <typename LoRaModule>
    void LoRaWAN<LoRaModule>::loop() {
        // create downlinkPayload byte array
        uint8_t downlinkPayload[255]; // Make sure this fits your plans!
        size_t downlinkSize;          // To hold the actual payload size received

        // you can also retrieve additional information about an uplink or
        // downlink by passing a reference to LoRaWANEvent_t structure
        static LoRaWANEvent_t uplinkDetails{};
        static LoRaWANEvent_t downlinkDetails{};

        int16_t state = 0;
        if (downlinkDetails.frmPending || downlinkDetails.confirmed) { // At first run this is false due to initialization
            Serial.println(F("[LoRaWAN] Sending request for pending frame"));
            state = node.sendReceive((uint8_t*) "", // cppcheck-suppress cstyleCast
                                     0,
                                     220,
                                     downlinkPayload,
                                     &downlinkSize,
                                     false,
                                     &uplinkDetails,
                                     &downlinkDetails);
        } else {
            Serial.print(F("[LoRaWAN] Sending: "));
            Serial.print(F("fPort = "));
            Serial.print(fPort);
            Serial.print(", ");
            Serial.println(uplinkPayload.c_str());

            if (node.getFCntUp() == 1) {
                Serial.println(F("[LoRaWAN]   and requesting LinkCheck and DeviceTime"));

                node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
                node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);
            }

            state = node.sendReceive((uint8_t*) uplinkPayload.c_str(), // cppcheck-suppress cstyleCast
                                     uplinkPayload.length(),
                                     fPort,
                                     downlinkPayload,
                                     &downlinkSize,
                                     false,
                                     &uplinkDetails,
                                     &downlinkDetails);
        }

        debug((state < RADIOLIB_ERR_NONE), F("Error in sendReceive"), state, false); // This is correct

        if (state > 0) {
            Serial.println(F("[LoRaWAN] Downlink received"));

            if (downlinkSize > 0) {
                Serial.print(F("[LoRaWAN] Payload:\t"));
                arrayDump(downlinkPayload, downlinkSize);
                if (downlinkCB) {
                    downlinkCB(downlinkDetails.fPort, downlinkPayload, downlinkSize);
                }
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

        if (state <= 0 || !(downlinkDetails.frmPending || downlinkDetails.confirmed)) {
            // now save session to RTC memory
            const uint8_t* persist = node.getBufferSession();
            memcpy(session, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

            // wait until next uplink - observing legal & TTN FUP constraints
            gotoSleep(RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS);
        }
    }

    // Helper function to display any issues
    static void debug(bool isFail, const __FlashStringHelper* message, int state, bool Freeze) {
        if (isFail) {
            Serial.print(message);
            Serial.print("(");
            Serial.print(state);
            Serial.println(")");
            while (Freeze)
                ;
        }
    }

    // Helper function to display a byte array
    static void arrayDump(uint8_t* buffer, uint16_t len) {
        for (uint16_t c = 0; c < len; c++) {
            Serial.printf("0x%02X ", buffer[c]);
        }
        Serial.print("-> ");

        char str[len + 1];
        str[len] = '\0';

        snprintf(str, len + 1, "%s", buffer);
        Serial.println(str);
    }

} // namespace GAIT
