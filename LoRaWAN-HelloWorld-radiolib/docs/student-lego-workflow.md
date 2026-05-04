# Student Guide — LoRaWAN Water Monitoring Buoy Lego Firmware

This guide explains how to work with the configurable “lego” firmware branch of the LoRaWAN water-monitoring buoy project.

The current branch is:

```text
course/lego-configurable
```

The current workflow is generator-based. Students use the web generator to create two files that must match each other:

```text
platformio.ini
payload-formatter.js
```

The generated `platformio.ini` configures the firmware build. The generated `payload-formatter.js` is installed in The Things Networks Sandbox (TTN) so that TTN can decode uplinks into meaningful fields.

The goal of this branch is that each student group can configure its own buoy firmware by selecting the sensors, radio module, pins, LoRaWAN version, TTN credentials, and calibration values it actually uses.

---

# 1. System overview

The node consists of:

- ESP32 microcontroller
- SX1262 or SX1276 LoRa radio module
- GPS module
- DS18B20 water-temperature sensor
- PH4502C pH module
- PH4502C onboard/board-temperature channel
- Gravity TDS sensor
- analog turbidity sensor

The firmware is designed for low-power operation. In normal operation the ESP32 wakes up, reads one sensor value, sends one LoRaWAN uplink, and returns to deep sleep.

The basic runtime model is:

```text
wake up
select current sensor
read selected sensor
send one LoRaWAN uplink
put radio to sleep
enter ESP32 deep sleep
```

The device does not measure all sensors during every wake-up. Instead, it rotates through the enabled sensor list. This makes each wake-up shorter and saves energy.

Example with all current measurement channels enabled:

```text
wake 1 -> GPS                  -> fPort 1
wake 2 -> DS18B20 temperature  -> fPort 2
wake 3 -> PH4502C pH           -> fPort 3
wake 4 -> Gravity TDS          -> fPort 4
wake 5 -> Turbidity            -> fPort 5
wake 6 -> PH4502C board temp   -> fPort 6
wake 7 -> GPS                  -> fPort 1
```

---

# 2. Current fPort mapping

Current measurement fPorts:

| fPort | Measurement | Sensor / module | Decoded TTN field |
|---:|---|---|---|
| 1 | GPS position | GPS module | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | Water temperature | DS18B20 | `temperature_c` |
| 3 | pH | PH4502C pH board | `ph_level` |
| 4 | Total dissolved solids | Gravity TDS | `tds_ppm` |
| 5 | Turbidity | Analog turbidity sensor | `turbidity_ntu` |
| 6 | PH4502C board temperature | PH4502C T1 / onboard temperature output | `ph_board_temperature_c` |

Diagnostic fPorts:

| fPort | Meaning |
|---:|---|
| 220 | Request further downlinks |
| 221 | Info / diagnostics |
| 222 | Warning |
| 223 | Error |

Important current behavior:

- fPort 3 sends only the pH value.
- fPort 4 sends only the TDS ppm value.
- fPort 5 sends only the turbidity NTU value.
- fPort 6 sends only the PH4502C board-temperature value.
- Measurement payloads do not include `payload_raw` in the decoded TTN output.

---

# 3. Repository structure

The PlatformIO project is located in:

```text
LoRaWAN-HelloWorld-radiolib/
```

Important directories and files:

```text
LoRaWAN-HelloWorld-radiolib/
├── src/
│   ├── LoRaWANTemplate.cpp
│   ├── LoRaWAN.hpp
│   ├── GPS.cpp
│   ├── GPS.h
│   └── sensors/
│       ├── DS18B20.cpp
│       ├── DS18B20.h
│       ├── PH4502C.cpp
│       ├── PH4502C.h
│       ├── TdS.cpp
│       ├── TdS.h
│       ├── TurbiditySensor.cpp
│       └── TurbiditySensor.h
├── tools/
│   └── platformio-ini-generator/
│       └── www/
│           ├── index.html
│           ├── app.js
│           ├── dynamic-payload-formatter.js
│           └── style.css
└── docs/
    └── student-lego-workflow.md
```

## 3.1 Firmware source

The main firmware file is:

```text
src/LoRaWANTemplate.cpp
```

It handles:

- sensor object construction
- compile-time sensor selection
- calibration mode detection
- LoRaWAN setup
- session restore
- uplink preparation
- sensor rotation
- deep sleep

The sensor classes are in:

```text
src/sensors/
```

## 3.2 Generator source

The generator is located in:

```text
tools/platformio-ini-generator/www/
```

The important files are:

| File | Purpose |
|---|---|
| `index.html` | page structure, input fields, documentation blocks |
| `style.css` | layout, cards, responsive behavior |
| `app.js` | core `platformio.ini` generation |
| `dynamic-payload-formatter.js` | dynamic payload formatter and several current generator extensions |

Some current generator behavior is injected by `dynamic-payload-formatter.js`. This is functional, but a later cleanup could move these pieces directly into `index.html` and `app.js`.

---

# 4. Student git workflow

Each student group should work in its own fork and branch.

Fork:

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

Create a working branch from the lego branch:

```bash
git switch -c my-water-buoy upstream/course/lego-configurable
```

Enter the PlatformIO project:

```bash
cd LoRaWAN-HelloWorld-radiolib
```

Do not commit directly to `course/lego-configurable` unless explicitly instructed.

---

# 5. Open the generator

Open this local file in a browser:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

The generator configures:

- environment name
- radio module
- LoRaWAN version
- region
- uplink interval
- TTN credentials
- sensor selection
- ESP32 sensor pins
- LoRa module pin map
- maintenance and calibration pins
- raw ADC calibration values
- TTN payload formatter

The generator page contains the following major areas:

1. Device and LoRaWAN
2. TTN Credentials
3. Maintenance and calibration pins
4. Sensor modules
5. PlatformIO INI file
6. TTN payload formatter
7. Usage notes and fPort documentation

---

# 6. Device and LoRaWAN configuration

## 6.1 Environment name

The environment name becomes the PlatformIO environment name.

Example:

```text
water_buoy_sx1262
```

The generated INI contains:

```ini
[env:water_buoy_sx1262]
```

The generator also writes this environment as the default:

```ini
[platformio]
default_envs = water_buoy_sx1262
```

Therefore the usual build and upload commands do not need `-e`.

## 6.2 Radio module

Current radio options:

```text
SX1262
SX1276
```

Select the module that is actually wired to the ESP32. The radio selection affects the RadioLib module type and the meaning of the pin map.

## 6.3 LoRaWAN version

Current visible options:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

For LoRaWAN 1.1.0, the generator expects and emits:

```text
AppKey
NwkKey
```

For LoRaWAN 1.0.x, the generator emits:

```text
AppKey only
```

The NwkKey field is disabled for LoRaWAN 1.0.x.

## 6.4 Region

For the course setup in Europe, use:

```text
EU868
```

Other region options are available for completeness, for example:

```text
US915
EU433
AU915
CN470
AS923
AS923_2
AS923_3
AS923_4
KR920
IN865
```

The selected region must match the legal frequency plan and the TTN device configuration.

## 6.5 Uplink interval

The uplink interval is given in seconds.

Short intervals are useful during lab work, but they consume more energy and create more LoRaWAN traffic.

Typical values:

| Use case | Typical interval |
|---|---:|
| debugging in the lab | 30–120 s |
| classroom demonstration | 60–300 s |
| outdoor test | 5–15 min |
| long-term solar operation | 15–60 min |

---

# 7. TTN credentials

The TTN credentials card contains:

```text
DevEUI
AppKey
NwkKey
```

Each key can be entered in several formats. The generator normalizes the values for the generated PlatformIO build flags.

## 7.1 DevEUI

The DevEUI identifies the end device.

Example direct hex format:

```text
70B3D57ED007733D
```

The generated PlatformIO file emits the DevEUI in RadioLib-compatible `0x...` form.

The DevEUI must match the device registered in TTN.

## 7.2 AppKey

The AppKey is an OTAA root key.

Example direct hex format:

```text
00112233445566778899AABBCCDDEEFF
```

The generated PlatformIO file emits it as comma-separated bytes, for example:

```text
0x00, 0x11, 0x22, ...
```

Do not publish real AppKeys in screenshots, repositories, or public documents.

## 7.3 NwkKey

The NwkKey is used for LoRaWAN 1.1.0.

It is intentionally disabled for LoRaWAN 1.0.x in the current generator.

When enabled, it is emitted as comma-separated bytes in the generated INI.

---

# 8. Generate and use `platformio.ini`

In the generator:

1. Enter TTN credentials.
2. Select radio module, LoRaWAN version, and region.
3. Select and configure sensors.
4. Configure pins.
5. Enter raw ADC calibration values.
6. Press **Generate platformio.ini**.
7. Download the file.
8. Save or rename it as `platformio.ini`.
9. Copy it into:

```text
LoRaWAN-HelloWorld-radiolib/
```

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

If the generated file has a custom name, use `-c`:

```bash
pio run -c platformio.my-device.ini
pio run -c platformio.my-device.ini -t upload
```

---

# 9. Generate and install the TTN payload formatter

The firmware sends compact text payloads. TTN receives bytes and needs a JavaScript formatter to decode them.

In the generator:

1. Press **Generate payload formatter**.
2. Copy the generated formatter from the page or download `payload-formatter.js`.
3. In TTN, open the application or end-device uplink formatter settings.
4. Choose a custom JavaScript formatter.
5. Paste the generated formatter.
6. Save it.

The generated formatter must match the generated `platformio.ini`.

If sensor selections change, regenerate both files:

```text
platformio.ini
payload-formatter.js
```

Decoded measurement fields:

| fPort | Decoded fields |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

The current measurement formatter does not emit `payload_raw`.

---

# 10. Sensor selection and rotation

The generator emits compile-time build flags such as:

```ini
-D APP_HAS_GPS=1
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=1
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=1
-D APP_HAS_PH_BOARD_TEMPERATURE=1
```

The board-temperature channel is mandatory in the current workflow:

```ini
-D APP_HAS_PH_BOARD_TEMPERATURE=1
```

The firmware builds a sensor table from the enabled flags. The RTC boot counter selects the current sensor.

Example sensor table:

```text
GPS
temperature
pH
TDS
turbidity
PH4502C board temperature
```

Example rotation:

```text
wake 1 -> GPS
wake 2 -> temperature
wake 3 -> pH
wake 4 -> TDS
wake 5 -> turbidity
wake 6 -> PH4502C board temperature
wake 7 -> GPS
```

---

# 11. Pin naming and defaults

The generator uses ESP32 pin names first. Sensor-side names are shown in parentheses.

| Generator label | Meaning |
|---|---|
| GPS RX pin (TX) | ESP32 RX connected to GPS TX |
| GPS TX pin (RX) | ESP32 TX connected to GPS RX |
| OneWire data pin (T2) | DS18B20 data pin |
| pH analog pin (P0) | PH4502C pH analog output |
| pH board temperature pin (T1) | PH4502C onboard temperature analog output |
| TDS analog pin (A) | TDS analog output |
| Turbidity analog pin (A) | Turbidity analog output |

Typical defaults:

| Function | ESP32 GPIO |
|---|---:|
| GPS RX pin (TX) | 16 |
| GPS TX pin (RX) | 17 |
| DS18B20 OneWire data pin | 27 |
| pH analog pin | 34 |
| pH board temperature pin | 35 |
| TDS analog pin | 32 |
| Turbidity analog pin | 33 |

ADC-capable ESP32 pins must be used for analog sensors.

---

# 12. LoRa module pin map

The default LoRa module pin map is:

```text
5, 2, 14, 4
```

For SX1262 the order is:

```text
NSS / CS
DIO1 / IRQ
RESET
BUSY
```

For SX1276 the order is:

```text
NSS / CS
DIO0 / IRQ
RESET
DIO1
```

Wrong LoRa pins usually result in radio initialization failure, join failure, or no uplinks.

---

# 13. Maintenance and calibration pins

The maintenance and calibration pins use internal pull-ups. Connect a button from the selected GPIO to GND.

```text
ESP32 GPIO ---- pushbutton ---- GND
```

Hold the button low during power-on, reset, or timer wake-up.

Reset buttons:

| Button | Behavior |
|---|---|
| Factory reset | clears saved LoRaWAN session but preserves nonces |
| Dangerous nonce reset | clears session and nonces |

Use dangerous nonce reset carefully. TTN may reject reused DevNonces if the network side was not reset too.

Analog calibration buttons:

| Button | Calibration mode |
|---|---|
| pH calibration pin | streams raw ADC and calculated pH |
| pH board temp. calibration pin | streams raw ADC and calculated board temperature |
| TDS calibration pin | streams raw ADC and calculated ppm |
| Turbidity calibration pin | streams raw ADC and calculated NTU |

Releasing the calibration button restarts the ESP32 into normal operation.

---

# 14. Raw ADC calibration principle

All analog calibration values in the generator and generated `platformio.ini` are raw ESP32 ADC values.

They are not voltages.

This makes the calibration process consistent with the serial output:

```text
[CAL] sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>
```

Raw ADC values include all real-world effects:

- ESP32 ADC behavior
- selected ADC pin
- sensor board supply voltage
- sensor board output range
- resistor dividers
- RC filters
- wiring
- tolerances

Do not enter voltages into ADC fields.

---

# 15. pH calibration

pH uses three raw ADC calibration points:

| Field | Meaning |
|---|---|
| pH 4 ADC | raw ADC value in pH 4 buffer |
| pH 7 ADC | raw ADC value in pH 7 buffer |
| pH 10 ADC | raw ADC value in pH 10 buffer |

The firmware uses these three points for a quadratic fit.

Recommended process:

1. Rinse the probe.
2. Place it in pH 7 buffer.
3. Wait until the reading is stable.
4. Record raw ADC.
5. Repeat for pH 4 and pH 10.
6. Enter the values into the generator.
7. Regenerate and flash.

Example calibration output:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
```

The pH LoRaWAN uplink sends only the calibrated pH value on fPort 3.

---

# 16. PH4502C board-temperature calibration

The PH4502C board-temperature channel uses the analog T1/onboard temperature output.

It is sent on fPort 6 as:

```text
ph_board_temperature_c
```

Current generator defaults:

| Field | Default |
|---|---:|
| Low temp ADC | 0 |
| Low temp °C | 0 |
| High temp ADC | 302 |
| High temp °C | 26 |

The firmware uses a two-point linear fit.

This value should be interpreted as PH4502C board/onboard temperature, not as precise water temperature. It may be affected by sunlight, enclosure temperature, board self-heating, and electronics nearby.

Use the DS18B20 for water temperature.

---

# 17. TDS calibration

TDS uses two raw ADC / ppm points and a linear fit.

| Field | Meaning |
|---|---|
| Low TDS ADC | raw ADC value in the low/reference solution |
| Low TDS ppm | known ppm value of the low/reference solution |
| High TDS ADC | raw ADC value in the high/reference solution |
| High TDS ppm | known ppm value of the high/reference solution |
| TDS fallback °C | temperature used if no valid water-temperature value is available |

Current fPort 4 behavior:

```text
tds_ppm only
```

The compensation temperature may be used internally, but it is not sent with the TDS uplink.

Example decoded TTN payload:

```json
{
  "tds_ppm": 350.0
}
```

---

# 18. Turbidity calibration

Turbidity uses two raw ADC / NTU points and a linear fit.

| Field | Meaning |
|---|---|
| Clear water ADC | raw ADC value in clear water |
| Clear water NTU | assigned or reference NTU for clear water |
| Turbid water ADC | raw ADC value in a turbid reference sample |
| Turbid water NTU | assigned or reference NTU for the turbid sample |

Current fPort 5 behavior:

```text
turbidity_ntu only
```

The direction of the ADC change depends on the sensor module. Enter the raw ADC values exactly as measured.

---

# 19. Detailed calibration procedure

Open the serial monitor:

```bash
pio device monitor
```

For each analog sensor:

1. Prepare the reference liquid or reference condition.
2. Hold the corresponding calibration button low.
3. Power on or reset the ESP32.
4. Keep the button pressed.
5. Watch the `[CAL]` output.
6. Wait for stable readings.
7. Record raw ADC values.
8. Release the button.
9. Enter the values into the generator.
10. Generate a new `platformio.ini`.
11. Flash again.

Example output:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
[CAL] sensor=turbidity, raw_adc=1200.00, calibrated_ntu=600.000
[CAL] sensor=ph_board_temperature, raw_adc=302.00, calibrated_temperature_c=26.000
```

Do not use unstable first readings immediately after power-on.

---

# 20. Sensor notes

## 20.1 GPS

GPS sends latitude, longitude, altitude, and HDOP on fPort 1. Indoors, GPS may fail to get a fix and the firmware sends an info message on fPort 221.

## 20.2 DS18B20

DS18B20 sends water temperature in °C on fPort 2. The OneWire data line needs a pull-up resistor, usually 4.7 kΩ.

## 20.3 PH4502C pH

pH sends only the calibrated pH value on fPort 3. Raw ADC values are only printed during calibration mode.

## 20.4 PH4502C board temperature

The board-temperature channel sends only the calibrated board/onboard temperature on fPort 6. It is diagnostic context, not water temperature.

## 20.5 Gravity TDS

TDS sends only calibrated ppm on fPort 4. Water temperature may be used internally for compensation.

## 20.6 Turbidity

Turbidity sends only calibrated NTU on fPort 5. Low-cost turbidity sensors are useful for trends and comparison, not as laboratory instruments.

---

# 21. Deep sleep and power domains

The firmware is intended for battery/solar operation.

Normal operation:

1. ESP32 wakes up.
2. One sensor slot is selected.
3. The selected sensor is read.
4. One LoRaWAN uplink is sent.
5. The radio is put to sleep.
6. ESP32 enters deep sleep.

External analog sensor boards such as PH4502C, TDS, and turbidity do not have a reliable software sleep mode. For final buoy hardware, switch their supply rails with MOSFETs or load switches.

Recommended power domains:

| Rail | Loads |
|---|---|
| `3V3_MAIN` | ESP32 and LoRa radio |
| switched `3V3_SENS` | GPS and DS18B20 |
| switched `5V_SENS` | PH4502C, TDS, turbidity |

---

# 22. Flash and test checklist

After generating and copying `platformio.ini`:

```bash
pio run
pio run -t upload
pio device monitor
```

Check serial output for:

- boot reason
- boot count
- selected sensor configuration
- LoRaWAN join or session restore
- selected current sensor
- expected fPort
- diagnostic messages

Check TTN live data for:

- uplinks arriving
- correct fPort sequence
- decoded fields
- no formatter syntax errors
- no unexpected `payload_raw` in measurement output

---

# 23. Common problems

## 23.1 Payload formatter syntax error

Reload the generator, press **Generate payload formatter**, copy the complete generated code again, and replace the formatter in TTN.

## 23.2 Wrong decoded fields

The formatter and firmware probably do not match. Regenerate both `platformio.ini` and `payload-formatter.js` from the same generator settings.

## 23.3 GPS only sends diagnostics

GPS may have no fix indoors, RX/TX may be swapped, or the GPS module may not be powered.

## 23.4 pH values unrealistic

Usually caused by missing calibration, unstable probe readings, wrong ADC pin, or changed analog scaling.

## 23.5 TDS always zero

Check sensor power, ADC pin, calibration values, and whether the chosen reference points are meaningful.

## 23.6 Turbidity inverted

Some modules produce lower ADC values for more turbidity; others may behave differently. Use the raw ADC values observed during calibration.

## 23.7 Join fails after nonce reset

TTN may reject reused DevNonces. Use dangerous nonce reset only together with TTN nonce reset or a new DevEUI.

---

# 24. Commit student work

Typical files to commit in a student branch:

```text
LoRaWAN-HelloWorld-radiolib/platformio.ini
notes/calibration-values.md
```

Do not commit real production keys to a public repository.

Example:

```bash
git status
git add LoRaWAN-HelloWorld-radiolib/platformio.ini
git commit -m "Configure LoRaWAN buoy for selected sensors"
git push -u origin my-water-buoy
```

---

# 25. Short workflow summary

```text
1. Open generator.
2. Enter TTN credentials.
3. Select radio, region, LoRaWAN version.
4. Select sensors and pins.
5. Enter raw ADC calibration values.
6. Generate platformio.ini.
7. Copy platformio.ini into LoRaWAN-HelloWorld-radiolib/.
8. Generate payload-formatter.js.
9. Install payload formatter in TTN.
10. Run pio run.
11. Run pio run -t upload.
12. Open pio device monitor.
13. Watch TTN live data.
14. Calibrate analog sensors if needed.
15. Regenerate and reflash after calibration changes.
```

---

# 26. Notes for students

- Do not share real AppKey or NwkKey values publicly.
- Use one unique DevEUI per real device.
- For LoRaWAN 1.0.x, the NwkKey field is intentionally disabled.
- For LoRaWAN 1.1.0, both AppKey and NwkKey are required.
- pH probes need buffer solutions and careful handling.
- TDS and turbidity sensors are useful for trends but are not precise chemical analysis instruments.
- PH4502C board temperature is an onboard/board-temperature value, not water temperature.
- DS18B20 is the water-temperature sensor.
- The payload formatter must match the selected sensors and firmware configuration.
- Use raw ADC values for analog calibration, not voltages.
- Regenerate both generated files after changing selected sensors.
