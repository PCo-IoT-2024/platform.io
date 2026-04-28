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
    // User-tunable production settings
    // -------------------------------------------------------------------------

#ifndef RADIOLIB_LORAWAN_PAYLOAD_SIZE
    // Conservative default for EU868 at low data rates.
    // Override in platformio.ini if you know your effective DR/payload budget.
#define RADIOLIB_LORAWAN_PAYLOAD_SIZE 51
#endif

#ifndef RADIOLIB_JOIN_RETRY_MIN_SECONDS
#define RADIOLIB_JOIN_RETRY_MIN_SECONDS 60UL
#endif

#ifndef RADIOLIB_JOIN_RETRY_MAX_SECONDS
#define RADIOLIB_JOIN_RETRY_MAX_SECONDS (60UL * 60UL)
#endif

#ifndef RADIOLIB_PENDING_DRAIN_LIMIT
#define RADIOLIB_PENDING_DRAIN_LIMIT 3
#endif

#ifndef RADIOLIB_DEBUG_LOG
#define RADIOLIB_DEBUG_LOG 1
#endif

#if RADIOLIB_DEBUG_LOG
#define RL_LOG(x) Serial.print(x)
#define RL_LOGLN(x) Serial.println(x)
#define RL_PRINTF(...) Serial.printf(__VA_ARGS__)
#define RL_LOG_FLOAT(x, digits) Serial.print((x), (digits))
#else
#define RL_LOG(x)                                                                                                                          \
    do {                                                                                                                                   \
    } while (false)

#define RL_LOGLN(x)                                                                                                                        \
    do {                                                                                                                                   \
    } while (false)

#define RL_PRINTF(...)                                                                                                                     \
    do {                                                                                                                                   \
    } while (false)

#define RL_LOG_FLOAT(x, digits)                                                                                                            \
    do {                                                                                                                                   \
    } while (false)
#endif

    // -------------------------------------------------------------------------
    // Persistence configuration
    // -------------------------------------------------------------------------

    static constexpr uint32_t RTC_SESSION_MAGIC = 0x4C57534Eu; // "LWSN"
    static constexpr uint16_t RTC_SESSION_VERSION = 5;
    static constexpr uint16_t NVS_SCHEMA_VERSION = 5;

    static constexpr const char* NVS_NAMESPACE = "radiolib";
    static constexpr const char* NVS_KEY_SCHEMA = "schema";
    static constexpr const char* NVS_KEY_IDENTITY = "identity";
    static constexpr const char* NVS_KEY_NONCES = "nonces";
    static constexpr const char* NVS_KEY_SESSION = "session";

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
    // Utility functions
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
        RL_LOG(message);
        RL_LOG(F(" ("));
        RL_LOG(state);
        RL_LOGLN(F(")"));
    }

    inline void logJoinFailureHint(int16_t state) {
        if (state == RADIOLIB_ERR_NO_JOIN_ACCEPT) {
            RL_LOGLN(F("[LoRaWAN] No JoinAccept received."));
            RL_LOGLN(F("[LoRaWAN] Check TTN Live Data:"));
            RL_LOGLN(F("[LoRaWAN]   - no JoinRequest visible: RF, region, pins, antenna, gateway coverage"));
            RL_LOGLN(F("[LoRaWAN]   - JoinRequest visible but rejected: EUI/key/DevNonce/frequency-plan issue"));
            RL_LOGLN(F("[LoRaWAN]   - JoinRequest accepted but node misses JoinAccept: downlink/range/gateway duty-cycle issue"));
        }
    }

    inline void arrayDump(const uint8_t* buffer, size_t len) {
        if (buffer == nullptr || len == 0) {
            RL_LOGLN(F("<empty>"));
            return;
        }

        for (size_t i = 0; i < len; ++i) {
            RL_PRINTF("0x%02X ", buffer[i]);
        }

        RL_LOG(F("-> \""));

        for (size_t i = 0; i < len; ++i) {
            const uint8_t c = buffer[i];
            RL_LOG(std::isprint(static_cast<unsigned char>(c)) ? static_cast<char>(c) : '.');
        }

        RL_LOGLN(F("\""));
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

    inline uint32_t calculateJoinBackoffSeconds(uint16_t failures) {
        uint32_t delaySeconds = RADIOLIB_JOIN_RETRY_MIN_SECONDS;

        for (uint16_t i = 0; i < failures; ++i) {
            if (delaySeconds >= RADIOLIB_JOIN_RETRY_MAX_SECONDS / 2UL) {
                delaySeconds = RADIOLIB_JOIN_RETRY_MAX_SECONDS;
                break;
            }

            delaySeconds *= 2UL;
        }

        if (delaySeconds > RADIOLIB_JOIN_RETRY_MAX_SECONDS) {
            delaySeconds = RADIOLIB_JOIN_RETRY_MAX_SECONDS;
        }

        return delaySeconds;
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
            RL_LOG(F("[LoRaWAN] Set radio sleep: "));
            const int16_t result = radio.sleep();
            RL_LOGLN(result == RADIOLIB_ERR_NONE ? F("SUCCESS") : F("ERROR"));
        }

        void goToSleep() {
            sleepRadio();
        }

        void setup(uint16_t bootCount) {
            RL_LOGLN(F("[LoRaWAN] Initialize radio"));

            const int16_t state = radio.begin();

            if (state != RADIOLIB_ERR_NONE) {
                logState(F("[LoRaWAN] Initialize radio failed"), state);
                ::goToSleep(RADIOLIB_JOIN_RETRY_MAX_SECONDS);
                return;
            }

            const int16_t activationState = activate(bootCount);

            if (activationState == RADIOLIB_LORAWAN_NEW_SESSION) {
                RL_LOGLN(F("[LoRaWAN] Activated with new session"));
                saveSessionToRtc();
                saveSessionToNvs();
                return;
            }

            if (activationState == RADIOLIB_LORAWAN_SESSION_RESTORED) {
                RL_LOGLN(F("[LoRaWAN] Activated with restored session"));
                return;
            }

            logState(F("[LoRaWAN] Activation failed"), activationState);
            ::goToSleep(RADIOLIB_JOIN_RETRY_MAX_SECONDS);
        }

        void setUplinkPayload(uint8_t port, const std::string& payload) {
            fPort = port;
            uplinkPayload = payload;
        }

        void setDownlinkCB(std::function<void(uint8_t, const uint8_t*, std::size_t)> cb) {
            downlinkCB = std::move(cb);
        }

        void clearSessionPersistence() {
            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                RL_LOGLN(F("[LoRaWAN] NVS open failed; cannot clear session persistence"));
                return;
            }

            store.remove(NVS_KEY_SESSION);
            store.remove(NVS_KEY_IDENTITY);
            store.remove(NVS_KEY_SCHEMA);

            store.putUShort(NVS_KEY_SCHEMA, NVS_SCHEMA_VERSION);
            store.putBytes(NVS_KEY_IDENTITY, &identityHash, sizeof(identityHash));

            store.end();

            invalidateRtcSession();
            resetJoinBackoff();

            RL_LOGLN(F("[LoRaWAN] Session persistence cleared; nonces preserved"));
        }

        void clearAllPersistenceDangerous() {
            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                RL_LOGLN(F("[LoRaWAN] NVS open failed; cannot clear persistence"));
                return;
            }

            store.remove(NVS_KEY_SESSION);
            store.remove(NVS_KEY_NONCES);
            store.remove(NVS_KEY_IDENTITY);
            store.remove(NVS_KEY_SCHEMA);

            store.putUShort(NVS_KEY_SCHEMA, NVS_SCHEMA_VERSION);
            store.putBytes(NVS_KEY_IDENTITY, &identityHash, sizeof(identityHash));

            store.end();

            invalidateRtcSession();
            resetJoinBackoff();

            RL_LOGLN(F("[LoRaWAN] DANGER: session and nonces cleared"));
            RL_LOGLN(F("[LoRaWAN] Only do this if Join Server DevNonces were reset or DevEUI changed"));
        }

        void loop() {
            uint8_t downlinkPayload[255] = {};
            size_t downlinkSize = 0;

            LoRaWANEvent_t uplinkDetails{};
            LoRaWANEvent_t downlinkDetails{};

            const uint32_t fCntBefore = node.getFCntUp();

            int16_t state = RADIOLIB_ERR_UNKNOWN;

            if (pendingDrainCount > 0) {
                RL_LOGLN(F("[LoRaWAN] Requesting pending downlink frame"));

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
                enforcePayloadLimit();

                RL_LOG(F("[LoRaWAN] Sending: fPort = "));
                RL_LOG(fPort);
                RL_LOG(F(", "));
                RL_LOGLN(uplinkPayload.c_str());

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
                RL_LOGLN(F("[LoRaWAN] No downlink received"));
            }

            if (state >= RADIOLIB_ERR_NONE) {
                RL_LOG(F("[LoRaWAN] FCntUp before/after: "));
                RL_LOG(fCntBefore);
                RL_LOG(F(" -> "));
                RL_LOGLN(fCntAfter);

                saveSessionToRtc();

                // Production rule:
                // Save after every successful uplink. This avoids restoring an old
                // FCntUp after power loss, which would make the next uplink look
                // duplicate/too old to TTN.
                saveSessionToNvs();
            } else {
                RL_LOGLN(F("[LoRaWAN] Uplink failed, session not saved"));
            }

            if (downlinkDetails.frmPending && pendingDrainCount < RADIOLIB_PENDING_DRAIN_LIMIT) {
                ++pendingDrainCount;

                RL_LOG(F("[LoRaWAN] More downlink data pending, drain attempt "));
                RL_LOG(pendingDrainCount);
                RL_LOG(F("/"));
                RL_LOGLN(RADIOLIB_PENDING_DRAIN_LIMIT);

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
            hash = fnv1a64UpdateU32(hash, NVS_SCHEMA_VERSION);

            return hash;
        }

        bool hasValidStoredNonces(Preferences& store) {
            return store.getBytesLength(NVS_KEY_NONCES) == RADIOLIB_LORAWAN_NONCES_BUF_SIZE;
        }

        bool ensureNvsSchema(Preferences& store) {
            const uint16_t storedSchema = store.getUShort(NVS_KEY_SCHEMA, 0);

            if (storedSchema == NVS_SCHEMA_VERSION) {
                return true;
            }

            RL_LOG(F("[LoRaWAN] Persistence schema mismatch: "));
            RL_LOG(storedSchema);
            RL_LOG(F(" -> "));
            RL_LOGLN(NVS_SCHEMA_VERSION);

            // Preserve nonces. For LoRaWAN 1.0.4 / 1.1.x, clearing nonces for a
            // reused DevEUI can cause DevNonce reuse or dev_nonce_too_small.
            store.remove(NVS_KEY_SESSION);
            store.remove(NVS_KEY_IDENTITY);
            store.remove(NVS_KEY_SCHEMA);

            store.putUShort(NVS_KEY_SCHEMA, NVS_SCHEMA_VERSION);

            invalidateRtcSession();

            return false;
        }

        bool ensureStoredIdentity(Preferences& store) {
            const bool schemaMatches = ensureNvsSchema(store);

            uint64_t storedIdentityHash = 0;
            const size_t len = store.getBytesLength(NVS_KEY_IDENTITY);

            if (schemaMatches && len == sizeof(storedIdentityHash)) {
                const size_t readLen = store.getBytes(NVS_KEY_IDENTITY, &storedIdentityHash, sizeof(storedIdentityHash));

                if (readLen == sizeof(storedIdentityHash) && storedIdentityHash == identityHash) {
                    RL_LOGLN(F("[LoRaWAN] Stored identity matches current firmware"));
                    return true;
                }

                RL_LOGLN(F("[LoRaWAN] Stored identity differs from current firmware"));
            } else {
                RL_LOGLN(F("[LoRaWAN] No stored identity found"));
            }

            RL_LOGLN(F("[LoRaWAN] Clearing old LoRaWAN session, preserving nonces"));

            store.remove(NVS_KEY_SESSION);
            store.remove(NVS_KEY_IDENTITY);

            invalidateRtcSession();
            resetJoinBackoff();

            if (hasValidStoredNonces(store)) {
                RL_LOGLN(F("[LoRaWAN] Existing nonces found and preserved"));
            } else {
                RL_LOGLN(F("[LoRaWAN] No valid nonces to preserve"));
                RL_LOGLN(F("[LoRaWAN] If this DevEUI was used before, TTN may reject DevNonce=0"));
            }

            store.putUShort(NVS_KEY_SCHEMA, NVS_SCHEMA_VERSION);

            const size_t written = store.putBytes(NVS_KEY_IDENTITY, &identityHash, sizeof(identityHash));

            if (written == sizeof(identityHash)) {
                RL_LOGLN(F("[LoRaWAN] Stored new identity"));
            } else {
                RL_LOGLN(F("[LoRaWAN] Failed to store new identity"));
            }

            return false;
        }

        int16_t activate(uint16_t bootCount) {
            RL_LOGLN(F("[LoRaWAN] Recalling LoRaWAN nonces and session"));

            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                RL_LOGLN(F("[LoRaWAN] NVS open failed; continuing without durable persistence"));
                return joinNetworkWithoutNvs();
            }

            const bool identityMatches = ensureStoredIdentity(store);

            // Always restore nonces if present.
            //
            // Production rule:
            // For LoRaWAN 1.0.4 / 1.1.x, DevNonce is monotonic. Never clear
            // NVS nonces for a reused DevEUI unless the Join Server DevNonce
            // state is also reset, or the device is reprovisioned with a new DevEUI.
            restoreNoncesFromNvs(store);

            if (identityMatches) {
                int16_t state = restoreSessionFromRtc();

                if (state != RADIOLIB_ERR_NONE) {
                    state = restoreSessionFromNvs(store);
                }

                if (state == RADIOLIB_ERR_NONE) {
                    LoRaWANJoinEvent_t joinEvent{};

                    RL_LOGLN(F("[LoRaWAN] Restored session, activating"));

                    state = node.activateOTAA(&joinEvent);

                    if (state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
                        RL_LOGLN(F("[LoRaWAN] Session restored"));
                        store.end();
                        return state;
                    }

                    logState(F("[LoRaWAN] Failed to activate restored session"), state);

                    store.remove(NVS_KEY_SESSION);
                    invalidateRtcSession();
                } else if (bootCount > 1) {
                    logState(F("[LoRaWAN] No restorable session found"), state);
                }
            } else {
                RL_LOGLN(F("[LoRaWAN] Identity changed or first run; forcing fresh OTAA join"));
            }

            const int16_t state = joinNetworkWithNvs(store);

            store.end();

            return state;
        }

        int16_t joinNetworkWithoutNvs() {
            LoRaWANJoinEvent_t joinEvent{};
            int16_t state = RADIOLIB_ERR_NETWORK_NOT_JOINED;

            while (state != RADIOLIB_LORAWAN_NEW_SESSION) {
                RL_LOGLN(F("[LoRaWAN] Join network"));
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
                RL_LOGLN(F("[LoRaWAN] Join network"));
                state = node.activateOTAA(&joinEvent);

                // Production rule:
                // Save nonces after every join attempt, successful or not.
                // DevNonce may have advanced even if no JoinAccept was received.
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
            RL_LOG(F("[LoRaWAN] Join failed: "));
            RL_LOGLN(state);

            logJoinFailureHint(state);

            const uint32_t sleepForSeconds = calculateJoinBackoffSeconds(bootCountSinceUnsuccessfulJoin);

            if (bootCountSinceUnsuccessfulJoin < UINT16_MAX) {
                ++bootCountSinceUnsuccessfulJoin;
            }

            RL_LOG(F("[LoRaWAN] Boots since unsuccessful join: "));
            RL_LOGLN(bootCountSinceUnsuccessfulJoin);

            RL_LOG(F("[LoRaWAN] Retrying join in "));
            RL_LOG(sleepForSeconds);
            RL_LOGLN(F(" seconds"));

            ::goToSleep(sleepForSeconds);
        }

        void handleJoinSuccess(const LoRaWANJoinEvent_t& joinEvent) {
            RL_LOGLN(F("[LoRaWAN] Joined"));

            RL_LOG(F("[LoRaWAN] JoinNonce: "));
            RL_LOGLN(joinEvent.joinNonce);

            RL_LOG(F("[LoRaWAN] DevNonce: "));
            RL_LOGLN(joinEvent.devNonce);

            RL_LOG(F("[LoRaWAN] NewSession: "));
            RL_LOGLN(joinEvent.newSession);

            bootCountSinceUnsuccessfulJoin = 0;
            pendingDrainCount = 0;
        }

        void restoreNoncesFromNvs(Preferences& store) {
            const size_t len = store.getBytesLength(NVS_KEY_NONCES);

            if (len != RADIOLIB_LORAWAN_NONCES_BUF_SIZE) {
                RL_LOGLN(F("[LoRaWAN] No valid nonces saved; starting with current RadioLib nonces"));
                return;
            }

            uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE] = {};
            const size_t readLen = store.getBytes(NVS_KEY_NONCES, buffer, sizeof(buffer));

            if (readLen != sizeof(buffer)) {
                RL_LOGLN(F("[LoRaWAN] Failed to read nonces from NVS"));
                return;
            }

            const int16_t state = node.setBufferNonces(buffer);

            if (state == RADIOLIB_ERR_NONE) {
                RL_LOGLN(F("[LoRaWAN] Restored nonces from NVS"));
            } else {
                logState(F("[LoRaWAN] Restoring nonces failed"), state);
            }
        }

        void saveNoncesToNvs(Preferences& store) {
            const uint8_t* persist = node.getBufferNonces();

            if (persist == nullptr) {
                RL_LOGLN(F("[LoRaWAN] Nonces buffer unavailable"));
                return;
            }

            if (nvsPutBytesIfChanged(store, NVS_KEY_NONCES, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE)) {
                RL_LOGLN(F("[LoRaWAN] Nonces saved to NVS"));
            } else {
                RL_LOGLN(F("[LoRaWAN] Saving nonces to NVS failed"));
            }
        }

        int16_t restoreSessionFromRtc() {
            if (rtcSession.magic != RTC_SESSION_MAGIC || rtcSession.version != RTC_SESSION_VERSION ||
                rtcSession.size != RADIOLIB_LORAWAN_SESSION_BUF_SIZE || rtcSession.identityHash != identityHash) {
                return RADIOLIB_ERR_UNKNOWN;
            }

            const uint32_t calculated = crc32(rtcSession.buffer, rtcSession.size);

            if (calculated != rtcSession.crc32) {
                RL_LOGLN(F("[LoRaWAN] RTC session CRC mismatch"));
                return RADIOLIB_ERR_UNKNOWN;
            }

            const int16_t state = node.setBufferSession(rtcSession.buffer);

            if (state == RADIOLIB_ERR_NONE) {
                RL_LOGLN(F("[LoRaWAN] Restored session from RTC"));
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
                RL_LOGLN(F("[LoRaWAN] Failed to read session from NVS"));
                return RADIOLIB_ERR_UNKNOWN;
            }

            const int16_t state = node.setBufferSession(buffer);

            if (state == RADIOLIB_ERR_NONE) {
                RL_LOGLN(F("[LoRaWAN] Restored session from NVS"));
            } else {
                logState(F("[LoRaWAN] Restoring NVS session failed"), state);
            }

            return state;
        }

        void saveSessionToRtc() {
            const uint8_t* persist = node.getBufferSession();

            if (persist == nullptr) {
                RL_LOGLN(F("[LoRaWAN] Session buffer unavailable"));
                return;
            }

            std::memcpy(rtcSession.buffer, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

            rtcSession.magic = RTC_SESSION_MAGIC;
            rtcSession.version = RTC_SESSION_VERSION;
            rtcSession.size = RADIOLIB_LORAWAN_SESSION_BUF_SIZE;
            rtcSession.identityHash = identityHash;
            rtcSession.crc32 = crc32(rtcSession.buffer, rtcSession.size);
            rtcSession.fCntUp = node.getFCntUp();

            RL_LOGLN(F("[LoRaWAN] Session saved to RTC"));
        }

        void saveSessionToNvs() {
            Preferences store;

            if (!store.begin(NVS_NAMESPACE, false)) {
                RL_LOGLN(F("[LoRaWAN] NVS open failed; session not saved"));
                return;
            }

            ensureStoredIdentity(store);
            saveSessionToNvs(store);

            store.end();
        }

        void saveSessionToNvs(Preferences& store) {
            const uint8_t* persist = node.getBufferSession();

            if (persist == nullptr) {
                RL_LOGLN(F("[LoRaWAN] Session buffer unavailable"));
                return;
            }

            if (nvsPutBytesIfChanged(store, NVS_KEY_SESSION, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE)) {
                RL_LOGLN(F("[LoRaWAN] Session saved to NVS"));
            } else {
                RL_LOGLN(F("[LoRaWAN] Saving session to NVS failed"));
            }
        }

        void sendMacRequestsIfUseful() {
            if (node.getFCntUp() != 1) {
                return;
            }

            RL_LOGLN(F("[LoRaWAN] Requesting LinkCheck and DeviceTime"));

            node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
            node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);
        }

        void enforcePayloadLimit() {
            if (uplinkPayload.size() <= RADIOLIB_LORAWAN_PAYLOAD_SIZE) {
                return;
            }

            RL_LOG(F("[LoRaWAN] Payload too large, truncating from "));
            RL_LOG(uplinkPayload.size());
            RL_LOG(F(" to "));
            RL_LOGLN(RADIOLIB_LORAWAN_PAYLOAD_SIZE);

            uplinkPayload.resize(RADIOLIB_LORAWAN_PAYLOAD_SIZE);
        }

        void handleDownlink(int16_t rxWindow, const uint8_t* downlinkPayload, size_t downlinkSize, LoRaWANEvent_t& downlinkDetails) {
            RL_LOGLN(F("[LoRaWAN] Downlink received"));

            if (downlinkSize > 0) {
                RL_LOG(F("[LoRaWAN] Payload: "));
                arrayDump(downlinkPayload, downlinkSize);

                if (downlinkCB) {
                    downlinkCB(downlinkDetails.fPort, downlinkPayload, downlinkSize);
                }
            } else {
                RL_LOGLN(F("[LoRaWAN] <MAC commands only>"));
            }

            RL_LOGLN(F("[LoRaWAN] Signal:"));

            RL_LOG(F("[LoRaWAN]     RSSI:               "));
            RL_LOG(radio.getRSSI());
            RL_LOGLN(F(" dBm"));

            RL_LOG(F("[LoRaWAN]     SNR:                "));
            RL_LOG(radio.getSNR());
            RL_LOGLN(F(" dB"));

            RL_LOGLN(F("[LoRaWAN] Event information:"));

            RL_LOG(F("[LoRaWAN]     Confirmed:          "));
            RL_LOGLN(downlinkDetails.confirmed);

            RL_LOG(F("[LoRaWAN]     Confirming:         "));
            RL_LOGLN(downlinkDetails.confirming);

            RL_LOG(F("[LoRaWAN]     FrmPending:         "));
            RL_LOGLN(downlinkDetails.frmPending);

            RL_LOG(F("[LoRaWAN]     Datarate:           "));
            RL_LOGLN(downlinkDetails.datarate);

            RL_LOG(F("[LoRaWAN]     Frequency:          "));
            RL_LOG_FLOAT(downlinkDetails.freq, 3);
            RL_LOGLN(F(" MHz"));

            RL_LOG(F("[LoRaWAN]     Frame count:        "));
            RL_LOGLN(downlinkDetails.fCnt);

            RL_LOG(F("[LoRaWAN]     Port:               "));
            RL_LOGLN(downlinkDetails.fPort);

            RL_LOG(F("[LoRaWAN]     Time-on-air:        "));
            RL_LOG(node.getLastToA());
            RL_LOGLN(F(" ms"));

            RL_LOG(F("[LoRaWAN]     Rx window:          "));
            RL_LOGLN(rxWindow);

            printMacAnswers();
        }

        void printMacAnswers() {
            uint8_t margin = 0;
            uint8_t gwCnt = 0;

            if (node.getMacLinkCheckAns(&margin, &gwCnt) == RADIOLIB_ERR_NONE) {
                RL_LOGLN(F("[LoRaWAN] Link check:"));

                RL_LOG(F("[LoRaWAN]     LinkCheck margin:   "));
                RL_LOGLN(margin);

                RL_LOG(F("[LoRaWAN]     LinkCheck count:    "));
                RL_LOGLN(gwCnt);
            }

            uint32_t networkTime = 0;
            uint16_t milliseconds = 0;

            if (node.getMacDeviceTimeAns(&networkTime, &milliseconds, true) == RADIOLIB_ERR_NONE) {
                RL_LOGLN(F("[LoRaWAN] Timing:"));

                RL_LOG(F("[LoRaWAN]     DeviceTime:         "));
                RL_LOG(networkTime);
                RL_LOG('.');

                if (milliseconds < 100) {
                    RL_LOG('0');
                }

                if (milliseconds < 10) {
                    RL_LOG('0');
                }

                RL_LOGLN(milliseconds);
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

#undef RL_LOG
#undef RL_LOGLN
#undef RL_PRINTF
#undef RL_LOG_FLOAT

#endif // LORAWAN_HPP
