#if !defined(ESP32)
#error "This example is ESP32 only"
#endif

#include "GPS.h"
#include "LoRaWAN.hpp"

#include <Arduino.h>
#include <Preferences.h>
#include <string>

RTC_DATA_ATTR uint16_t bootCount = 0;

static const uint8_t appKey[16] = {RADIOLIB_LORAWAN_APP_KEY};

#ifdef RADIOLIB_LORAWAN_NWK_KEY
static const uint8_t nwkKey[16] = {RADIOLIB_LORAWAN_NWK_KEY};
#else
static const uint8_t* nwkKey = nullptr;
#endif

static radio::LoRaWAN<RADIOLIB_LORA_MODULE>
    loRaWAN(RADIOLIB_LORA_REGION, RADIOLIB_LORAWAN_JOIN_EUI, RADIOLIB_LORAWAN_DEV_EUI, appKey, nwkKey, RADIOLIB_LORA_MODULE_BITMAP);

static position::GPS gps(GPS_SERIAL_PORT, GPS_SERIAL_BAUD_RATE, GPS_SERIAL_CONFIG, GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN);

static void printWakeupReason() {
    const esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();

    if (wakeupReason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println(F("[APP] Wake from deep sleep"));
    } else {
        Serial.print(F("[APP] Wake not caused by deep sleep: "));
        Serial.println(static_cast<int>(wakeupReason));
    }

    ++bootCount;

    Serial.print(F("[APP] Boot count: "));
    Serial.println(bootCount);
}

void goToSleep(uint32_t seconds) {
    loRaWAN.sleepRadio();
    gps.goToSleep();

    Serial.print(F("[APP] Go to sleep for "));
    Serial.print(seconds);
    Serial.println(F(" seconds"));
    Serial.println();

    Serial.flush();

    esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(seconds) * 1000ULL * 1000ULL);
    esp_deep_sleep_start();

    Serial.println(F("\n\n[APP] Sleep failed, restarting in 5 minutes\n"));
    delay(5UL * 60UL * 1000UL);
    ESP.restart();
}

static std::string buildGpsPayload() {
    if (!gps.isValid()) {
        Serial.println(F("[APP] GPS positioning data not valid"));
        return RADIOLIB_LORAWAN_PAYLOAD;
    }

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

static void acquireDataAndPrepareUplink() {
    Serial.println(F("[APP] Acquire data and construct LoRaWAN uplink"));

    constexpr uint8_t SENSOR_COUNT = 1;

    const uint8_t currentSensor = static_cast<uint8_t>((bootCount - 1) % SENSOR_COUNT);

    uint8_t fPort = 221;
    std::string uplinkPayload = RADIOLIB_LORAWAN_PAYLOAD;

    switch (currentSensor) {
        case 0:
            gps.setup();

            if (gps.isValid()) {
                fPort = 1;
                uplinkPayload = buildGpsPayload();
            } else {
                fPort = 221;
                uplinkPayload = "RadioLib experiment device: Waiting for GPS";
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
    Serial.begin(115200);

    while (!Serial) {
        delay(10);
    }

    delay(2000);

    printWakeupReason();

    Serial.println(F("[APP] Setup"));

    loRaWAN.setup(bootCount);

    loRaWAN.setDownlinkCB([](uint8_t fPort, const uint8_t* downlinkPayload, std::size_t downlinkSize) {
        Serial.print(F("[APP] Downlink payload: fPort="));
        Serial.print(fPort);
        Serial.print(F(", "));
        radio::arrayDump(downlinkPayload, downlinkSize);
    });

    acquireDataAndPrepareUplink();
}

void loop() {
    loRaWAN.loop();
}
