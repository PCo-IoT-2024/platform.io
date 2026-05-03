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

#define APP_ANY_SENSOR_ENABLED (APP_HAS_GPS || APP_HAS_TEMPERATURE || APP_HAS_PH || APP_HAS_TDS || APP_HAS_TURBIDITY)

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

#ifndef APP_PH_CALIBRATION_PIN
#define APP_PH_CALIBRATION_PIN -1
#endif

#ifndef APP_TURBIDITY_CALIBRATION_PIN
#define APP_TURBIDITY_CALIBRATION_PIN -1
#endif

#ifndef APP_TDS_CALIBRATION_PIN
#define APP_TDS_CALIBRATION_PIN -1
#endif

#ifndef APP_LOW_BATTERY_SLEEP_SECONDS
#define APP_LOW_BATTERY_SLEEP_SECONDS (30UL * 60UL)
#endif

#ifndef TDS_DEFAULT_TEMPERATURE_C
#define TDS_DEFAULT_TEMPERATURE_C 22.0f
#endif

#ifndef TDS_LOW_ADC_VALUE
#define TDS_LOW_ADC_VALUE 0.0f
#endif

#ifndef TDS_LOW_PPM_VALUE
#define TDS_LOW_PPM_VALUE 0.0f
#endif

#ifndef TDS_HIGH_ADC_VALUE
#define TDS_HIGH_ADC_VALUE 1800.0f
#endif

#ifndef TDS_HIGH_PPM_VALUE
#define TDS_HIGH_PPM_VALUE 1000.0f
#endif

#ifndef TURBIDITY_CLEAR_WATER_ADC_VALUE
#define TURBIDITY_CLEAR_WATER_ADC_VALUE 2500.0f
#endif

#ifndef TURBIDITY_CLEAR_WATER_NTU
#define TURBIDITY_CLEAR_WATER_NTU 0.0f
#endif

#ifndef TURBIDITY_TURBID_WATER_ADC_VALUE
#define TURBIDITY_TURBID_WATER_ADC_VALUE 1200.0f
#endif

#ifndef TURBIDITY_TURBID_WATER_NTU
#define TURBIDITY_TURBID_WATER_NTU 600.0f
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
static tds::TdS tdsSensor(TDS_SENSOR_PIN,
                          TDS_LOW_ADC_VALUE,
                          TDS_LOW_PPM_VALUE,
                          TDS_HIGH_ADC_VALUE,
                          TDS_HIGH_PPM_VALUE);
#endif

#if APP_HAS_TURBIDITY
static turbidity::TurbiditySensor turbiditySensor(TURBIDITY_PIN,
                                                  TURBIDITY_CLEAR_WATER_ADC_VALUE,
                                                  TURBIDITY_CLEAR_WATER_NTU,
                                                  TURBIDITY_TURBID_WATER_ADC_VALUE,
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
        Serial.println(F("[APP] Wake from deep sleep timer"));
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

static bool isLowPinRequested(int pin) {
    if (pin < 0) {
        return false;
    }

    pinMode(static_cast<uint8_t>(pin), INPUT_PULLUP);
    delay(20);
    return digitalRead(static_cast<uint8_t>(pin)) == LOW;
}

static bool isFactoryResetRequested() {
    return isLowPinRequested(APP_FACTORY_RESET_PIN);
}

static bool isDangerousNonceResetRequested() {
    return isLowPinRequested(APP_DANGEROUS_NONCE_RESET_PIN);
}

static void printCalibrationSample(const __FlashStringHelper* sensor, float rawAdc, const __FlashStringHelper* valueName, float calibratedValue) {
    Serial.print(F("[CAL] sensor="));
    Serial.print(sensor);
    Serial.print(F(", raw_adc="));
    Serial.print(rawAdc, 2);
    Serial.print(F(", calibrated_"));
    Serial.print(valueName);
    Serial.print(F("="));
    Serial.println(calibratedValue, 3);
}

static void restartAfterCalibrationButtonRelease() {
#if APP_DEBUG_SERIAL
    Serial.println(F("[CAL] Release detected; restarting for normal operation"));
    Serial.flush();
#endif

    delay(250);
    ESP.restart();

    while (true) {
        delay(1000);
    }
}

#if APP_HAS_PH && APP_PH_CALIBRATION_PIN >= 0
static void runPhCalibrationMode() {
    pinMode(APP_PH_CALIBRATION_PIN, INPUT_PULLUP);
    pH.setup();

    Serial.println(F("[CAL] sensor=ph, mode=active"));
    Serial.println(F("[CAL] format: sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>"));
    Serial.println(F("[CAL] keep_button_pressed_to_stream=1, release_button_to_restart=1"));
    Serial.println(F("[CAL] note=Use raw_adc values measured in pH 4, pH 7 and pH 10 buffer solutions"));

    while (digitalRead(APP_PH_CALIBRATION_PIN) == LOW) {
        const float rawAdc = pH.readADC();
        const float calibratedPh = pH.getPHLevel();
        printCalibrationSample(F("ph"), rawAdc, F("ph"), calibratedPh);
        Serial.flush();
        delay(1000);
    }

    restartAfterCalibrationButtonRelease();
}
#endif

#if APP_HAS_TDS && APP_TDS_CALIBRATION_PIN >= 0
static void runTdsCalibrationMode() {
    pinMode(APP_TDS_CALIBRATION_PIN, INPUT_PULLUP);
    tdsSensor.setup();

    Serial.println(F("[CAL] sensor=tds, mode=active"));
    Serial.println(F("[CAL] format: sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>"));
    Serial.println(F("[CAL] keep_button_pressed_to_stream=1, release_button_to_restart=1"));
    Serial.println(F("[CAL] note=Use raw_adc values and ppm reference solutions for the linear TDS fit"));

    while (digitalRead(APP_TDS_CALIBRATION_PIN) == LOW) {
        const float rawAdc = tdsSensor.readADC();
        const float calibratedPpm = tdsSensor.getValueFromADC(rawAdc, getWaterTemperatureOrDefault());
        printCalibrationSample(F("tds"), rawAdc, F("ppm"), calibratedPpm);
        Serial.flush();
        delay(1000);
    }

    restartAfterCalibrationButtonRelease();
}
#endif

#if APP_HAS_TURBIDITY && APP_TURBIDITY_CALIBRATION_PIN >= 0
static void runTurbidityCalibrationMode() {
    pinMode(APP_TURBIDITY_CALIBRATION_PIN, INPUT_PULLUP);
    turbiditySensor.setup();

    Serial.println(F("[CAL] sensor=turbidity, mode=active"));
    Serial.println(F("[CAL] format: sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>"));
    Serial.println(F("[CAL] keep_button_pressed_to_stream=1, release_button_to_restart=1"));
    Serial.println(F("[CAL] note=Use raw_adc values and NTU reference samples for the linear turbidity fit"));

    while (digitalRead(APP_TURBIDITY_CALIBRATION_PIN) == LOW) {
        const float rawAdc = turbiditySensor.readADC();
        const float calibratedNtu = turbiditySensor.getNTUFromADC(rawAdc, getWaterTemperatureOrDefault());
        printCalibrationSample(F("turbidity"), rawAdc, F("ntu"), calibratedNtu);
        Serial.flush();
        delay(1000);
    }

    restartAfterCalibrationButtonRelease();
}
#endif

static void enterCalibrationModeIfRequested() {
#if APP_HAS_PH && APP_PH_CALIBRATION_PIN >= 0
    if (isLowPinRequested(APP_PH_CALIBRATION_PIN)) {
        runPhCalibrationMode();
    }
#endif

#if APP_HAS_TDS && APP_TDS_CALIBRATION_PIN >= 0
    if (isLowPinRequested(APP_TDS_CALIBRATION_PIN)) {
        runTdsCalibrationMode();
    }
#endif

#if APP_HAS_TURBIDITY && APP_TURBIDITY_CALIBRATION_PIN >= 0
    if (isLowPinRequested(APP_TURBIDITY_CALIBRATION_PIN)) {
        runTurbidityCalibrationMode();
    }
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
#if !APP_ANY_SENSOR_ENABLED
    {"none", nullptr},
#endif
};

static void acquireDataAndPrepareUplink() {
#if APP_DEBUG_SERIAL
    Serial.println(F("[APP] Acquire data and construct LoRaWAN uplink"));
#endif

    PreparedUplink uplink;

#if !APP_ANY_SENSOR_ENABLED
    uplink.fPort = 221;
    uplink.payload = "RadioLib experiment device: No sensor enabled";
#else
    constexpr std::size_t sensorCount = sizeof(sensorSlots) / sizeof(sensorSlots[0]);
    const std::size_t currentSensor = static_cast<std::size_t>((bootCount - 1) % sensorCount);

#if APP_DEBUG_SERIAL
    Serial.print(F("[APP] Current sensor index: "));
    Serial.println(static_cast<unsigned>(currentSensor));
    Serial.print(F("[APP] Current sensor name: "));
    Serial.println(sensorSlots[currentSensor].name);
#endif

    sensorSlots[currentSensor].prepare(uplink);
#endif

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

    enterCalibrationModeIfRequested();

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
