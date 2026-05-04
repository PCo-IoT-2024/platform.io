# Student Lego Workflow — Teaching Book Entry Point

The student lego workflow has been split into a small teaching book with multiple focused chapters.

Start here:

```text
LoRaWAN-HelloWorld-radiolib/docs/student-lego-workflow/README.md
```

## Book chapters

| Chapter | File |
|---|---|
| Course context and learning goals | `student-lego-workflow/00-course-context.md` |
| System overview | `student-lego-workflow/01-system-overview.md` |
| LoRaWAN and TTN theory | `student-lego-workflow/02-lorawan-and-ttn.md` |
| Generator workflow | `student-lego-workflow/03-generator-workflow.md` |
| Firmware architecture | `student-lego-workflow/04-firmware-architecture.md` |
| Sensors and measurement theory | `student-lego-workflow/05-sensors-and-measurements.md` |
| Analog calibration | `student-lego-workflow/06-analog-calibration.md` |
| Power, deep sleep, and solar operation | `student-lego-workflow/07-power-and-deep-sleep.md` |
| Build, flash, test, and troubleshoot | `student-lego-workflow/08-build-test-troubleshoot.md` |
| Student tasks and suggested exercises | `student-lego-workflow/09-student-tasks.md` |

## Quick project state

The current workflow is generator-based. Students use the web generator to create:

```text
platformio.ini
payload-formatter.js
```

The generator is located at:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

Current measurement fPorts:

| fPort | Measurement | Decoded TTN field |
|---:|---|---|
| 1 | GPS position | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | DS18B20 water temperature | `temperature_c` |
| 3 | PH4502C pH | `ph_level` |
| 4 | Gravity TDS | `tds_ppm` |
| 5 | Turbidity | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

Important current behavior:

- PH4502C board temperature is mandatory and uses fPort 6.
- Measurement decoded payloads do not contain `payload_raw`.
- TDS sends only `tds_ppm`.
- pH sends only `ph_level`.
- All analog calibration values are raw ESP32 ADC values, not voltages.
