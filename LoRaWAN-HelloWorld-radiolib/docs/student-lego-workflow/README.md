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
3. [Installing Qt Creator, PlatformIO, and CP2102 drivers on Windows and macOS](01-installation-windows-macos.md)
4. [System overview](01-system-overview.md)
5. [LoRaWAN and TTN theory](02-lorawan-and-ttn.md)
6. [Generator workflow](03-generator-workflow.md)
7. [Firmware architecture](04-firmware-architecture.md)
8. [Sensors and measurement theory](05-sensors-and-measurements.md)
9. [Analog calibration](06-analog-calibration.md)
10. [Power, deep sleep, and solar operation](07-power-and-deep-sleep.md)
11. [Build, flash, test, and troubleshoot](08-build-test-troubleshoot.md)
12. [Student tasks and suggested exercises](09-student-tasks.md)

## How to read this book

Students with little technical background should first read chapters 00 and 00a for the basic mental model. Then they should read the installation chapter before the first hands-on workshop session.

After the installation is working, the system overview and LoRaWAN/TTN theory chapters provide the conceptual background for the generator and firmware workflow.

Students who already know embedded development can use the quick workflow below and then return to the theory chapters when interpreting measurement results.

## Important external resources

- [Git downloads](https://git-scm.com/downloads)
- [Python downloads](https://www.python.org/downloads/)
- [PlatformIO Core installation](https://docs.platformio.org/en/latest/core/installation/index.html)
- [Qt Creator installation documentation](https://doc.qt.io/qtcreator/creator-how-to-install.html)
- [Qt downloads](https://www.qt.io/download/)
- [Silicon Labs CP210x USB to UART Bridge VCP drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers)
- [Homebrew](https://brew.sh/)
- [The Things Network / The Things Stack documentation](https://www.thethingsindustries.com/docs/)
- [PlatformIO documentation](https://docs.platformio.org/)
- [RadioLib documentation](https://jgromes.github.io/RadioLib/)

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
1. Install Qt Creator, PlatformIO, Git/Python, and USB bridge drivers.
2. Open tools/platformio-ini-generator/www/index.html.
3. Enter TTN credentials.
4. Select radio module, region, and LoRaWAN version.
5. Select sensors and pins.
6. Enter raw ADC calibration values.
7. Generate platformio.ini.
8. Copy platformio.ini into LoRaWAN-HelloWorld-radiolib/.
9. Generate payload-formatter.js.
10. Install payload formatter in TTN.
11. Build with pio run.
12. Upload with pio run -t upload.
13. Monitor serial output with pio device monitor.
14. Check TTN live data.
15. Calibrate analog sensors.
16. Regenerate and reflash after calibration changes.
```
