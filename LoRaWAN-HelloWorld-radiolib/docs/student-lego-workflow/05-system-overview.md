# 01 — System Overview

The buoy is a distributed measurement system. It starts with physical quantities in water and ends with live and historical data views in the dashboard.

The system is not only the ESP32 device. It also includes TTN, MQTT transport, a Raspberry Pi backend, MariaDB storage, and a web dashboard.

## System chain

```text
water / environment
  -> sensor probe or sensor board
  -> ESP32 ADC or digital interface
  -> firmware calibration model
  -> compact uplink payload
  -> LoRa radio
  -> LoRaWAN network
  -> TTN application
  -> JavaScript payload formatter
  -> decoded data fields
  -> TTN MQTT integration
  -> mqttbridge on Raspberry Pi
  -> local mqttbroker on Raspberry Pi
  -> mqttcli live subscriber and storage process
  -> MariaDB database
  -> SNode.C backend
  -> dashboard frontend
  -> live and historical data view
```

Every stage can introduce mistakes. A wrong pin, wrong calibration value, wrong LoRaWAN key, wrong payload formatter, wrong MQTT topic, broken bridge configuration, missing database table, or wrong dashboard query can make the system appear broken.

## Realtime and historical paths

After TTN has decoded the payload, the data can be used in two closely related ways.

### Realtime path

```text
TTN decoded uplink
  -> TTN MQTT integration
  -> mqttbridge
  -> local mqttbroker
  -> mqttcli live subscriber
  -> dashboard/live view
```

In this course setup, the live dashboard path is also provided through `mqttcli`. `mqttcli` can subscribe to local MQTT topics and provide or feed the live view of incoming messages.

### Historical path

```text
TTN decoded uplink
  -> TTN MQTT integration
  -> mqttbridge
  -> local mqttbroker
  -> mqttcli storage process
  -> MariaDB
  -> SNode.C backend
  -> dashboard historical view
```

The realtime path is useful for seeing whether messages are arriving now. The historical path is useful for charts, time ranges, reports, and later analysis.

A simple mental model is:

```text
MQTT = live message stream
mqttcli = live subscriber and storage bridge
MariaDB = memory
SNode.C = web/backend access
Dashboard = human view
```

## Main hardware and infrastructure blocks

| Block | Role |
|---|---|
| ESP32 | microcontroller, sensor control, payload construction, deep sleep |
| SX1262/SX1276 | LoRa radio transceiver |
| GPS | position of the buoy |
| DS18B20 | water temperature |
| PH4502C | pH measurement and onboard temperature channel |
| Gravity TDS | dissolved solids estimate |
| Turbidity sensor | water cloudiness estimate |
| battery/solar system | outdoor energy supply |
| TTN / The Things Stack | LoRaWAN network backend and payload formatter |
| TTN MQTT integration | exposes decoded uplinks as MQTT messages |
| Raspberry Pi | local backend server |
| mqttbridge | forwards TTN MQTT messages to the local broker |
| local mqttbroker | local MQTT message hub |
| mqttcli | subscribes to local MQTT messages, supports live view and storage workflow |
| MariaDB | stores historical measurements |
| SNode.C backend | serves dashboard/API access to stored data |
| dashboard frontend | browser view for students and observers |

## Firmware runtime model

The firmware is built around a short wake cycle.

```text
wake up
print boot information
check maintenance/calibration buttons
restore or join LoRaWAN session
select one sensor from sensor table
read sensor
build payload
send uplink
sleep radio
enter ESP32 deep sleep
```

The current sensor is selected from the RTC boot count. This boot count survives deep sleep.

Example with all channels enabled:

```text
boot 1 -> GPS
boot 2 -> temperature
boot 3 -> pH
boot 4 -> TDS
boot 5 -> turbidity
boot 6 -> PH4502C board temperature
boot 7 -> GPS again
```

This approach is good for low-power operation because the device does not keep all sensors powered and active for a long time during every wake-up.

## Measurement and diagnostic fPorts

Measurement fPorts:

| fPort | Measurement |
|---:|---|
| 1 | GPS |
| 2 | DS18B20 water temperature |
| 3 | pH |
| 4 | TDS ppm |
| 5 | turbidity NTU |
| 6 | PH4502C board temperature |

Diagnostic fPorts:

| fPort | Meaning |
|---:|---|
| 220 | request further downlinks |
| 221 | info / diagnostics |
| 222 | warning |
| 223 | error |

## Why fPorts matter

LoRaWAN fPorts separate payload types. The TTN payload formatter uses `input.fPort` to decide how to decode the bytes.

For example:

```text
fPort 3 -> interpret payload as pH
fPort 4 -> interpret payload as TDS ppm
fPort 6 -> interpret payload as PH4502C board temperature
```

If the firmware and formatter disagree about fPorts, the decoded data is wrong.

The same field names then continue through the backend path. For example, `tds_ppm` should mean the same thing in TTN, local MQTT, MariaDB, the SNode.C backend, and the dashboard.

## Current payload philosophy

The current firmware sends only the actual measurement value for each measurement fPort.

Examples:

```text
pH uplink:       7.120000
TDS uplink:      350.000000
turbidity uplink: 42.000000
```

The raw ADC values are printed only during calibration mode. They are not sent via LoRaWAN during normal operation.

This keeps uplinks small and avoids mixing measurement data with calibration/debug data.

## Physical interpretation

The system measures several different types of information:

| Measurement | Type | Interpretation |
|---|---|---|
| GPS | position | where the buoy is |
| water temperature | physical temperature | water context and compensation |
| pH | chemical indicator | acidity / alkalinity |
| TDS | electrical / chemical proxy | dissolved solids estimate |
| turbidity | optical proxy | suspended particles / cloudiness |
| board temperature | electronics context | PH4502C board/environment diagnostic |

The sensors are not equally accurate. GPS and DS18B20 are usually easier to interpret. Low-cost pH, TDS, and turbidity sensors need calibration and careful discussion of limitations.
