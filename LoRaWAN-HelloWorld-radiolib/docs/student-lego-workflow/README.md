# Student Book — LoRaWAN Water Monitoring Buoy

This directory contains the teaching-book version of the student lego workflow for the configurable LoRaWAN water-monitoring buoy firmware and the local Raspberry Pi backend.

The book is written for students in a management-oriented study program who may not have a strong electronics or embedded-systems background. It follows the workshop flow: orientation, group organization, installation, device setup, TTN cloud setup, local Raspberry Pi backend, dashboard, real water measurement, and final reflection.

Use this book together with the current branch:

```text
course/lego-configurable
```

The practical firmware workflow is generator-based. The web generator creates two files that must match each other:

```text
platformio.ini
payload-formatter.js
```

The practical backend workflow uses one Raspberry Pi per group:

```text
user: water
hostnames: group1, group2, group3, group4
local MQTT port: 1883
dashboard port: 8080
```

The MariaDB backend uses two tables:

```text
measurements     -> scalar sensor values, identified by f_port
gps_positions   -> GPS latitude/longitude/altitude/HDOP records
```

## Table of contents

1. [Course context and learning goals](00-course-context.md)
2. [Technology primer for managers](01-technology-primer-for-managers.md)
3. [End-to-end buoy system architecture](02-end-to-end-architecture.md)
4. [Group work organization](03-group-work-organization.md)
5. [Installing Qt Creator, PlatformIO, and CP2102 drivers on Windows and macOS](04-installation-windows-macos.md)
6. [System overview](05-system-overview.md)
7. [LoRaWAN and TTN theory](06-lorawan-and-ttn.md)
8. [TTN cloud setup](07-ttn-cloud-setup.md)
9. [Generator workflow](08-generator-workflow.md)
10. [Firmware architecture](09-firmware-architecture.md)
11. [Sensors and measurement theory](10-sensors-and-measurements.md)
12. [Analog calibration](11-analog-calibration.md)
13. [Build, flash, test, and troubleshoot](12-build-test-troubleshoot.md)
14. [Raspberry Pi setup](13-raspberry-pi-setup.md)
15. [MQTT theory for the buoy backend](14-mqtt-theory.md)
16. [Building SNode.C and MQTTSuite on the Raspberry Pi](15-building-snodec-and-mqttsuite.md)
17. [Running the local MQTT broker and TTN bridge](16-local-mqttbroker-and-bridge.md)
18. [Storing MQTT data in MariaDB with mqttcli](17-mariadb-storage-with-mqttcli.md)
19. [Dashboard with mqttcli and SNode.C](18-dashboard-with-snodec.md)
20. [Power, deep sleep, and solar operation](19-power-and-deep-sleep.md)
21. [Running processes and final end-to-end test](20-running-services-and-final-test.md)
22. [Real water measurement and field test](21-real-water-measurement-and-field-test.md)
23. [Final presentation and conclusion](22-final-presentation-and-conclusion.md)
24. [Student tasks and deliverables](23-student-tasks.md)

## How to read this book

Students with little technical background should first read the orientation chapters before touching tools. The group-work chapter should be read before the first hands-on workshop session, because it defines the five individual workstreams.

The practical build work starts after installation. The device-side chapters lead to a working ESP32-to-TTN path. The backend chapters then extend the system from TTN into the Raspberry Pi, MariaDB, and the mqttcli/SNode.C dashboard. The final chapters prepare the field measurement at the lake and the conclusion.

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
- [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/)
- [MariaDB documentation](https://mariadb.com/kb/en/documentation/)
- [SNode.C repository](https://github.com/SNodeC/snode.c)
- [MQTTSuite repository](https://github.com/SNodeC/mqttsuite)

## Current fPort mapping

| fPort | Measurement | Decoded TTN field | MariaDB table |
|---:|---|---|---|
| 1 | GPS position | `latitude`, `longitude`, `altitude`, `hdop` | `gps_positions` |
| 2 | DS18B20 water temperature | `temperature_c` | `measurements` |
| 3 | PH4502C pH | `ph_level` | `measurements` |
| 4 | Gravity TDS | `tds_ppm` | `measurements` |
| 5 | Turbidity | `turbidity_ntu` | `measurements` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` | `measurements` |

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
- MariaDB stores scalar values in `measurements` using `f_port` plus `value`.
- MariaDB stores GPS values in `gps_positions` using latitude, longitude, altitude, and HDOP columns.

## Quick workflow

```text
1. Understand the mission and end-to-end architecture.
2. Split the group into five workstreams.
3. Install Qt Creator, PlatformIO, Git/Python, and USB bridge drivers.
4. Set up TTN application, device, payload formatter, and MQTT access.
5. Generate platformio.ini and payload-formatter.js.
6. Build and flash the ESP32 firmware.
7. Calibrate sensors and verify TTN decoded uplinks.
8. Prepare the Raspberry Pi backend as water@groupN.local.
9. Build SNode.C master and MQTTSuite mqttcli-mariadb.
10. Run mqttbroker, mqttbridge, mqttcli storage/dashboard, and MariaDB.
11. Store scalar measurements in measurements and GPS positions in gps_positions.
12. Serve the dashboard with mqttcli/SNode.C on port 8080.
13. Run an end-to-end test.
14. Measure real water at the lake.
15. Present results, limitations, and improvements.
```
