# System Overview

**Primary workstreams:** Student 1 — ESP32 firmware, Student 2 — sensors, Student 3 — TTN cloud setup

**Interfaces:** fPort mapping, decoded field names, pin mapping, calibration values

This chapter describes the concrete buoy node and its immediate data model. The full cross-system architecture is explained earlier in the end-to-end architecture chapter. Here the focus is narrower: what the ESP32 firmware measures, how the measurements are scheduled, which fPorts are used, and which fields must remain consistent later in TTN, MQTT, MariaDB, and the dashboard.

The buoy node is a distributed measurement system. It starts with physical quantities in water and produces decoded measurement fields in TTN.

```text
water / environment
  -> sensor
  -> ESP32 pin or digital bus
  -> firmware measurement code
  -> calibration model
  -> LoRaWAN uplink
  -> TTN payload formatter
  -> decoded field
```

Every stage can introduce mistakes. A wrong pin, wrong calibration value, wrong LoRaWAN key, wrong fPort, or stale payload formatter can make the system appear broken.

## Main device-side blocks

| Block | Role |
|---|---|
| ESP32 | microcontroller running the firmware |
| LoRa radio module | radio link to LoRaWAN gateway |
| GPS | buoy position |
| DS18B20 | water temperature |
| PH4502C pH channel | pH value |
| PH4502C board-temperature channel | onboard/air temperature value |
| Gravity TDS | dissolved-solids estimate |
| turbidity sensor | water-cloudiness estimate |
| battery/solar system | outdoor power supply |

The backend blocks are intentionally not repeated in detail here. They are covered in the Raspberry Pi, MQTT, MariaDB, and dashboard chapters.

## Firmware runtime model

The firmware normally follows this cycle:

```text
wake up
  -> initialize hardware
  -> restore LoRaWAN session or join
  -> check calibration/reset buttons
  -> read one selected sensor
  -> apply calibration if needed
  -> send one uplink on one fPort
  -> put radio to sleep
  -> enter ESP32 deep sleep
```

Only one measurement channel is sent per wake-up. The firmware rotates through the enabled measurement channels. This keeps payloads small and reduces active time.

Example with all current measurement channels enabled:

```text
wake 1 -> fPort 1 -> GPS
wake 2 -> fPort 2 -> water temperature
wake 3 -> fPort 3 -> pH
wake 4 -> fPort 4 -> TDS
wake 5 -> fPort 5 -> turbidity
wake 6 -> fPort 6 -> PH4502C board temperature
wake 7 -> fPort 1 -> GPS again
```

## fPort contract

The fPort contract is one of the most important interfaces in the whole project.

| fPort | Measurement | Decoded field |
|---:|---|---|
| 1 | GPS position | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | DS18B20 water temperature | `temperature_c` |
| 3 | PH4502C pH | `ph_level` |
| 4 | Gravity TDS | `tds_ppm` |
| 5 | Turbidity | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

The same meaning must be used in:

```text
ESP32 firmware
TTN payload formatter
TTN live data
local MQTT messages
MariaDB f_port column
dashboard interpretation
final report
```

## Payload philosophy

The current firmware sends only the final measurement value for each measurement fPort.

It does not send additional raw ADC values, intermediate voltages, or pH board temperature together with the pH value.

Examples:

```text
fPort 3 -> only ph_level
fPort 4 -> only tds_ppm
fPort 5 -> only turbidity_ntu
fPort 6 -> only ph_board_temperature_c
```

Calibration-mode serial output is different. During calibration the ESP32 prints raw ADC values to the serial console so students can enter calibration values into the generator. These raw calibration values are not part of the normal LoRaWAN measurement payload.

## Done when

This chapter is understood when the group can explain:

```text
[ ] which sensor uses which fPort
[ ] why only one measurement is sent per wake-up
[ ] why raw ADC values are used for calibration but not sent as normal payload
[ ] why firmware, formatter, database, and dashboard must use the same field meanings
```
