# Student Lego Workflow — LoRaWAN Water Monitoring Buoy

This guide describes the current student workflow for building, configuring, flashing, calibrating, and testing the configurable LoRaWAN water-monitoring firmware.

The project is designed as a lego-style firmware setup: students select the radio module, LoRaWAN profile, TTN device credentials, sensors, pins, and calibration values in the web generator. The generator creates a matching `platformio.ini` and `payload-formatter.js`.

## 1. Project idea

The firmware runs on an ESP32 with a LoRa radio module and selected water-monitoring sensors. The device wakes up periodically, reads one sensor value, sends one LoRaWAN uplink, and then returns to sleep.

Supported modules in the current workflow:

| Module | Purpose | fPort |
|---|---:|---:|
| GPS | position: latitude, longitude, altitude, HDOP | 1 |
| DS18B20 | water temperature in degrees Celsius | 2 |
| PH4502C pH | calibrated pH value | 3 |
| Gravity TDS | calibrated TDS value in ppm | 4 |
| Turbidity sensor | calibrated turbidity value in NTU | 5 |
| PH4502C board temperature | calibrated board/onboard temperature in degrees Celsius | 6 |

The PH4502C board-temperature channel is mandatory in the current generator and payload formatter. It is treated as a separate measurement on fPort 6.

Diagnostics use these fPorts:

| fPort | Meaning |
|---:|---|
| 220 | Request further downlinks |
| 221 | Info / diagnostics |
| 222 | Warning |
| 223 | Error |

## 2. Open the generator

Open the web generator in:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

Use it to configure:

- environment name
- radio module: `SX1262` or `SX1276`
- LoRaWAN version: `LoRaWAN 1.1.0` or `LoRaWAN 1.0.x`
- region, usually `EU868` for the course setup
- TTN device credentials
- sensor selection
- pin mapping
- analog calibration values
- maintenance and calibration pins

## 3. LoRaWAN / TTN setup

Create a device in The Things Networks Sandbox (TTN).

Use OTAA activation.

For this course setup, the generator uses:

```text
JoinEUI = 0000000000000000
```

### LoRaWAN 1.1.0

Use this if the TTN device is configured with the LoRaWAN 1.1.0 key model.

The generator emits:

```text
AppKey
NwkKey
```

### LoRaWAN 1.0.x

Use this if the TTN device uses the older LoRaWAN 1.0.x key model.

The generator emits:

```text
AppKey only
```

The `NwkKey` field is disabled for LoRaWAN 1.0.x.

## 4. Generate `platformio.ini`

In the generator:

1. Enter the TTN credentials.
2. Select the sensor modules and pins.
3. Set the calibration values.
4. Press **Generate platformio.ini**.
5. Download the generated file as `platformio.ini`.
6. Copy it into:

```text
LoRaWAN-HelloWorld-radiolib/
```

The generated file sets the selected environment as `default_envs`. Therefore, in the usual case no `-e` option is needed.

Build:

```bash
pio run
```

Upload:

```bash
pio run -t upload
```

The explicit environment workflow is still valid:

```bash
pio run -e <environment-name>
pio run -e <environment-name> -t upload
```

If the generated file is stored under a non-default filename, pass it with `-c`:

```bash
pio run -c platformio.my-device.ini
pio run -c platformio.my-device.ini -t upload
pio run -c platformio.my-device.ini -e <environment-name>
pio run -c platformio.my-device.ini -e <environment-name> -t upload
```

## 5. Generate the TTN payload formatter

The firmware sends compact text payloads. TTN needs a JavaScript payload formatter to turn those payloads into readable fields.

In the generator:

1. Press **Generate payload formatter**.
2. Either copy the generated formatter directly from the page or download it as `payload-formatter.js`.
3. In TTN, open the application or device payload formatter settings.
4. Select JavaScript uplink formatter.
5. Paste the generated formatter.
6. Save the formatter.

The generated formatter includes only the selected optional sensor fPorts plus the mandatory PH4502C board-temperature fPort 6 and diagnostic fPorts.

Decoded measurement payloads contain only measurement fields. They do not include raw payload strings.

Expected decoded fields:

| fPort | Decoded fields |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

## 6. Sensor pins

The generator uses ESP32 pin numbers. Sensor-side labels are shown in parentheses in the UI.

Current naming convention:

| Generator label | Meaning |
|---|---|
| GPS RX pin (TX) | ESP32 RX connected to GPS TX |
| GPS TX pin (RX) | ESP32 TX connected to GPS RX |
| OneWire data pin (T2) | DS18B20 data pin |
| pH analog pin (P0) | PH4502C pH analog output |
| pH board temperature pin (T1) | PH4502C onboard temperature analog output |
| TDS analog pin (A) | TDS analog output |
| Turbidity analog pin (A) | Turbidity analog output |

## 7. Maintenance and calibration pins

The maintenance pins use internal pull-ups. Connect a button from the selected ESP32 pin to GND.

Hold the button low during power-on, reset, or timer wake-up.

Reset pins:

| Function | Behavior |
|---|---|
| Factory reset | Clears saved LoRaWAN session but preserves nonces |
| Dangerous nonce reset | Clears session and nonces; only use together with TTN nonce reset or a new DevEUI |

Analog calibration buttons:

| Button | Calibration mode |
|---|---|
| pH calibration pin | streams pH raw ADC and calculated pH |
| pH board temp. calibration pin | streams raw ADC and calculated board temperature in °C |
| TDS calibration pin | streams raw ADC and calculated ppm |
| Turbidity calibration pin | streams raw ADC and calculated NTU |

When the button is released, the ESP32 restarts and continues normal LoRaWAN operation.

## 8. Analog calibration model

All analog calibration values in the generator and generated `platformio.ini` are raw ESP32 ADC values, not voltages.

This is important because the real ADC values depend on:

- ESP32 ADC behavior
- selected ADC pin
- supply voltage
- sensor board output range
- possible resistor dividers
- analog filtering

### pH calibration

pH uses three raw ADC calibration points:

| Field | Meaning |
|---|---|
| pH 4 ADC | raw ADC value in pH 4 buffer |
| pH 7 ADC | raw ADC value in pH 7 buffer |
| pH 10 ADC | raw ADC value in pH 10 buffer |

The firmware uses these points for a quadratic fit.

### PH4502C board temperature calibration

The onboard temperature channel uses two raw ADC / temperature points and a linear fit.

Current defaults:

| Field | Default |
|---|---:|
| Low temp ADC | 0 |
| Low temp °C | 0 |
| High temp ADC | 302 |
| High temp °C | 26 |

This value should be interpreted as PH4502C board/onboard temperature, not as precise water temperature. The DS18B20 is the water-temperature sensor.

### TDS calibration

TDS uses two raw ADC / ppm points and a linear fit.

| Field | Meaning |
|---|---|
| Low TDS ADC | raw ADC value in the low/reference solution |
| Low TDS ppm | known ppm value of the low/reference solution |
| High TDS ADC | raw ADC value in the high/reference solution |
| High TDS ppm | known ppm value of the high/reference solution |
| TDS fallback °C | temperature used for compensation if no valid water-temperature value is available |

Only the final TDS measurement value is sent on fPort 4 as `tds_ppm`.

### Turbidity calibration

Turbidity uses two raw ADC / NTU points and a linear fit.

| Field | Meaning |
|---|---|
| Clear water ADC | raw ADC value in clear water |
| Clear water NTU | assigned or reference NTU for clear water |
| Turbid water ADC | raw ADC value in the turbid reference sample |
| Turbid water NTU | assigned or reference NTU for the turbid sample |

Only the final turbidity measurement value is sent on fPort 5 as `turbidity_ntu`.

## 9. Calibration procedure

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
8. Generate and flash a new `platformio.ini` configuration.

Typical calibration output format:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
[CAL] sensor=turbidity, raw_adc=1200.00, calibrated_ntu=600.000
[CAL] sensor=ph_board_temperature, raw_adc=302.00, calibrated_temperature_c=26.000
```

Use stable readings, not the first transient values immediately after power-on.

## 10. Flash and test

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
- decoded payload field in TTN

## 11. Deep sleep and power behavior

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

## 12. Notes for students

- Do not share real AppKey or NwkKey values publicly.
- Use one unique DevEUI per real device.
- For LoRaWAN 1.0.x, the NwkKey field is intentionally disabled.
- For LoRaWAN 1.1.0, both AppKey and NwkKey are required.
- pH probes need buffer solutions and careful handling.
- TDS and turbidity sensors are useful for trends but are not precise chemical analysis instruments.
- The PH4502C board-temperature output is an onboard/board-temperature value, not water temperature.
- The DS18B20 is the water-temperature sensor.
- The generated payload formatter must match the selected sensors and firmware configuration.
