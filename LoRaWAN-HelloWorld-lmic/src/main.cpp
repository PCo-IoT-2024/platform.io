/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library
#elif defined(ESP32)
#include <WiFi.h> //ESP32 Core WiFi Library
#else
#error Stop right now, and choose a valid board architecture!
#endif

//
#include <SPI.h>
#include <lmic.h>
//
#include <hal/hal.h>

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#define FILLEDIN
#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#ifndef FILLEDIN
#warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void os_getArtEui(u1_t* buf) {
    memcpy_P(buf, APPEUI, 8);
}

#define VOC_ENDDEVICE_EXP
// #define VOC_ENDDEVICE_EXP_2

// This should also be in little endian format, see above.
#if defined(VOC_ENDDEVICE_EXP)
static const u1_t PROGMEM DEVEUI[8] = {0xFC, 0xA8, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
#elif defined(VOC_ENDDEVICE_EXP_2)
static const u1_t PROGMEM DEVEUI[8] = {0x42, 0xAC, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
#endif
void os_getDevEui(u1_t* buf) {
    memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
#if defined(VOC_ENDDEVICE_EXP)
static const u1_t PROGMEM APPKEY[16] = {0xD5, 0xF5, 0x0F, 0x1D, 0x0B, 0x66, 0xD8, 0x3E, 0x37, 0x05, 0xB9, 0x5A, 0x97, 0xF6, 0x31, 0xDB};
#elif defined(VOC_ENDDEVICE_EXP_2)
static const u1_t PROGMEM APPKEY[16] = {0x18, 0x47, 0xF0, 0xA3, 0x22, 0x66, 0x21, 0xDF, 0x17, 0x86, 0x10, 0x94, 0x9F, 0x09, 0x49, 0x06};
#endif
void os_getDevKey(u1_t* buf) {
    memcpy_P(buf, APPKEY, 16);
}

#if defined(VOC_ENDDEVICE_EXP)
static uint8_t mydata[] = "Hello, world!";
#elif defined(VOC_ENDDEVICE_EXP_2)
static uint8_t mydata[] = "Hello, beautiful world!";
#endif

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20;

// Pin mapping
// const lmic_pinmap lmic_pins = {
//    .nss = 6,
//    .rxtx = LMIC_UNUSED_PIN,
//    .rst = 5,
//    .dio = {2, 3, 4},
//};

const lmic_pinmap lmic_pins = { //
    .nss = 5,                   // chip select
    .rxtx = LMIC_UNUSED_PIN,    // antenna switch send/receive
    .rst = 14,                  // reset pin
    .dio = {2, 4, 15}};

// Hack to avoid diagnostic error messages from clangd for non error
// usage:
//   serial.print[ln](...) instead of
//   Serial.print[ln](...)
Print& serial = (Print&) Serial;

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        serial.print('0');
    serial.print(v, HEX);
}

void do_send(osjob_t* j);

void onEvent(ev_t ev) {
    serial.print(os_getTime());
    serial.print(": ");
    switch (ev) {
        case EV_SCAN_TIMEOUT:
            serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            serial.println(F("EV_JOINED"));
            {
                u4_t netid = 0;
                devaddr_t devaddr = 0;
                u1_t nwkKey[16];
                u1_t artKey[16];
                LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
                serial.print("netid: ");
                serial.println(netid, DEC);
                serial.print("devaddr: ");
                serial.println(devaddr, HEX);
                serial.print("AppSKey: ");
                for (size_t i = 0; i < sizeof(artKey); ++i) {
                    if (i != 0)
                        serial.print("-");
                    printHex2(artKey[i]);
                }
                serial.println("");
                serial.print("NwkSKey: ");
                for (size_t i = 0; i < sizeof(nwkKey); ++i) {
                    if (i != 0)
                        serial.print("-");
                    printHex2(nwkKey[i]);
                }
                serial.println();
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
            /*
                    || This event is defined but not used in the code. No
                    || point in wasting codespace on it.
                    ||
                    || case EV_RFU1:
                    ||     serial.println(F("EV_RFU1"));
                    ||     break;
                    */
        case EV_JOIN_FAILED:
            serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
                serial.println(F("Received ack"));
            if (LMIC.dataLen != 0) {
                serial.print(F("Received "));
                serial.print(LMIC.dataLen);
                serial.println(F(" bytes of payload"));

                serial.print(F("LMIC.dataBeg "));
                serial.println(LMIC.dataBeg);

                serial.print(F("Full Payload as hex: "));
                for (int i = 0; i < LMIC.dataBeg + LMIC.dataLen; i++) {
                    serial.print("0x");
                    serial.print(LMIC.frame[i], HEX);
                    serial.print(" ");
                }
                serial.println();

                serial.print(F("Data Payload as hex: "));
                for (int i = 0; i < LMIC.dataLen; i++) {
                    serial.print("0x");
                    serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
                    serial.print(" ");
                }
                serial.println();

                serial.print(F("Data Payload as string: "));
                serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
                serial.println();
            }

            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
              || This event is defined but not used in the code. No
              || point in wasting codespace on it.
              ||
              || case EV_SCAN_FOUND:
              ||    serial.println(F("EV_SCAN_FOUND"));
              ||    break;
              */
        case EV_TXSTART:
            serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;

        default:
            serial.print(F("Unknown event: "));
            serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        int ret = LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
        serial.print(F("Packet queued: Return value: "));
        serial.println(ret);
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    // Power off WiFi - we use LoRa and not WiFi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

#if defined(ESP32)
    //  Power off Bluetooth - we use LoRa and not Bluetooth
    esp_err_t err = btStop();

    if (err == ESP_OK) {
        printf("Bluetooth stopped successfully\n");
    } else {
        printf("Failed to stop Bluetooth\n");
    }
#endif

    Serial.begin(9600);
    serial.println(F("Starting"));

#ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
#endif

    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
