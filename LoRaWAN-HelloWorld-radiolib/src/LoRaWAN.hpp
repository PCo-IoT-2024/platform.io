#ifndef LORAWAN_HPP
#define LORAWAN_HPP

#include <Arduino.h>
#include <Preferences.h>
#include <RadioLib.h>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <esp_attr.h>
#include <functional>
#include <string>
#include <utility>

// Application-level deep sleep function implemented in the main file.
void goToSleep(uint32_t seconds);

namespace radio {

    // -------------------------------------------------------------------------
    // Configuration
    // -------------------------------------------------------------------------

    static constexpr uint32_t RTC_SESSION_MAGIC = 0x4C57534Eu; // "LWSN"
    static constexpr uint16_t RTC_SESSION_VERSION = 4;

    static constexpr const char* NVS_NAMESPACE = "radiolib";
    static constexpr const char* NVS_KEY_IDENTITY = "identity";
    static constexpr const char* NVS_KEY_NONCES = "nonces";
    static constexpr const char* NVS_KEY_SESSION = "session";

    static constexpr uint32_t JOIN_RETRY_MIN_SECONDS = 60;
    static constexpr uint32_t JOIN_RETRY_MAX_SECONDS = 3 * 60;

    static constexpr uint8_t MAX_PENDING_DRAIN_UPLINKS = 3;

    static constexpr uint8_t FPORT_PENDING_DRAIN = 220;
    static constexpr uint8_t FPORT_INFO = 221;

    // -------------------------------------------------------------------------
    // RTC-persistent data
    // -------------------------------------------------------------------------

    struct RtcSessionStore {
        uint32_t magic = 0;
        uint16_t version = 0;
        uint16_t size = 0;
        uint64_t identityHash = 0;
        uint32_t crc32 = 0;
        uint32_t fCntUp = 0;
        uint8_t buffer[RADIOLIB_LORAWAN_SESSION_BUF_SIZE] = {};
    };

    RTC_DATA_ATTR inline uint16_t bootCountSinceUnsuccessfulJoin = 0;
    RTC_DATA_ATTR inline uint8_t pendingDrainCount = 0;
    RTC_DATA_ATTR inline RtcSessionStore rtcSession = {};

    // -------------------------------------------------------------------------
    // Utilities
    // -------------------------------------------------------------------------

    inline uint32_t crc32Update(uint32_t crc, uint8_t data) {
        crc ^= data;

        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 1u) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc >>= 1;
            }
        }

        return crc;
    }

    inline uint32_t crc32(const uint8_t* data, size_t len) {
        uint32_t crc = 0xFFFFFFFFu;

        for (size_t i = 0; i < len; ++i) {
            crc = crc32Update(crc, data[i]);
        }

        return ~crc;
    }

    inline uint64_t fnv1a64Update(uint64_t hash, uint8_t byte) {
        hash ^= byte;
        hash *= 1099511628211ULL;
        return hash;
    }

    inline uint64_t fnv1a64UpdateBuffer(uint64_t hash, const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            hash = fnv1a64Update(hash, data[i]);
        }

        return hash;
    }

    inline uint64_t fnv1a64UpdateU64(uint64_t hash, uint64_t value) {
        for (uint8_t i = 0; i < 8; ++i) {
            hash = fnv1a64Update(hash, static_cast<uint8_t>((value >> (8 * i)) & 0xFFu));
        }

        return hash;
    }

    inline uint64_t fnv1a64UpdateU32(uint64_t hash, uint32_t value) {
        for (uint8_t i = 0; i < 4; ++i) {
            hash = fnv1a64Update(hash, static_cast<uint8_t>((value >> (8 * i)) & 0xFFu));
        }

        return hash;
    }

    inline void logState(const __FlashStringHelper* message, int16_t state) {
        Serial.print(message);
        Serial.print(F(" ("));
        Serial.print(state);
        Serial.println(F(")"));
    }

    inline void logJoinFailureHint(int16_t state) {
        if (state == RADIOLIB_ERR_NO_JOIN_ACCEPT) {
            Serial.println(F("[LoRaWAN] No JoinAccept received."));
            Serial.println(F("[LoRaWAN] Check TTN Live Data:"));
            Serial.println(F("[LoRaWAN]   - no JoinRequest visible: RF, region, pins, antenna, gateway coverage"));
            Serial.println(F("[LoRaWAN]   - JoinRequest visible but rejected: EUI/key/DevNonce/frequency-plan issue"));
            Serial.println(F("[LoRaWAN]   - JoinRequest accepted but node misses JoinAccept: downlink/range/gateway duty-cycle issue"));
        }
    }

    inline void arrayDump(const uint8_t* buffer, size_t len) {
        if (buffer == nullptr || len == 0) {
            Serial.println(F("<empty>"));
            return;
        }

        for (size_t i = 0; i < len; ++i) {
            Serial.printf("0x%02X ", buffer[i]);
        }

        Serial.print(F("-> \""));

        for (size_t i = 0; i < len; ++i) {
            const uint8_t c = buffer[i];
            Serial.print(std::isprint(static_cast<unsigned char>(c)) ? static_cast<char>(c) : '.');
        }

        Serial.println(F("\""));
    }

    inline bool nvsPutBytesIfChanged(Preferences& store, const char* key, const uint8_t* data, size_t len) {
        if (data == nullptr || len == 0) {
            return false;
        }

        const size_t oldLen = store.getBytesLength(key);

        if (oldLen == len) {
            uint8_t* oldData = static_cast<uint8_t*>(std::malloc(len));

            if (oldData != nullptr) {
                const size_t readLen = store.getBytes(key, oldData, len);
                const bool unchanged = readLen == len && std::memcmp(oldData, data, len) == 0;

                std::free(oldData);

                if (unchanged) {
                    return true;
                }
            }
        }

        const size_t written = store.putBytes(key, data, len);
        return written == len;
    }

    inline void invalidateRtcSession() {
        rtcSession.magic = 0;
        rtcSession.version = 0;
        rtcSession.size = 0;
        rtcSession.identityHash = 0;
        rtcSession.crc32 = 0;
        rtcSession.fCntUp = 0;
        std::memset(rtcSession.buffer, 0, sizeof(rtcSession.buffer));

        pendingDrainCount = 0;
    }

    inline void resetJoinBackoff() {
        bootCountSinceUnsuccessfulJoin = 0;
    }

    // -------------------------------------------------------------------------
    // LoRaWAN wrapper
    // -------------------------------------------------------------------------

    template <typename LoRaModule>
    class LoRaWAN {
    public:
        LoRaWAN(const LoRaWANBand_t& region,
                uint64_t joinEUI,
                uint64_t devEUI,
                const uint8_t appKey[16],
                const uint8_t nwkKey[16],
                uint8_t pin1,
                uint8_t pin2,
                uint8_t pin3,
                uint8_t pin4,
                uint8_t subBand = 0)
            : radio(new Module(pin1, pin2, pin3, pin4))
            , node(&radio, &region, subBand)
            , joinEUI(joinEUI)
            , devEUI(devEUI)
            , subBand(subBand) {
            std::memcpy(this->appKey, appKey, sizeof(this->appKey));

            if (nwkKey != nullptr) {
                std::memcpy(this->nwkKey, nwkKey, sizeof(this->nwkKey));
                hasNwkKey = true;
            } else {
                std::memset(this->nwkKey, 0, sizeof(this->nwkKey));
                hasNwkKey = false;
            }

            identityHash = calculateIdentityHash();

            node.beginOTAA(joinEUI, devEUI, hasNwkKey ? this->nwkKey : nullptr, this->appKey);
        }

        void sleepRadio() {
            Serial.print(F("[LoRaWAN] Set radio sleep: "));
            const int16_t result = radio.sleep();
            Serial.println(result == RADIOLIB_ERR_NONE ? F("SUCCESS") : F("ERROR"));
        }

        void goToSleep() {
            sleepRadio();
        }

        void setup(uint16_t bootCount) {
            Serial.println(F("[LoRaWAN] Initialize radio"));

            const int16_t state = radio.begin();

            if (state != RADIOLIB_ERR_NONE) {
                logState(F("[LoRaWAN] Initialize radio failed"), state);
                ::goToSleep(JOIN_RETRY_MAX_SECONDS);
                return;
            }

            const int16_t activationState = activate(bootCount);

            if (activationState == RADIOLIB_LORAWAN_NEW_SESSION || activationState == RADIOLIB_LORAWAN_SESSION_RESTORED) {
                Serial.println(F("[LoRaWAN] Activated"));
                saveSessionToRtc();
                saveSessionToNvs();
                return;
            }

            logState(F("[LoRaWAN] Activation failed"), activationState);
            ::goToSleep(JOIN_RETRY_MAX_SECONDS);
        }

        void setUplinkPayload(uint8_t port, const std::string& payload) {
            fPort = port;
            uplinkPayload = payload;
        }

        void setDownlinkCB(std::function<void(uint8_t, const uint8_t*, std::size_t)> cb) {
            downlinkCB = std::move(cb);
        }

        void loop() {
            uint8_t downlinkPayload[255] = {};
            size_t downlinkSize = 0;

            LoRaWANEvent_t uplinkDetails{};
            LoRaWANEvent_t downlinkDetails{};

            const uint32_t fCntBefore = node.getFCntUp();

            int16_t state = RADIOLIB_ERR_UNKNOWN;

            if (pendingDrainCount > 0) {
                Serial.println(F("[LoRaWAN] Requesting pending downlink frame"));

                state = node.sendReceive(reinterpret_cast<const uint8_t*>(""),
                                         0,
                                         FPORT_PENDING_DRAIN,
                                         downlinkPayload,
                                         &downlinkSize,
                                         false,
                                         &uplinkDetails,
                                         &downlinkDetails);
            } else {
                sendMacRequestsIfUseful();

                Serial.print(F("[LoRaWAN] Sending: fPort = "));
                Serial.print(fPort);
                Serial.print(F(", "));
                Serial.println(uplinkPayload.c_str());

                state = node.sendReceive(reinterpret_cast<const uint8_t*>(uplinkPayload.data()),
                                         uplinkPayload.size(),
                                         fPort,
                                         downlinkPayload,
                                         &downlinkSize,
                                         false,
                                         &uplinkDetails,
                                         &downlinkDetails);
            }

            const uint32_t fCntAfter = node.getFCntUp();

            if (state < RADIOLIB_ERR_NONE) {
                logState(F("[LoRaWAN] Error in sendReceive"), state);
            }

            if (state > 0) {
                handleDownlink(state, downlinkPayload, downlinkSize, downlinkDetails);
            } else {
                Serial.println(F("[LoRaWAN] No downlink received"));
            }

            if (state >= RADIOLIB_ERR_NONE) {
                Serial.print(F("[LoRaWAN] FCntUp before/after: "));
                Serial.print(fCntBefore);
                Serial.print(F(" -> "));
                Serial.println(fCntAfter);

                saveSessionToRtc();

                // Important:
                // Save session to NVS after every successful uplink.
                // Otherwise a hard reset may restore an old FCntUp and TTN may drop
                // the first uplink after reboot as duplicate/too old.
                saveSessionToNvs();
            } else {
                Serial.println(F("[LoRaWAN] Uplink failed, session not saved"));
            }

            if (downlinkDetails.frmPending && pendingDrainCount < MAX_PENDING_DRAIN_UPLINKS) {
                ++pendingDrainCount;

                Serial.print(F("[LoRaWAN] More downlink data pending, drain attempt "));
                Serial.print(pendingDrainCount);
                Serial.print(F("/"));
                Serial.println(MAX_PENDING_DRAIN_UPLINKS);

                return;
            }

            pendingDrainCount = 0;
            ::goToSleep(RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS);
        }

    private:
        uint64_t calculateIdentityHash() const {
            uint64_t hash = 14695981039346656037ULL;

            hash = fnv1a64UpdateU64(hash, joinEUI);
            hash = fnv1a64UpdateU64(hash, devEUI);
            hash = fnv1a64Update(hash, subBand);
            hash = fnv1a64Update(hash, hasNwkKey ? 1 : 0);

            hash = fnv1a64UpdateBuffer(hash, appKey, sizeof(appKey));
            hash = fnv1a64UpdateBuffer(hash, nwkKey, sizeof(nwkKey));

            hash = fnv1a64UpdateU32(hash, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
            hash = fnv1a64UpdateU32(hash, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);

            return hash;
        }

        bool hasValidStoredNonces(Preferences& store) {
            return store.getBytesLength(NVS_KEY_NONCES) == RADIOLIB_LORAWAN_NONCES_BUF_SIZE;
        }

        bool ensureStoredIdentity(Preferences& store) {
            uint64_t storedIdentityHash = 0;
            const size_t len = store.getBytesLength(NVS_KEY_IDENTITY);

            if (len == sizeof(storedIdentityHash)) {
                const size_t readLen = store.getBytes(NVS_KEY_IDENTITY, &storedIdentityHash, sizeof(storedIdentityHash));

                if (readLen == sizeof(storedIdentityHash) && storedIdentityHash == identityHash) {
                    Serial.println(F("[LoRaWAN] Stored identity matches current firmware"));
                    return true;
                }

                Serial.println(F("[LoRaWAN] Stored identity differs from current firmware"));
            } else {
                Serial.println(F("[LoRaWAN] No stored identity found"));
            }

            Serial.println(F("[LoRaWAN] Clearing old LoRaWAN session, preserving nonces"));

            store.remove(NVS_KEY_SESSION);
            store.remove(NVS_KEY_IDENTITY);

            invalidateRtcSession();
            resetJoinBackoff();

            if (hasValidStoredNonces(store)) {
                Serial.println(F("[LoRaWAN] Existing nonces found and preserved"));
            } else {
                Serial.println(F("[LoRaWAN] No valid nonces to preserve"));
            }

            const size_t written = store.putBytes(NVS_KEY_IDENTITY, &identityHash, sizeof(identityHash));

            if (written == sizeof(identityHash)) {
                Serial.println(F("[LoRaWAN] Stored new identity"));
            } else {
                Serial.println(F("[LoRaWAN] Failed to store new identity"));
            }

            return false;
        }

        int16_t activate(uint16_t bootCount) {
            Serial.println(F("[LoRaWAN] Recalling LoRaWAN nonces and session"));

            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                Serial.println(F("[LoRaWAN] NVS open failed; continuing without durable persistence"));
                return joinNetworkWithoutNvs();
            }

            const bool identityMatches = ensureStoredIdentity(store);

            // Always restore nonces if present.
            // This avoids DevNonce reuse after firmware updates or power loss.
            restoreNoncesFromNvs(store);

            if (identityMatches) {
                int16_t state = restoreSessionFromRtc();

                if (state != RADIOLIB_ERR_NONE) {
                    state = restoreSessionFromNvs(store);
                }

                if (state == RADIOLIB_ERR_NONE) {
                    LoRaWANJoinEvent_t joinEvent{};

                    Serial.println(F("[LoRaWAN] Restored session, activating"));

                    state = node.activateOTAA(&joinEvent);

                    if (state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
                        Serial.println(F("[LoRaWAN] Session restored"));
                        store.end();
                        return state;
                    }

                    logState(F("[LoRaWAN] Failed to activate restored session"), state);

                    // The stored session exists but cannot be used.
                    // Keep nonces, discard session, force OTAA.
                    store.remove(NVS_KEY_SESSION);
                    invalidateRtcSession();
                } else if (bootCount > 1) {
                    logState(F("[LoRaWAN] No restorable session found"), state);
                }
            } else {
                Serial.println(F("[LoRaWAN] Identity changed or first run; forcing fresh OTAA join"));
            }

            const int16_t state = joinNetworkWithNvs(store);

            store.end();

            return state;
        }

        int16_t joinNetworkWithoutNvs() {
            LoRaWANJoinEvent_t joinEvent{};
            int16_t state = RADIOLIB_ERR_NETWORK_NOT_JOINED;

            while (state != RADIOLIB_LORAWAN_NEW_SESSION) {
                Serial.println(F("[LoRaWAN] Join network"));
                state = node.activateOTAA(&joinEvent);

                if (state != RADIOLIB_LORAWAN_NEW_SESSION) {
                    handleJoinFailure(state);
                }
            }

            handleJoinSuccess(joinEvent);
            return state;
        }

        int16_t joinNetworkWithNvs(Preferences& store) {
            LoRaWANJoinEvent_t joinEvent{};
            int16_t state = RADIOLIB_ERR_NETWORK_NOT_JOINED;

            while (state != RADIOLIB_LORAWAN_NEW_SESSION) {
                Serial.println(F("[LoRaWAN] Join network"));
                state = node.activateOTAA(&joinEvent);

                // Save nonces after every join attempt, successful or not.
                // This is important because DevNonce may have advanced even if
                // the node did not receive a JoinAccept.
                saveNoncesToNvs(store);

                if (state != RADIOLIB_LORAWAN_NEW_SESSION) {
                    handleJoinFailure(state);
                }
            }

            handleJoinSuccess(joinEvent);

            saveSessionToRtc();
            saveSessionToNvs(store);

            return state;
        }

        void handleJoinFailure(int16_t state) {
            Serial.print(F("[LoRaWAN] Join failed: "));
            Serial.println(state);

            logJoinFailureHint(state);

            const uint32_t requestedSleep = (static_cast<uint32_t>(bootCountSinceUnsuccessfulJoin) + 1UL) * JOIN_RETRY_MIN_SECONDS;

            const uint32_t sleepForSeconds = requestedSleep < JOIN_RETRY_MAX_SECONDS ? requestedSleep : JOIN_RETRY_MAX_SECONDS;

            ++bootCountSinceUnsuccessfulJoin;

            Serial.print(F("[LoRaWAN] Boots since unsuccessful join: "));
            Serial.println(bootCountSinceUnsuccessfulJoin);

            Serial.print(F("[LoRaWAN] Retrying join in "));
            Serial.print(sleepForSeconds);
            Serial.println(F(" seconds"));

            ::goToSleep(sleepForSeconds);
        }

        void handleJoinSuccess(const LoRaWANJoinEvent_t& joinEvent) {
            Serial.println(F("[LoRaWAN] Joined"));
            Serial.print(F("[LoRaWAN] JoinNonce: "));
            Serial.println(joinEvent.joinNonce);
            Serial.print(F("[LoRaWAN] DevNonce: "));
            Serial.println(joinEvent.devNonce);
            Serial.print(F("[LoRaWAN] NewSession: "));
            Serial.println(joinEvent.newSession);

            bootCountSinceUnsuccessfulJoin = 0;
            pendingDrainCount = 0;
        }

        void restoreNoncesFromNvs(Preferences& store) {
            const size_t len = store.getBytesLength(NVS_KEY_NONCES);

            if (len != RADIOLIB_LORAWAN_NONCES_BUF_SIZE) {
                Serial.println(F("[LoRaWAN] No valid nonces saved; starting with current RadioLib nonces"));
                return;
            }

            uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE] = {};
            const size_t readLen = store.getBytes(NVS_KEY_NONCES, buffer, sizeof(buffer));

            if (readLen != sizeof(buffer)) {
                Serial.println(F("[LoRaWAN] Failed to read nonces from NVS"));
                return;
            }

            const int16_t state = node.setBufferNonces(buffer);

            if (state == RADIOLIB_ERR_NONE) {
                Serial.println(F("[LoRaWAN] Restored nonces from NVS"));
            } else {
                logState(F("[LoRaWAN] Restoring nonces failed"), state);
            }
        }

        void saveNoncesToNvs(Preferences& store) {
            const uint8_t* persist = node.getBufferNonces();

            if (persist == nullptr) {
                Serial.println(F("[LoRaWAN] Nonces buffer unavailable"));
                return;
            }

            if (nvsPutBytesIfChanged(store, NVS_KEY_NONCES, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE)) {
                Serial.println(F("[LoRaWAN] Nonces saved to NVS"));
            } else {
                Serial.println(F("[LoRaWAN] Saving nonces to NVS failed"));
            }
        }

        int16_t restoreSessionFromRtc() {
            if (rtcSession.magic != RTC_SESSION_MAGIC || rtcSession.version != RTC_SESSION_VERSION ||
                rtcSession.size != RADIOLIB_LORAWAN_SESSION_BUF_SIZE || rtcSession.identityHash != identityHash) {
                return RADIOLIB_ERR_UNKNOWN;
            }

            const uint32_t calculated = crc32(rtcSession.buffer, rtcSession.size);

            if (calculated != rtcSession.crc32) {
                Serial.println(F("[LoRaWAN] RTC session CRC mismatch"));
                return RADIOLIB_ERR_UNKNOWN;
            }

            const int16_t state = node.setBufferSession(rtcSession.buffer);

            if (state == RADIOLIB_ERR_NONE) {
                Serial.println(F("[LoRaWAN] Restored session from RTC"));
            } else {
                logState(F("[LoRaWAN] Restoring RTC session failed"), state);
            }

            return state;
        }

        int16_t restoreSessionFromNvs(Preferences& store) {
            const size_t len = store.getBytesLength(NVS_KEY_SESSION);

            if (len != RADIOLIB_LORAWAN_SESSION_BUF_SIZE) {
                return RADIOLIB_ERR_UNKNOWN;
            }

            uint8_t buffer[RADIOLIB_LORAWAN_SESSION_BUF_SIZE] = {};
            const size_t readLen = store.getBytes(NVS_KEY_SESSION, buffer, sizeof(buffer));

            if (readLen != sizeof(buffer)) {
                Serial.println(F("[LoRaWAN] Failed to read session from NVS"));
                return RADIOLIB_ERR_UNKNOWN;
            }

            const int16_t state = node.setBufferSession(buffer);

            if (state == RADIOLIB_ERR_NONE) {
                Serial.println(F("[LoRaWAN] Restored session from NVS"));
            } else {
                logState(F("[LoRaWAN] Restoring NVS session failed"), state);
            }

            return state;
        }

        void saveSessionToRtc() {
            const uint8_t* persist = node.getBufferSession();

            if (persist == nullptr) {
                Serial.println(F("[LoRaWAN] Session buffer unavailable"));
                return;
            }

            std::memcpy(rtcSession.buffer, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

            rtcSession.magic = RTC_SESSION_MAGIC;
            rtcSession.version = RTC_SESSION_VERSION;
            rtcSession.size = RADIOLIB_LORAWAN_SESSION_BUF_SIZE;
            rtcSession.identityHash = identityHash;
            rtcSession.crc32 = crc32(rtcSession.buffer, rtcSession.size);
            rtcSession.fCntUp = node.getFCntUp();

            Serial.println(F("[LoRaWAN] Session saved to RTC"));
        }

        void saveSessionToNvs() {
            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                Serial.println(F("[LoRaWAN] NVS open failed; session not saved"));
                return;
            }

            ensureStoredIdentity(store);
            saveSessionToNvs(store);

            store.end();
        }

        void saveSessionToNvs(Preferences& store) {
            const uint8_t* persist = node.getBufferSession();

            if (persist == nullptr) {
                Serial.println(F("[LoRaWAN] Session buffer unavailable"));
                return;
            }

            if (nvsPutBytesIfChanged(store, NVS_KEY_SESSION, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE)) {
                Serial.println(F("[LoRaWAN] Session saved to NVS"));
            } else {
                Serial.println(F("[LoRaWAN] Saving session to NVS failed"));
            }
        }

        void sendMacRequestsIfUseful() {
            if (node.getFCntUp() != 1) {
                return;
            }

            Serial.println(F("[LoRaWAN] Requesting LinkCheck and DeviceTime"));

            node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
            node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);
        }

        void handleDownlink(int16_t rxWindow, const uint8_t* downlinkPayload, size_t downlinkSize, LoRaWANEvent_t& downlinkDetails) {
            Serial.println(F("[LoRaWAN] Downlink received"));

            if (downlinkSize > 0) {
                Serial.print(F("[LoRaWAN] Payload: "));
                arrayDump(downlinkPayload, downlinkSize);

                if (downlinkCB) {
                    downlinkCB(downlinkDetails.fPort, downlinkPayload, downlinkSize);
                }
            } else {
                Serial.println(F("[LoRaWAN] <MAC commands only>"));
            }

            Serial.println(F("[LoRaWAN] Signal:"));

            Serial.print(F("[LoRaWAN]     RSSI:               "));
            Serial.print(radio.getRSSI());
            Serial.println(F(" dBm"));

            Serial.print(F("[LoRaWAN]     SNR:                "));
            Serial.print(radio.getSNR());
            Serial.println(F(" dB"));

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
            Serial.println(rxWindow);

            printMacAnswers();
        }

        void printMacAnswers() {
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
            uint16_t milliseconds = 0;

            if (node.getMacDeviceTimeAns(&networkTime, &milliseconds, true) == RADIOLIB_ERR_NONE) {
                Serial.println(F("[LoRaWAN] Timing:"));

                Serial.print(F("[LoRaWAN]     DeviceTime:         "));
                Serial.print(networkTime);
                Serial.print('.');

                if (milliseconds < 100) {
                    Serial.print('0');
                }

                if (milliseconds < 10) {
                    Serial.print('0');
                }

                Serial.println(milliseconds);
            }
        }

        std::function<void(uint8_t, const uint8_t*, std::size_t)> downlinkCB;

        LoRaModule radio;
        LoRaWANNode node;

        uint64_t joinEUI = 0;
        uint64_t devEUI = 0;
        uint8_t appKey[16] = {};
        uint8_t nwkKey[16] = {};
        bool hasNwkKey = false;
        uint8_t subBand = 0;
        uint64_t identityHash = 0;

        uint8_t fPort = FPORT_INFO;
        std::string uplinkPayload;
    };

} // namespace radio

#endif // LORAWAN_HPP
