# Student Book — LoRaWAN Water Monitoring Buoy

This directory contains the teaching-book version of the student lego workflow for the configurable LoRaWAN water-monitoring buoy firmware.

The book is written for students in a management-oriented study program who may not have a strong electronics or embedded-systems background. It therefore explains not only the commands and workflow, but also the technical theory behind the system: sensors, analog signals, ADC values, calibration, microcontrollers, LoRaWAN, TTN, payload formatting, deep sleep, and power design.

Use this book together with the current branch:

```text
course/lego-configurable
```

The practical workflow is generator-based. The web generator creates two files that must match each other:

```text
platformio.ini
payload-formatter.js
```

## Table of contents

1. [Course context and learning goals](00-course-context.md)
2. [Technology primer for managers](00a-technology-primer-for-managers.md)
3. [System overview](01-system-overview.md)
4. [LoRaWAN and TTN theory](02-lorawan-and-ttn.md)
5. [Generator workflow](03-generator-workflow.md)
6. [Firmware architecture](04-firmware-architecture.md)
7. [Sensors and measurement theory](05-sensors-and-measurements.md)
8. [Analog calibration](06-analog-calibration.md)
9. [Power, deep sleep, and solar operation](07-power-and-deep-sleep.md)
10. [Build, flash, test, and troubleshoot](08-build-test-troubleshoot.md)
11. [Student tasks and suggested exercises](09-student-tasks.md)
12. [Installing Qt Creator, PlatformIO, and CP2102 drivers on Windows and macOS](10-installation-windows-macos.md)

## How to read this book

Students with little technical background should first read chapters 00, 00a, 01, and 02 before touching the generator. These chapters provide the mental model needed to understand what the generator actually configures.

Students who set up their own computer should also read chapter 10 before the first lab session, especially if the ESP32 board does not appear as a serial port.

Students who already know embedded development can use the quick workflow below and then return to the theory chapters when interpreting measurement results.

## Current fPort mapping

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

Important current behavior:

- The PH4502C board-temperature channel is mandatory and uses fPort 6.
- Measurement decoded payloads do not contain `payload_raw`.
- TDS sends only `tds_ppm`, not the compensation temperature.
- pH sends only `ph_level`, not raw ADC and not the board temperature.
- All analog calibration values are raw ESP32 ADC values, not voltages.

## Quick workflow

```text
1. Open tools/platformio-ini-generator/www/index.html.
2. Enter TTN credentials.
3. Select radio module, region, and LoRaWAN version.
4. Select sensors and pins.
5. Enter raw ADC calibration values.
6. Generate platformio.ini.
7. Copy platformio.ini into LoRaWAN-HelloWorld-radiolib/.
8. Generate payload-formatter.js.
9. Install payload formatter in TTN.
10. Build with pio run.
11. Upload with pio run -t upload.
12. Monitor serial output with pio device monitor.
13. Check TTN live data.
14. Calibrate analog sensors.
15. Regenerate and reflash after calibration changes.
```
