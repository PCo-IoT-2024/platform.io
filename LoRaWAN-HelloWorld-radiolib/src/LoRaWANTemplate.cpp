#if !defined(ESP32)
#error "This example is ESP32 only"
#endif

#include "DS18B20.h"
#include "GPS.h"
#include "LoRaWAN.hpp"

#include <Arduino.h>
#include <Preferences.h>
#include <string>

RTC_DATA_ATTR uint16_t bootCount = 0;

#ifndef APP_DEBUG_SERIAL
#define APP_DEBUG_SERIAL 1
#endif

#ifndef APP_FACTORY_RESET_PIN
#define APP_FACTORY_RESET_PIN -1
#endif

#ifndef APP_DANGEROUS_NONCE_RESET_PIN
#define APP_DANGEROUS_NONCE_RESET_PIN -1
#endif

#ifndef APP_LOW_BATTERY_SLEEP_SECONDS
#define APP_LOW_BATTERY_SLEEP_SECONDS (30UL * 60UL)
#endif

static const uint8_t appKey[16] = {RADIOLIB_LORAWAN_APP_KEY};

#ifdef RADIOLIB_LORAWAN_NWK_KEY
static const uint8_t nwkKey[16] = {RADIOLIB_LORAWAN_NWK_KEY};
#else
static const uint8_t* nwkKey = nullptr;
#endif

static radio::LoRaWAN<RADIOLIB_LORA_MODULE>
    loRaWAN(RADIOLIB_LORA_REGION, RADIOLIB_LORAWAN_JOIN_EUI, RADIOLIB_LORAWAN_DEV_EUI, appKey, nwkKey, RADIOLIB_LORA_MODULE_BITMAP);

static position::GPS gps(GPS_SERIAL_PORT, GPS_SERIAL_BAUD_RATE, GPS_SERIAL_CONFIG, GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN);

static temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
static temperature::DS18B20 temp;

static void printWakeupReason() {
#if APP_DEBUG_SERIAL
    const esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();

    if (wakeupReason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println(F("[APP] Wake from deep sleep"));
    } else {
        Serial.print(F("[APP] Wake not caused by deep sleep: "));
        Serial.println(static_cast<int>(wakeupReason));
    }
#endif

    ++bootCount;

#if APP_DEBUG_SERIAL
    Serial.print(F("[APP] Boot count: "));
    Serial.println(bootCount);
#endif
}

static bool isFactoryResetRequested() {
#if APP_FACTORY_RESET_PIN >= 0
    pinMode(APP_FACTORY_RESET_PIN, INPUT_PULLUP);
    delay(20);
    return digitalRead(APP_FACTORY_RESET_PIN) == LOW;
#else
    return false;
#endif
}

static bool isDangerousNonceResetRequested() {
#if APP_DANGEROUS_NONCE_RESET_PIN >= 0
    pinMode(APP_DANGEROUS_NONCE_RESET_PIN, INPUT_PULLUP);
    delay(20);
    return digitalRead(APP_DANGEROUS_NONCE_RESET_PIN) == LOW;
#else
    return false;
#endif
}

static bool isBatteryTooLow() {
    // Production hook:
    // Add ADC-based battery measurement here.
    //
    // Recommended policy:
    // - if voltage is too low, do not join/send
    // - sleep longer
    // - avoid radio TX during brownout-prone conditions
    return false;
}

void goToSleep(uint32_t seconds) {
    loRaWAN.sleepRadio();
    gps.goToSleep();

#if APP_DEBUG_SERIAL
    Serial.print(F("[APP] Go to sleep for "));
    Serial.print(seconds);
    Serial.println(F(" seconds"));
    Serial.println();
    Serial.flush();
#endif

    esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(seconds) * 1000ULL * 1000ULL);
    esp_deep_sleep_start();

#if APP_DEBUG_SERIAL
    Serial.println(F("\n\n[APP] Sleep failed, restarting in 5 minutes\n"));
    Serial.flush();
#endif

    delay(5UL * 60UL * 1000UL);
    ESP.restart();
}

static std::string buildGpsPayload() {
    std::string payload;
    payload.reserve(80);

    payload += std::to_string(gps.getLatitude());
    payload += ',';
    payload += std::to_string(gps.getLongitude());
    payload += ',';
    payload += std::to_string(gps.getAltitude());
    payload += ',';
    payload += std::to_string(gps.getHdop());

    return payload;
}

static std::string buildTemperaturePayload() {
    std::string payload;
    payload.reserve(80);

    payload += std::to_string(temp.getTemperature());

    return payload;
}

static void acquireDataAndPrepareUplink() {
#if APP_DEBUG_SERIAL
    Serial.println(F("[APP] Acquire data and construct LoRaWAN uplink"));
#endif

    constexpr uint8_t SENSOR_COUNT = 2;

    const uint8_t currentSensor = static_cast<uint8_t>((bootCount - 1) % SENSOR_COUNT);

    Serial.print(F("[APP] Current sensor: "));
    Serial.println(currentSensor);

    uint8_t fPort = 221;
    std::string uplinkPayload;

    fPort = currentSensor + 1;

    switch (currentSensor) {
        case 0:
            gps.setup();

            if (gps.isValid()) {
                fPort = 1;
                uplinkPayload = buildGpsPayload();
            } else {
#if APP_DEBUG_SERIAL
                Serial.println(F("[APP] GPS positioning data not valid"));
#endif
                fPort = 221;
                uplinkPayload = "RadioLib experiment device: Waiting for GPS";
            }
            break;
        case 1:
            temp.setup();

            if (temp.isValid()) {
                uplinkPayload = buildTemperaturePayload();
            } else {
#if APP_DEBUG_SERIAL
                Serial.println(F("[APP] Temperature sensor data not valid"));
#endif

                fPort = 221;
                uplinkPayload = "RadioLib experiment device: Temperature sensor error";
            }
            break;
        default:
            fPort = 221;
            uplinkPayload = "RadioLib experiment device: No sensor selected";
            break;
    }

    loRaWAN.setUplinkPayload(fPort, uplinkPayload);
}

void setup() {
#if APP_DEBUG_SERIAL
    Serial.begin(115200);

    const uint32_t serialWaitStart = millis();

    while (!Serial && millis() - serialWaitStart < 3000UL) {
        delay(10);
    }

    delay(500);
#else
    Serial.begin(115200);
#endif

    printWakeupReason();

#if APP_DEBUG_SERIAL
    Serial.println(F("[APP] Setup"));
#endif

    if (isDangerousNonceResetRequested()) {
#if APP_DEBUG_SERIAL
        Serial.println(F("[APP] DANGEROUS nonce reset requested"));
        Serial.println(F("[APP] Only safe if TTN used DevNonces were reset or DevEUI changed"));
#endif
        loRaWAN.clearAllPersistenceDangerous();
    } else if (isFactoryResetRequested()) {
#if APP_DEBUG_SERIAL
        Serial.println(F("[APP] Factory/session reset requested"));
        Serial.println(F("[APP] Clearing session only; preserving nonces"));
#endif
        loRaWAN.clearSessionPersistence();
    }

    if (isBatteryTooLow()) {
#if APP_DEBUG_SERIAL
        Serial.println(F("[APP] Battery too low; sleeping without radio activity"));
#endif
        goToSleep(APP_LOW_BATTERY_SLEEP_SECONDS);
        return;
    }

    loRaWAN.setup(bootCount);

    loRaWAN.setDownlinkCB(
        []([[maybe_unused]] uint8_t fPort, [[maybe_unused]] const uint8_t* downlinkPayload, [[maybe_unused]] std::size_t downlinkSize) {
#if APP_DEBUG_SERIAL
            Serial.print(F("[APP] Downlink payload: fPort="));
            Serial.print(fPort);
            Serial.print(F(", "));
            radio::arrayDump(downlinkPayload, downlinkSize);
#endif
        });

    acquireDataAndPrepareUplink();
}

void loop() {
    loRaWAN.loop();
}
