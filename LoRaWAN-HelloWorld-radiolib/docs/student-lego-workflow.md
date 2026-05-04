# Student Guide — LoRaWAN Water Monitoring Buoy Lego Firmware

This guide explains how to work with the configurable “lego” firmware branch of the LoRaWAN water monitoring buoy project.

The goal is that each student group can build its own specific buoy firmware by selecting only the sensors they actually use.

The configurable branch is:

```text
course/lego-configurable
```

This branch is based on the validated full firmware branch:

```text
course/full
```

The important difference is:

- `course/full` contains the complete reference solution with all sensors enabled.
- `course/lego-configurable` lets students enable or disable sensors using PlatformIO build flags.

---

# 1. Repository Structure

The repository contains several branches for different teaching purposes.

## 1.1 Important Branches

```text
course/base-lorawan
course/full
course/lego-configurable
course/temperature-ds18b20
course/module-ph-ph4502c
course/mod03
course/mod04
```

## 1.2 Branch Purpose

| Branch | Purpose |
|---|---|
| `course/base-lorawan` | Minimal LoRaWAN base branch |
| `course/full` | Validated complete firmware with all sensors |
| `course/lego-configurable` | Student-friendly configurable firmware |
| `course/temperature-ds18b20` | Temperature module branch |
| `course/module-ph-ph4502c` | pH module branch |
| `course/mod03` | TDS module branch |
| `course/mod04` | Turbidity module branch |

For student work, use:

```text
course/lego-configurable
```

Do not start from `main` or `radiolib-master` unless explicitly instructed.

---

# 2. Project Directory Structure

The PlatformIO project is located in:

```text
LoRaWAN-HelloWorld-radiolib/
```

Important files and directories:

```text
LoRaWAN-HelloWorld-radiolib/
├── platformio.ini
├── platformio.course-full.ini
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
├── modules/
│   ├── temperature-ds18b20/
│   ├── ph-ph4502c/
│   ├── tds-gravity/
│   └── turbidity-analog/
├── docs/
│   ├── course-full.md
│   └── lego-configurable.md
└── ttn/
    └── payload-formatter.js
```

## 2.1 `src/`

This is the actual firmware source code used by PlatformIO.

The main application logic is in:

```text
src/LoRaWANTemplate.cpp
```

The sensor classes are in:

```text
src/sensors/
```

## 2.2 `modules/`

The `modules/` directory contains self-contained teaching modules for each sensor.

These are useful for studying each sensor separately.

Example:

```text
modules/temperature-ds18b20/
├── DS18B20.cpp
├── DS18B20.h
├── README.md
└── platformio.fragment.ini
```

The module folders explain the sensor wiring, required libraries, and how the sensor is integrated.

## 2.3 `platformio.course-full.ini`

This is the main PlatformIO configuration for the course branches.

Use this file instead of the original `platformio.ini` when working with the lego system.

Build example:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262
```

Upload example:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262 -t upload
```

## 2.4 `ttn/payload-formatter.js`

This is the TTN uplink payload formatter.

It decodes the ASCII CSV payloads sent by the device.

The fPort mapping is:

| fPort | Meaning |
|---:|---|
| 1 | GPS |
| 2 | Temperature |
| 3 | pH |
| 4 | TDS |
| 5 | Turbidity |
| 221 | Info / diagnostics |
| 222 | Warning |
| 223 | Error |

---

# 3. Getting Started with the Lego System

## 3.1 Fork the Repository

Each student group should fork the repository on GitHub.

Original repository:

```text
PCo-IoT-2024/platform.io
```

After forking, each group has its own copy, for example:

```text
student-name/platform.io
```

This is important because every group needs its own configuration, commits, and possibly its own TTN credentials.

## 3.2 Clone Your Fork

```bash
git clone git@github.com:<your-github-user>/platform.io.git
cd platform.io
```

Example:

```bash
git clone git@github.com:alice/platform.io.git
cd platform.io
```

## 3.3 Add the Course Repository as Upstream

```bash
git remote add upstream git@github.com:PCo-IoT-2024/platform.io.git
git fetch upstream
```

Check remotes:

```bash
git remote -v
```

Expected:

```text
origin    git@github.com:<your-github-user>/platform.io.git
upstream  git@github.com:PCo-IoT-2024/platform.io.git
```

## 3.4 Create Your Own Working Branch

Start from the lego branch:

```bash
git switch -c my-water-buoy upstream/course/lego-configurable
```

You are now working on your own branch.

Do not commit directly to `course/lego-configurable`.

---

# 4. Building the Firmware

Enter the PlatformIO project directory:

```bash
cd LoRaWAN-HelloWorld-radiolib
```

Build the full firmware:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262
```

If this succeeds, the toolchain is working.

---

# 5. Uploading the Firmware

Upload to the ESP32:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262 -t upload
```

Open the serial monitor:

```bash
pio device monitor
```

Expected serial output includes something like:

```text
[APP] Sensor configuration:
[APP]   GPS: 1
[APP]   temperature: 1
[APP]   pH: 1
[APP]   TDS: 1
[APP]   turbidity: 1
```

The firmware wakes up, measures one sensor, sends one uplink, and goes back to sleep.

---

# 6. The Lego Configuration System

The lego system is controlled by compile-time build flags.

The important flags are:

```ini
-D APP_HAS_GPS=1
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=1
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=1
```

Each flag can be `1` or `0`.

| Value | Meaning |
|---:|---|
| `1` | sensor enabled |
| `0` | sensor disabled |

Example:

```ini
-D APP_HAS_GPS=0
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=0
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=0
```

This configuration enables only:

```text
temperature + TDS
```

---

# 7. Prepared Sensor Combinations

The file:

```text
platformio.course-full.ini
```

already contains prepared PlatformIO environments.

## 7.1 Full Firmware

All sensors enabled:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262
```

Enabled sensors:

```text
GPS
temperature
pH
TDS
turbidity
```

Expected fPorts:

```text
1, 2, 3, 4, 5
```

## 7.2 Base Firmware without Sensors

```bash
pio run -c platformio.course-full.ini -e course_base_sx1262
```

Enabled sensors:

```text
none
```

Expected fPort:

```text
221
```

Expected diagnostic message:

```text
RadioLib experiment device: No sensor enabled
```

## 7.3 GPS + Temperature

```bash
pio run -c platformio.course-full.ini -e course_gps_temperature_sx1262
```

Enabled sensors:

```text
GPS
temperature
```

Expected fPorts:

```text
1, 2
```

## 7.4 Temperature + TDS

```bash
pio run -c platformio.course-full.ini -e course_temp_tds_sx1262
```

Enabled sensors:

```text
temperature
TDS
```

Expected fPorts:

```text
2, 4
```

This is a meaningful combination because TDS can use the last valid water temperature for compensation.

## 7.5 GPS + Temperature + TDS

```bash
pio run -c platformio.course-full.ini -e course_gps_temp_tds_sx1262
```

Enabled sensors:

```text
GPS
temperature
TDS
```

Expected fPorts:

```text
1, 2, 4
```

## 7.6 pH + Turbidity

```bash
pio run -c platformio.course-full.ini -e course_ph_turbidity_sx1262
```

Enabled sensors:

```text
pH
turbidity
```

Expected fPorts:

```text
3, 5
```

## 7.7 Basic Water Quality without Turbidity

```bash
pio run -c platformio.course-full.ini -e course_water_basic_sx1262
```

Enabled sensors:

```text
GPS
temperature
pH
TDS
```

Expected fPorts:

```text
1, 2, 3, 4
```

---

# 8. Creating Your Own Sensor Combination

To create your own sensor combination, edit:

```text
platformio.course-full.ini
```

Do not edit the C++ scheduler unless instructed.

## 8.1 Example: GPS + pH + Turbidity

Add this to `platformio.course-full.ini`:

```ini
[features_my_group]
build_flags =
    -D APP_HAS_GPS=1
    -D APP_HAS_TEMPERATURE=0
    -D APP_HAS_PH=1
    -D APP_HAS_TDS=0
    -D APP_HAS_TURBIDITY=1

[env:course_my_group_sx1262]
build_flags =
    ${env.build_flags}
    ${course_sx1262.build_flags}
    ${features_my_group.build_flags}
```

Build:

```bash
pio run -c platformio.course-full.ini -e course_my_group_sx1262
```

Upload:

```bash
pio run -c platformio.course-full.ini -e course_my_group_sx1262 -t upload
```

Monitor:

```bash
pio device monitor
```

Expected serial output:

```text
[APP]   GPS: 1
[APP]   temperature: 0
[APP]   pH: 1
[APP]   TDS: 0
[APP]   turbidity: 1
```

Expected sensor rotation:

```text
GPS -> pH -> turbidity -> GPS -> pH -> turbidity
```

Expected fPorts:

```text
1, 3, 5
```

---

# 9. How the Sensor Scheduler Works

The firmware builds a compile-time sensor table.

Only enabled sensors are placed into this table.

Example:

```ini
-D APP_HAS_GPS=0
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=0
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=0
```

This creates a sensor table equivalent to:

```text
temperature
TDS
```

The firmware uses the RTC boot counter:

```cpp
bootCount
```

to rotate through the enabled sensors.

For temperature + TDS:

```text
wake 1 -> temperature
wake 2 -> TDS
wake 3 -> temperature
wake 4 -> TDS
```

This keeps each wake cycle short.

The device does not measure every sensor on every wake-up.

---

# 10. Confirmed Pin Mapping

The validated course wiring is:

| Function | ESP32 GPIO |
|---|---:|
| GPS RX | 16 |
| GPS TX | 17 |
| DS18B20 temperature | 27 |
| pH analog output | 34 |
| pH temperature output | 35 |
| Gravity TDS analog output | 32 |
| Turbidity analog output | 33 |
| LoRa module bitmap | `5, 2, 14, 4` |

Radio module bitmap:

```ini
-D RADIOLIB_LORA_MODULE_BITMAP="5, 2, 14, 4"
```

For SX1262 this means:

```text
NSS / CS
DIO1
RESET
BUSY
```

with the configured pin order used by the firmware.

---

# 11. Sensor Descriptions

## 11.1 GPS Module

### Purpose

The GPS module provides position data.

It is used to locate the buoy.

### Data Sent

fPort:

```text
1
```

Payload format:

```text
latitude,longitude,altitude,hdop
```

Example:

```text
48.368720,14.513520,273.000000,1.250000
```

### Wiring

| GPS | ESP32 |
|---|---:|
| TX | GPIO 16 |
| RX | GPIO 17 |
| VCC | according to GPS module |
| GND | GND |

The ESP32 receives GPS data on:

```ini
-D GPS_SERIAL_RX_PIN=16
```

The ESP32 transmits to GPS on:

```ini
-D GPS_SERIAL_TX_PIN=17
```

### Code

GPS is represented by:

```cpp
position::GPS gps(
    GPS_SERIAL_PORT,
    GPS_SERIAL_BAUD_RATE,
    GPS_SERIAL_CONFIG,
    GPS_SERIAL_RX_PIN,
    GPS_SERIAL_TX_PIN
);
```

The payload is built from:

```cpp
gps.getLatitude()
gps.getLongitude()
gps.getAltitude()
gps.getHdop()
```

### Important Notes

GPS may need time to get a fix.

If no valid GPS data is available, the firmware sends a diagnostic message on fPort `221`.

Typical message:

```text
RadioLib experiment device: Waiting for GPS
```

---

## 11.2 DS18B20 Temperature Sensor

### Purpose

The DS18B20 measures water temperature.

Temperature is important by itself and also useful for compensating other measurements such as TDS.

### Data Sent

fPort:

```text
2
```

Payload format:

```text
temperatureC
```

Example:

```text
21.875000
```

### Wiring

| DS18B20 | ESP32 |
|---|---:|
| DATA | GPIO 27 |
| VCC | 3.3 V |
| GND | GND |

A pull-up resistor is required:

```text
4.7 kΩ between DATA and 3.3 V
```

### PlatformIO Flags

```ini
-D DALLAS_TEMPERATURE_PIN=27
```

### Required Libraries

```ini
PaulStoffregen/OneWire
milesburton/DallasTemperature
```

### Code

The sensor is represented by:

```cpp
temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
```

Typical use:

```cpp
temp.setup();

if (temp.isValid()) {
    float temperatureC = temp.getTemperature();
}
```

### Usage in Lego Branch

If enabled:

```ini
-D APP_HAS_TEMPERATURE=1
```

the temperature sensor is inserted into the scheduler.

If TDS is also enabled, the last valid DS18B20 temperature is stored in RTC memory and used for TDS compensation.

### Common Problems

| Problem | Cause |
|---|---|
| invalid temperature | missing pull-up resistor |
| `DEVICE_DISCONNECTED_C` | wrong GPIO, bad wiring, sensor not powered |
| unstable readings | long cable, weak pull-up, bad waterproofing |

---

## 11.3 PH4502C pH Sensor

### Purpose

The PH4502C measures the pH value of water.

pH tells whether the water is acidic, neutral, or alkaline.

### Data Sent

fPort:

```text
3
```

Payload format:

```text
ph
```

Example:

```text
7.120000
```

### Wiring

| PH4502C | ESP32 |
|---|---:|
| pH analog output | GPIO 34 |
| temperature analog output | GPIO 35 |
| VCC | according to module |
| GND | GND |

### PlatformIO Flags

```ini
-D PH4502C_PH_PIN=34
-D PH4502C_TEMPERATURE_PIN=35
```

### Calibration Points

The firmware uses three calibration ADC values:

```ini
-D PH10_ADC_VALUE=1615
-D PH7_ADC_VALUE=2080
-D PH4_ADC_VALUE=2503
```

These represent approximate ADC readings for pH buffer solutions:

```text
pH 10
pH 7
pH 4
```

### Code

The sensor is represented by:

```cpp
ph::PH4502C pH(
    PH4502C_PH_PIN,
    PH4502C_TEMPERATURE_PIN,
    {{PH10_ADC_VALUE, 10}, {PH7_ADC_VALUE, 7}, {PH4_ADC_VALUE, 4}}
);
```

Typical use:

```cpp
pH.setup();
float phValue = pH.getPHLevel();
```

### Usage in Lego Branch

Enable with:

```ini
-D APP_HAS_PH=1
```

Disable with:

```ini
-D APP_HAS_PH=0
```

### Electronics Notes

The PH4502C is an analog sensor board.

Important:

- ESP32 ADC pins must not receive more than the allowed voltage.
- Check the module output voltage before connecting it.
- Calibration is essential.
- pH probes need proper storage and calibration solution.

### Common Problems

| Problem | Cause |
|---|---|
| unrealistic pH | not calibrated |
| noisy value | unstable analog signal |
| wrong value | wrong ADC pin |
| stuck value | probe not connected or board not powered |

---

## 11.4 Gravity TDS Sensor

### Purpose

TDS means:

```text
Total Dissolved Solids
```

It estimates the amount of dissolved solids in water.

The result is usually given in ppm.

### Data Sent

fPort:

```text
4
```

Payload format:

```text
tds_ppm,temperatureC
```

Example:

```text
350.000000,22.000000
```

### Wiring

| TDS board | ESP32 |
|---|---:|
| analog output | GPIO 32 |
| VCC | according to module |
| GND | GND |

### PlatformIO Flags

```ini
-D TDS_SENSOR_PIN=32
-D A1=TDS_SENSOR_PIN
-D TDS_SENSOR_VCC=3.3
-D TDS_SENSOR_ADC_RESOLUTION=4096
-D TDS_DEFAULT_TEMPERATURE_C=22.0f
```

### Why `A1` Is Defined

The Gravity TDS library internally refers to Arduino-style `A1`.

On ESP32, `A1` is not defined by default.

Therefore the firmware maps:

```ini
-D A1=TDS_SENSOR_PIN
```

This keeps `TDS_SENSOR_PIN` as the single source of truth.

### Required Libraries

```ini
https://github.com/PCo-IoT-2024/GravityTDS.git
eeprom
```

### Code

The sensor is represented by:

```cpp
tds::TdS tdsSensor(
    TDS_SENSOR_PIN,
    TDS_SENSOR_VCC,
    TDS_SENSOR_ADC_RESOLUTION
);
```

Typical use:

```cpp
tdsSensor.setup();
float tdsValue = tdsSensor.getValue(temperatureC);
```

### Temperature Compensation

TDS depends on temperature.

In the lego branch:

- if the DS18B20 sensor is enabled and has produced a valid value, TDS uses that value.
- otherwise TDS uses the fallback temperature:

```ini
-D TDS_DEFAULT_TEMPERATURE_C=22.0f
```

### Usage in Lego Branch

Enable with:

```ini
-D APP_HAS_TDS=1
```

Disable with:

```ini
-D APP_HAS_TDS=0
```

Recommended combination:

```ini
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_TDS=1
```

### Common Problems

| Problem | Cause |
|---|---|
| compile error about `A1` | missing `-D A1=TDS_SENSOR_PIN` |
| wrong TDS value | wrong ADC range or reference voltage |
| unstable value | bad analog wiring |
| unrealistic ppm | no calibration or wrong temperature compensation |

---

## 11.5 Turbidity Sensor

### Purpose

The turbidity sensor estimates how cloudy the water is.

Turbidity is related to suspended particles in water.

### Data Sent

fPort:

```text
5
```

Payload format:

```text
ntu
```

Example:

```text
120.500000
```

### Wiring

| Turbidity board | ESP32 |
|---|---:|
| analog output | GPIO 33 |
| VCC | according to module |
| GND | GND |

### PlatformIO Flags

```ini
-D TURBIDITY_PIN=33
-D TURBIDITY_VCC=3.3
-D TURBIDITY_ADC_MAX=4096
-D TURBIDITY_CLEAR_WATER_VOLTAGE=1.780
-D TURBIDITY_CLEAR_WATER_NTU=0.0
-D TURBIDITY_TURBID_WATER_VOLTAGE=0.840
-D TURBIDITY_TURBID_WATER_NTU=600.0
```

### Code

The sensor is represented by:

```cpp
turbidity::TurbiditySensor turbiditySensor(
    TURBIDITY_PIN,
    TURBIDITY_VCC,
    TURBIDITY_ADC_MAX,
    TURBIDITY_CLEAR_WATER_VOLTAGE,
    TURBIDITY_CLEAR_WATER_NTU,
    TURBIDITY_TURBID_WATER_VOLTAGE,
    TURBIDITY_TURBID_WATER_NTU
);
```

Typical use:

```cpp
turbiditySensor.setup();
float ntu = turbiditySensor.getNTU();
```

### Calibration

The current implementation uses a simple two-point linear calibration.

The default points are:

```ini
-D TURBIDITY_CLEAR_WATER_VOLTAGE=1.780
-D TURBIDITY_CLEAR_WATER_NTU=0.0
-D TURBIDITY_TURBID_WATER_VOLTAGE=0.840
-D TURBIDITY_TURBID_WATER_NTU=600.0
```

These values should be checked and adjusted experimentally.

### Usage in Lego Branch

Enable with:

```ini
-D APP_HAS_TURBIDITY=1
```

Disable with:

```ini
-D APP_HAS_TURBIDITY=0
```

### Common Problems

| Problem | Cause |
|---|---|
| wrong NTU value | calibration not adapted |
| unstable value | analog noise |
| constant zero | wrong voltage mapping |
| ADC saturation | output voltage too high |

---

# 12. Adding Individual Sensors

In the lego system, adding a sensor means enabling its `APP_HAS_*` flag and choosing or creating a PlatformIO environment.

## 12.1 Add GPS

Set:

```ini
-D APP_HAS_GPS=1
```

Make sure GPS pins are configured:

```ini
-D GPS_SERIAL_RX_PIN=16
-D GPS_SERIAL_TX_PIN=17
```

Expected fPort:

```text
1
```

## 12.2 Add Temperature

Set:

```ini
-D APP_HAS_TEMPERATURE=1
```

Make sure the pin is configured:

```ini
-D DALLAS_TEMPERATURE_PIN=27
```

Expected fPort:

```text
2
```

## 12.3 Add pH

Set:

```ini
-D APP_HAS_PH=1
```

Make sure the pins are configured:

```ini
-D PH4502C_PH_PIN=34
-D PH4502C_TEMPERATURE_PIN=35
```

Expected fPort:

```text
3
```

## 12.4 Add TDS

Set:

```ini
-D APP_HAS_TDS=1
```

Make sure the pin and `A1` alias are configured:

```ini
-D TDS_SENSOR_PIN=32
-D A1=TDS_SENSOR_PIN
```

Expected fPort:

```text
4
```

Recommended:

```ini
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_TDS=1
```

so TDS can use the last measured water temperature.

## 12.5 Add Turbidity

Set:

```ini
-D APP_HAS_TURBIDITY=1
```

Make sure the pin is configured:

```ini
-D TURBIDITY_PIN=33
```

Expected fPort:

```text
5
```

---

# 13. Step-by-Step: Extend Firmware with Sensors Already in the Repository

This section describes how to start with a minimal sensor set and extend it step by step.

## 13.1 Step 0 — Start from the Lego Branch

```bash
git fetch upstream
git switch -c my-water-buoy upstream/course/lego-configurable
cd LoRaWAN-HelloWorld-radiolib
```

Build the base firmware:

```bash
pio run -c platformio.course-full.ini -e course_base_sx1262
```

Upload:

```bash
pio run -c platformio.course-full.ini -e course_base_sx1262 -t upload
```

Expected result:

```text
No sensor enabled
fPort 221
```

## 13.2 Step 1 — Add GPS

Build GPS + temperature environment if you also want temperature:

```bash
pio run -c platformio.course-full.ini -e course_gps_temperature_sx1262
```

Or create your own GPS-only environment:

```ini
[features_gps_only]
build_flags =
    -D APP_HAS_GPS=1
    -D APP_HAS_TEMPERATURE=0
    -D APP_HAS_PH=0
    -D APP_HAS_TDS=0
    -D APP_HAS_TURBIDITY=0

[env:course_gps_only_sx1262]
build_flags =
    ${env.build_flags}
    ${course_sx1262.build_flags}
    ${features_gps_only.build_flags}
```

Build:

```bash
pio run -c platformio.course-full.ini -e course_gps_only_sx1262
```

Expected fPort:

```text
1
```

If GPS has no fix:

```text
fPort 221
Waiting for GPS
```

## 13.3 Step 2 — Add Temperature

Use:

```bash
pio run -c platformio.course-full.ini -e course_gps_temperature_sx1262
```

Expected fPorts:

```text
1, 2
```

Check serial output:

```text
Current sensor name: GPS
Current sensor name: temperature
```

## 13.4 Step 3 — Add TDS

Use:

```bash
pio run -c platformio.course-full.ini -e course_gps_temp_tds_sx1262
```

Expected fPorts:

```text
1, 2, 4
```

TDS uses the last valid DS18B20 temperature if available.

If no temperature has been measured yet, TDS uses:

```text
22.0 °C
```

from:

```ini
-D TDS_DEFAULT_TEMPERATURE_C=22.0f
```

## 13.5 Step 4 — Add pH

Use the basic water quality environment:

```bash
pio run -c platformio.course-full.ini -e course_water_basic_sx1262
```

Expected fPorts:

```text
1, 2, 3, 4
```

Sensor order:

```text
GPS -> temperature -> pH -> TDS
```

## 13.6 Step 5 — Add Turbidity

Use the full environment:

```bash
pio run -c platformio.course-full.ini -e course_full_sx1262
```

Expected fPorts:

```text
1, 2, 3, 4, 5
```

Sensor order:

```text
GPS -> temperature -> pH -> TDS -> turbidity
```

---

# 14. TTN Payload Formatter

Use:

```text
ttn/payload-formatter.js
```

in the TTN console.

## 14.1 Install in TTN

In The Things Stack / TTN:

```text
Application
→ Payload formatters
→ Uplink
→ Custom Javascript formatter
```

Paste the contents of:

```text
LoRaWAN-HelloWorld-radiolib/ttn/payload-formatter.js
```

Save the formatter.

## 14.2 Decoded Fields

| fPort | Decoded Fields |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm`, `temperature_c` |
| 5 | `turbidity_ntu` |
| 221 | `message` |
| 222 | warning message |
| 223 | error message |

---

# 15. Checking TTN Uplinks

After uploading, check the TTN live data view.

Expected fPorts depend on your selected environment.

For `course_temp_tds_sx1262`:

```text
fPort 2
fPort 4
fPort 2
fPort 4
```

For `course_ph_turbidity_sx1262`:

```text
fPort 3
fPort 5
fPort 3
fPort 5
```

For `course_full_sx1262`:

```text
fPort 1
fPort 2
fPort 3
fPort 4
fPort 5
```

---

# 16. Recommended Student Git Workflow

After changing configuration:

```bash
git status
git add platformio.course-full.ini
git commit -m "Configure sensor combination for my buoy"
git push -u origin my-water-buoy
```

If submitting work back to the course repository, open a pull request from:

```text
<student>/platform.io:my-water-buoy
```

to the course repository branch requested by the lecturer.

---

# 17. Common Problems

## 17.1 Build Fails: `A1` Not Declared

Cause:

The Gravity TDS library expects `A1`.

Fix:

Make sure this exists in the build flags:

```ini
-D TDS_SENSOR_PIN=32
-D A1=TDS_SENSOR_PIN
```

## 17.2 No GPS Fix

This is normal indoors or shortly after boot.

Expected diagnostic:

```text
fPort 221
Waiting for GPS
```

Move the device near a window or outside.

## 17.3 Temperature Sensor Invalid

Check:

```text
GPIO 27
4.7 kΩ pull-up resistor
3.3 V
GND
waterproof DS18B20 wiring
```

## 17.4 pH Values Unrealistic

Check:

```text
calibration values
buffer solutions
ADC pins 34 and 35
probe condition
module voltage
```

## 17.5 TDS Values Unrealistic

Check:

```text
ADC pin 32
TDS_SENSOR_VCC
TDS_SENSOR_ADC_RESOLUTION
temperature compensation
calibration
```

## 17.6 Turbidity Always Zero or Too High

Check:

```text
ADC pin 33
sensor voltage
clear/turbid calibration points
ADC range
```

## 17.7 Wrong fPorts in TTN

Check the enabled flags in the selected PlatformIO environment.

Example:

```ini
-D APP_HAS_GPS=0
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=0
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=0
```

This should produce only:

```text
fPort 2
fPort 4
```

---

# 18. Summary

For most student groups:

1. Fork the repository.
2. Start from `course/lego-configurable`.
3. Choose or create a PlatformIO environment.
4. Enable only the required sensors using `APP_HAS_*` flags.
5. Build.
6. Upload.
7. Monitor serial output.
8. Check TTN fPorts and decoded payloads.
9. Commit and push the group-specific configuration.

The most important file for student configuration is:

```text
LoRaWAN-HelloWorld-radiolib/platformio.course-full.ini
```

The most important firmware file is:

```text
LoRaWAN-HelloWorld-radiolib/src/LoRaWANTemplate.cpp
```

But in the normal lego workflow, students should usually edit only:

```text
platformio.course-full.ini
```

and not the C++ scheduler.
