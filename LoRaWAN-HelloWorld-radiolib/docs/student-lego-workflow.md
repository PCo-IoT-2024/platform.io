# Student Guide — LoRaWAN Water Monitoring Buoy Lego Firmware

This guide describes the current workflow for the configurable LoRaWAN water-monitoring buoy firmware on branch:

```text
course/lego-configurable
```

The current workflow is generator-based: students use the web generator to create a matching `platformio.ini` and `payload-formatter.js` for their selected radio module, LoRaWAN version, TTN credentials, sensors, pins, and calibration values.

---

# 1. Current project state

The firmware runs on an ESP32 with an SX1262 or SX1276 LoRa radio module. It wakes up periodically, reads one selected sensor value, sends one LoRaWAN uplink, and returns to deep sleep.

Current measurement fPorts:

| fPort | Measurement | Decoded TTN field |
|---:|---|---|
| 1 | GPS position | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | DS18B20 water temperature | `temperature_c` |
| 3 | PH4502C pH | `ph_level` |
| 4 | Gravity TDS | `tds_ppm` |
| 5 | Turbidity | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

Diagnostic fPorts:

| fPort | Meaning |
|---:|---|
| 220 | Request further downlinks |
| 221 | Info / diagnostics |
| 222 | Warning |
| 223 | Error |

The PH4502C board-temperature channel is mandatory in the current generator and payload formatter. It is sent as a separate measurement on fPort 6.

---

# 2. Important paths

The PlatformIO project is located in:

```text
LoRaWAN-HelloWorld-radiolib/
```

The web generator is located in:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

The firmware entry point is:

```text
LoRaWAN-HelloWorld-radiolib/src/LoRaWANTemplate.cpp
```

The sensor implementations are in:

```text
LoRaWAN-HelloWorld-radiolib/src/sensors/
```

This guide is:

```text
LoRaWAN-HelloWorld-radiolib/docs/student-lego-workflow.md
```

---

# 3. Recommended student git workflow

Fork the course repository:

```text
PCo-IoT-2024/platform.io
```

Clone your fork:

```bash
git clone git@github.com:<your-github-user>/platform.io.git
cd platform.io
```

Add the course repository as upstream:

```bash
git remote add upstream git@github.com:PCo-IoT-2024/platform.io.git
git fetch upstream
```

Create your own working branch from the lego branch:

```bash
git switch -c my-water-buoy upstream/course/lego-configurable
```

Enter the PlatformIO project:

```bash
cd LoRaWAN-HelloWorld-radiolib
```

---

# 4. Open the generator

Open this file in a browser:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

The generator configures:

- PlatformIO environment name
- radio module: `SX1262` or `SX1276`
- LoRaWAN version: `LoRaWAN 1.1.0` or `LoRaWAN 1.0.x`
- LoRaWAN region, usually `EU868` for the course setup
- TTN DevEUI, AppKey, and optionally NwkKey
- selected sensors and sensor pins
- LoRa module pin map
- maintenance and calibration pins
- analog raw-ADC calibration values
- TTN payload formatter

---

# 5. LoRaWAN and TTN setup

Create an OTAA device in The Things Networks Sandbox (TTN).

For this course setup, the generated firmware uses:

```text
JoinEUI = 0000000000000000
```

## LoRaWAN 1.1.0

Select `LoRaWAN 1.1.0` when the TTN device uses the LoRaWAN 1.1.0 key model.

The generated `platformio.ini` includes:

```text
AppKey
NwkKey
```

## LoRaWAN 1.0.x

Select `LoRaWAN 1.0.x` when the TTN device uses the older LoRaWAN 1.0.x key model.

The generated `platformio.ini` includes:

```text
AppKey only
```

The `NwkKey` input is disabled for LoRaWAN 1.0.x.

---

# 6. Generate and use `platformio.ini`

In the generator:

1. Enter TTN credentials.
2. Select radio module, LoRaWAN version, and region.
3. Select and configure sensors.
4. Configure pins and raw-ADC calibration values.
5. Press **Generate platformio.ini**.
6. Download the file as `platformio.ini`.
7. Copy it into:

```text
LoRaWAN-HelloWorld-radiolib/
```

The generated file sets the selected environment as `default_envs`. Therefore the usual commands do not need `-e`.

Build:

```bash
pio run
```

Upload:

```bash
pio run -t upload
```

Open the serial monitor:

```bash
pio device monitor
```

The explicit environment workflow is still valid:

```bash
pio run -e <environment-name>
pio run -e <environment-name> -t upload
```

If the generated INI file has a non-default name, pass it with `-c`:

```bash
pio run -c platformio.my-device.ini
pio run -c platformio.my-device.ini -t upload
pio run -c platformio.my-device.ini -e <environment-name>
pio run -c platformio.my-device.ini -e <environment-name> -t upload
```

---

# 7. Generate and install the TTN payload formatter

The firmware sends compact text payloads. TTN needs a JavaScript uplink formatter to decode them.

In the generator:

1. Press **Generate payload formatter**.
2. Copy the generated code from the page or download it as `payload-formatter.js`.
3. In TTN, open the application or device payload formatter settings.
4. Select JavaScript uplink formatter.
5. Paste the generated formatter.
6. Save the formatter.

The generated formatter includes only selected optional sensor fPorts plus the mandatory PH4502C board-temperature fPort 6 and the diagnostic fPorts.

Decoded measurement payloads contain only decoded measurement fields. They do not include `payload_raw`.

Expected decoded fields:

| fPort | Decoded fields |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

---

# 8. Sensor pins

The generator uses ESP32 pin numbers. Sensor-side names are shown in parentheses.

| Generator label | Meaning |
|---|---|
| GPS RX pin (TX) | ESP32 RX connected to GPS TX |
| GPS TX pin (RX) | ESP32 TX connected to GPS RX |
| OneWire data pin (T2) | DS18B20 data pin |
| pH analog pin (P0) | PH4502C pH analog output |
| pH board temperature pin (T1) | PH4502C onboard temperature analog output |
| TDS analog pin (A) | TDS analog output |
| Turbidity analog pin (A) | Turbidity analog output |

The default LoRa module pin map is:

```text
5, 2, 14, 4
```

For SX1262 the order is:

```text
NSS / CS, DIO1 / IRQ, RESET, BUSY
```

For SX1276 the order is:

```text
NSS / CS, DIO0 / IRQ, RESET, DIO1
```

---

# 9. Maintenance and calibration pins

The maintenance and calibration pins use internal pull-ups. Connect a button from the selected ESP32 pin to GND.

Hold the button low during power-on, reset, or normal timer wake-up.

Reset buttons:

| Function | Behavior |
|---|---|
| Factory reset | Clears the saved LoRaWAN session but preserves nonces |
| Dangerous nonce reset | Clears session and nonces; only use together with TTN nonce reset or a new DevEUI |

Analog calibration buttons:

| Button | Calibration mode |
|---|---|
| pH calibration pin | streams pH raw ADC and calculated pH |
| pH board temp. calibration pin | streams raw ADC and calculated board temperature in °C |
| TDS calibration pin | streams raw ADC and calculated ppm |
| Turbidity calibration pin | streams raw ADC and calculated NTU |

When the calibration button is released, the ESP32 restarts and continues normal LoRaWAN operation.

---

# 10. Analog calibration model

All analog calibration values in the generator and generated `platformio.ini` are raw ESP32 ADC values, not voltages.

This is important because the real raw ADC value depends on:

- ESP32 ADC behavior
- selected ADC pin
- sensor board supply voltage
- sensor board output range
- possible resistor dividers
- analog filtering

## pH calibration

pH uses three raw ADC calibration points:

| Field | Meaning |
|---|---|
| pH 4 ADC | raw ADC value in pH 4 buffer |
| pH 7 ADC | raw ADC value in pH 7 buffer |
| pH 10 ADC | raw ADC value in pH 10 buffer |

The firmware uses these points for a quadratic fit.

## PH4502C board-temperature calibration

The PH4502C onboard temperature channel uses two raw ADC / temperature points and a linear fit.

Current generator defaults:

| Field | Default |
|---|---:|
| Low temp ADC | 0 |
| Low temp °C | 0 |
| High temp ADC | 302 |
| High temp °C | 26 |

Interpret this value as PH4502C board/onboard temperature, not as precise water temperature. The DS18B20 is the water-temperature sensor.

## TDS calibration

TDS uses two raw ADC / ppm points and a linear fit.

| Field | Meaning |
|---|---|
| Low TDS ADC | raw ADC value in the low/reference solution |
| Low TDS ppm | known ppm value of the low/reference solution |
| High TDS ADC | raw ADC value in the high/reference solution |
| High TDS ppm | known ppm value of the high/reference solution |
| TDS fallback °C | temperature used for compensation if no valid DS18B20 water-temperature value is available |

Only the final TDS measurement value is sent on fPort 4 as `tds_ppm`.

## Turbidity calibration

Turbidity uses two raw ADC / NTU points and a linear fit.

| Field | Meaning |
|---|---|
| Clear water ADC | raw ADC value in clear water |
| Clear water NTU | assigned or reference NTU for clear water |
| Turbid water ADC | raw ADC value in the turbid reference sample |
| Turbid water NTU | assigned or reference NTU for the turbid sample |

Only the final turbidity measurement value is sent on fPort 5 as `turbidity_ntu`.

---

# 11. Calibration procedure

Open the serial monitor:

```bash
pio device monitor
```

For each analog sensor:

1. Hold the corresponding calibration button low.
2. Power on or reset the ESP32.
3. Keep the button pressed.
4. Read the `[CAL]` output from the serial monitor.
5. Record the raw ADC value for the known reference condition.
6. Release the button to restart the ESP32.
7. Enter the recorded raw ADC values into the generator.
8. Generate and flash a new `platformio.ini`.

Typical calibration output format:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
[CAL] sensor=turbidity, raw_adc=1200.00, calibrated_ntu=600.000
[CAL] sensor=ph_board_temperature, raw_adc=302.00, calibrated_temperature_c=26.000
```

Use stable readings, not the first transient values immediately after power-on.

---

# 12. Sensor behavior and uplinks

The scheduler rotates through enabled sensors using the RTC boot counter. The device does not measure every sensor on every wake-up.

Example with all current measurement channels enabled:

```text
wake 1 -> GPS                  -> fPort 1
wake 2 -> DS18B20 temperature  -> fPort 2
wake 3 -> pH                   -> fPort 3
wake 4 -> TDS                  -> fPort 4
wake 5 -> turbidity            -> fPort 5
wake 6 -> PH4502C board temp   -> fPort 6
wake 7 -> GPS                  -> fPort 1
```

If GPS has no fix, the firmware sends an info message on fPort 221.

---

# 13. Sensor notes

## GPS

Sends latitude, longitude, altitude, and HDOP on fPort 1. Indoors, GPS may not get a valid fix. In that case, a diagnostic message is sent on fPort 221.

## DS18B20

Sends water temperature in °C on fPort 2. The OneWire data line needs a pull-up resistor, typically 4.7 kΩ.

## PH4502C pH

Sends only the calibrated pH value on fPort 3. The pH raw ADC value is used only during serial calibration mode and is not sent via LoRaWAN.

## PH4502C board temperature

Sends only the calibrated board/onboard temperature in °C on fPort 6. This is not water temperature and should mainly be treated as diagnostic board-temperature information.

## Gravity TDS

Sends only the calibrated TDS value in ppm on fPort 4. The DS18B20 water temperature may be used internally for compensation, but the compensation temperature is not sent with the TDS uplink.

## Turbidity

Sends only the calibrated turbidity value in NTU on fPort 5.

---

# 14. Deep sleep and power behavior

The firmware is intended for battery/solar operation.

During normal operation:

1. ESP32 wakes up.
2. One sensor slot is selected.
3. The selected sensor is read.
4. One LoRaWAN uplink is sent.
5. The radio is put to sleep.
6. The ESP32 enters deep sleep until the next interval.

External analog sensor boards such as PH4502C, TDS, and turbidity do not have a reliable software deep-sleep mode. For the final buoy hardware, switch their supply rails with MOSFETs or load switches.

Recommended power domains:

| Rail | Loads |
|---|---|
| `3V3_MAIN` | ESP32 and LoRa radio |
| switched `3V3_SENS` | GPS and DS18B20 |
| switched `5V_SENS` | PH4502C, TDS, turbidity |

---

# 15. Flash and test checklist

After generating and copying `platformio.ini`:

```bash
pio run
pio run -t upload
pio device monitor
```

Watch for:

- successful OTAA join or restored LoRaWAN session
- selected sensor name
- sent fPort
- TTN live data uplink
- decoded payload fields in TTN

---

# 16. Notes for students

- Do not share real AppKey or NwkKey values publicly.
- Use one unique DevEUI per real device.
- For LoRaWAN 1.0.x, the NwkKey field is intentionally disabled.
- For LoRaWAN 1.1.0, both AppKey and NwkKey are required.
- pH probes need buffer solutions and careful handling.
- TDS and turbidity sensors are useful for trends but are not precise chemical analysis instruments.
- The PH4502C board-temperature output is an onboard/board-temperature value, not water temperature.
- The DS18B20 is the water-temperature sensor.
- The generated payload formatter must match the selected sensors and firmware configuration.
