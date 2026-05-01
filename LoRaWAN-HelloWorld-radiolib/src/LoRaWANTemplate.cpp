#if !defined(ESP32)
#error "This example is ESP32 only"
#endif

#include "GPS.h"
#include "LoRaWAN.hpp"

#include <Arduino.h>
#include <Preferences.h>
#include <cmath>
#include <cstddef>
#include <string>

#ifndef APP_HAS_GPS
#define APP_HAS_GPS 1
#endif

#ifndef APP_HAS_TEMPERATURE
#define APP_HAS_TEMPERATURE 1
#endif

#ifndef APP_HAS_PH
#define APP_HAS_PH 1
#endif

#ifndef APP_HAS_TDS
#define APP_HAS_TDS 1
#endif

#ifndef APP_HAS_TURBIDITY
#define APP_HAS_TURBIDITY 1
#endif

#if APP_HAS_TEMPERATURE
#include "sensors/DS18B20.h"
#endif

#if APP_HAS_PH
#include "sensors/PH4502C.h"
#endif

#if APP_HAS_TDS
#include "sensors/TdS.h"
#endif

#if APP_HAS_TURBIDITY
#include "sensors/TurbiditySensor.h"
#endif

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR float lastWaterTemperatureC = NAN;

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

#ifndef TDS_DEFAULT_TEMPERATURE_C
#define TDS_DEFAULT_TEMPERATURE_C 22.0f
#endif

static const uint8_t appKey[16] = {RADIOLIB_LORAWAN_APP_KEY};

#ifdef RADIOLIB_LORAWAN_NWK_KEY
static const uint8_t nwkKey[16] = {RADIOLIB_LORAWAN_NWK_KEY};
#else
static const uint8_t* nwkKey = nullptr;
#endif

static radio::LoRaWAN<RADIOLIB_LORA_MODULE>
    loRaWAN(RADIOLIB_LORA_REGION, RADIOLIB_LORAWAN_JOIN_EUI, RADIOLIB_LORAWAN_DEV_EUI, appKey, nwkKey, RADIOLIB_LORA_MODULE_BITMAP);

#if APP_HAS_GPS
static position::GPS gps(GPS_SERIAL_PORT, GPS_SERIAL_BAUD_RATE, GPS_SERIAL_CONFIG, GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN);
#endif

#if APP_HAS_TEMPERATURE
static temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
#endif

#if APP_HAS_PH
static ph::PH4502C pH(PH4502C_PH_PIN,
                      PH4502C_TEMPERATURE_PIN,
                      {{PH10_ADC_VALUE, 10}, {PH7_ADC_VALUE, 7}, {PH4_ADC_VALUE, 4}});
#endif

#if APP_HAS_TDS
static tds::TdS tdsSensor(TDS_SENSOR_PIN, TDS_SENSOR_VCC, TDS_SENSOR_ADC_RESOLUTION);
#endif

#if APP_HAS_TURBIDITY
static turbidity::TurbiditySensor turbiditySensor(TURBIDITY_PIN,
                                                  TURBIDITY_VCC,
                                                  TURBIDITY_ADC_MAX,
                                                  TURBIDITY_CLEAR_WATER_VOLTAGE,
                                                  TURBIDITY_CLEAR_WATER_NTU,
                                                  TURBIDITY_TURBID_WATER_VOLTAGE,
                                                  TURBIDITY_TURBID_WATER_NTU);
#endif

struct PreparedUplink {
    uint8_t fPort = 221;
    std::string payload;
};

using PrepareSensorUplink = void (*)(PreparedUplink& uplink);

struct SensorSlot {
    const char* name;
    PrepareSensorUplink prepare;
};

static float getWaterTemperatureOrDefault() {
#if APP_HAS_TEMPERATURE
    if (!std::isnan(lastWaterTemperatureC)) {
        return lastWaterTemperatureC;
    }
#endif

    return TDS_DEFAULT_TEMPERATURE_C;
}

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

#if APP_HAS_GPS
    gps.goToSleep();
#endif

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

#if APP_HAS_GPS
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

static void prepareGpsUplink(PreparedUplink& uplink) {
    gps.setup();

    if (gps.isValid()) {
        uplink.fPort = 1;
        uplink.payload = buildGpsPayload();
    } else {
#if APP_DEBUG_SERIAL
        Serial.println(F("[APP] GPS positioning data not valid"));
#endif
        uplink.fPort = 221;
        uplink.payload = "RadioLib experiment device: Waiting for GPS";
    }
}
#endif

#if APP_HAS_TEMPERATURE
static std::string buildTemperaturePayload(float temperatureC) {
    return std::to_string(temperatureC);
}

static void prepareTemperatureUplink(PreparedUplink& uplink) {
    temp.setup();

    if (temp.isValid()) {
        const float temperatureC = temp.getTemperature();
        lastWaterTemperatureC = temperatureC;
        uplink.fPort = 2;
        uplink.payload = buildTemperaturePayload(temperatureC);
    } else {
#if APP_DEBUG_SERIAL
        Serial.println(F("[APP] Temperature sensor data not valid"));
#endif
        uplink.fPort = 221;
        uplink.payload = "RadioLib experiment device: Temperature sensor error";
    }
}
#endif

#if APP_HAS_PH
static std::string buildPhPayload(float phValue) {
    return std::to_string(phValue);
}

static void preparePhUplink(PreparedUplink& uplink) {
    pH.setup();
    uplink.fPort = 3;
    uplink.payload = buildPhPayload(pH.getPHLevel());
}
#endif

#if APP_HAS_TDS
static std::string buildTdsPayload(float tdsValue, float temperatureC) {
    std::string payload;
    payload.reserve(80);
    payload += std::to_string(tdsValue);
    payload += ',';
    payload += std::to_string(temperatureC);
    return payload;
}

static void prepareTdsUplink(PreparedUplink& uplink) {
    const float compensationTemperatureC = getWaterTemperatureOrDefault();
    tdsSensor.setup();
    uplink.fPort = 4;
    uplink.payload = buildTdsPayload(tdsSensor.getValue(compensationTemperatureC), compensationTemperatureC);
}
#endif

#if APP_HAS_TURBIDITY
static std::string buildTurbidityPayload(float ntu) {
    return std::to_string(ntu);
}

static void prepareTurbidityUplink(PreparedUplink& uplink) {
    turbiditySensor.setup();
    uplink.fPort = 5;
    uplink.payload = buildTurbidityPayload(turbiditySensor.getNTU(getWaterTemperatureOrDefault()));
}
#endif

static const SensorSlot sensorSlots[] = {
#if APP_HAS_GPS
    {"GPS", prepareGpsUplink},
#endif
#if APP_HAS_TEMPERATURE
    {"temperature", prepareTemperatureUplink},
#endif
#if APP_HAS_PH
    {"pH", preparePhUplink},
#endif
#if APP_HAS_TDS
    {"TDS", prepareTdsUplink},
#endif
#if APP_HAS_TURBIDITY
    {"turbidity", prepareTurbidityUplink},
#endif
};

static void acquireDataAndPrepareUplink() {
#if APP_DEBUG_SERIAL
    Serial.println(F("[APP] Acquire data and construct LoRaWAN uplink"));
#endif

    PreparedUplink uplink;

    constexpr std::size_t sensorCount = sizeof(sensorSlots) / sizeof(sensorSlots[0]);

    if constexpr (sensorCount == 0) {
        uplink.fPort = 221;
        uplink.payload = "RadioLib experiment device: No sensor enabled";
    } else {
        const std::size_t currentSensor = static_cast<std::size_t>((bootCount - 1) % sensorCount);

#if APP_DEBUG_SERIAL
        Serial.print(F("[APP] Current sensor index: "));
        Serial.println(static_cast<unsigned>(currentSensor));
        Serial.print(F("[APP] Current sensor name: "));
        Serial.println(sensorSlots[currentSensor].name);
#endif

        sensorSlots[currentSensor].prepare(uplink);
    }

    loRaWAN.setUplinkPayload(uplink.fPort, uplink.payload);
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
    Serial.println(F("[APP] Sensor configuration:"));
    Serial.print(F("[APP]   GPS: "));
    Serial.println(APP_HAS_GPS);
    Serial.print(F("[APP]   temperature: "));
    Serial.println(APP_HAS_TEMPERATURE);
    Serial.print(F("[APP]   pH: "));
    Serial.println(APP_HAS_PH);
    Serial.print(F("[APP]   TDS: "));
    Serial.println(APP_HAS_TDS);
    Serial.print(F("[APP]   turbidity: "));
    Serial.println(APP_HAS_TURBIDITY);
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
