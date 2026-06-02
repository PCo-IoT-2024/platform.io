# Configurable Lego Sensor Firmware

Branch: `course/lego-configurable`

This branch is the student-friendly configurable firmware. Students do not need to edit the C++ sensor scheduler to choose a sensor combination. Instead, they select or copy a PlatformIO environment and set the `APP_HAS_*` build flags.

## Feature Flags

| Flag | Meaning |
|---|---|
| `APP_HAS_GPS` | enable GPS uplinks on fPort 1 |
| `APP_HAS_TEMPERATURE` | enable DS18B20 temperature uplinks on fPort 2 |
| `APP_HAS_PH` | enable PH4502C pH uplinks on fPort 3 |
| `APP_HAS_TDS` | enable Gravity TDS uplinks on fPort 4 |
| `APP_HAS_TURBIDITY` | enable turbidity uplinks on fPort 5 |

Each flag is either `0` or `1`.

## Prepared Environments

Build from `LoRaWAN-HelloWorld-radiolib` with:

```bash
pio run -c platformio.course-full.ini -e <environment>
```

| Environment | Enabled sensors |
|---|---|
| `course_base_sx1262` | none; diagnostic only |
| `course_full_sx1262` | GPS, temperature, pH, TDS, turbidity |
| `course_gps_temperature_sx1262` | GPS, temperature |
| `course_temp_tds_sx1262` | temperature, TDS |
| `course_gps_temp_tds_sx1262` | GPS, temperature, TDS |
| `course_ph_turbidity_sx1262` | pH, turbidity |
| `course_water_basic_sx1262` | GPS, temperature, pH, TDS |

Example:

```bash
pio run -c platformio.course-full.ini -e course_temp_tds_sx1262
```

Upload:

```bash
pio run -c platformio.course-full.ini -e course_temp_tds_sx1262 -t upload
```

## Creating a Custom Combination

Copy one prepared environment in `platformio.course-full.ini` and change the feature flags.

Example: GPS + pH + turbidity:

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

Build with:

```bash
pio run -c platformio.course-full.ini -e course_my_group_sx1262
```

## Scheduling Behavior

The firmware creates a compile-time sensor table from the enabled sensors. On each wake cycle it sends exactly one sensor reading and rotates through the enabled sensors using `bootCount`.

For example, with temperature + TDS enabled:

```text
wake 1 -> temperature -> fPort 2
wake 2 -> TDS         -> fPort 4
wake 3 -> temperature -> fPort 2
wake 4 -> TDS         -> fPort 4
```

## TDS Temperature Compensation

If `APP_HAS_TEMPERATURE=1`, the firmware stores the last valid DS18B20 temperature in RTC memory and uses it for TDS compensation. If no valid temperature is available, it falls back to:

```ini
-D TDS_DEFAULT_TEMPERATURE_C=22.0f
```

## Confirmed Pins

| Function | GPIO |
|---|---:|
| GPS RX | 16 |
| GPS TX | 17 |
| DS18B20 temperature | 27 |
| pH analog | 34 |
| pH temperature | 35 |
| Gravity TDS | 32 |
| Turbidity | 33 |
| LoRa module bitmap | `5, 2, 14, 4` |

## Recommended Student Workflow

Students should fork the repository, create a branch from `course/lego-configurable`, and select one prepared environment or create their own.

```bash
git clone git@github.com:<student>/platform.io.git
cd platform.io
git remote add upstream git@github.com:PCo-IoT-2024/platform.io.git
git fetch upstream
git switch -c my-water-buoy upstream/course/lego-configurable
cd LoRaWAN-HelloWorld-radiolib
pio run -c platformio.course-full.ini -e course_temp_tds_sx1262
```
