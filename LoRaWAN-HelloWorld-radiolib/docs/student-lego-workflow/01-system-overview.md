# 01 — System Overview

The buoy is a distributed measurement system. It starts with physical quantities in water and ends with decoded fields in TTN.

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
```

Every stage can introduce mistakes. A wrong pin, wrong calibration value, wrong LoRaWAN key, or wrong payload formatter can make the system appear broken.

## Main hardware blocks

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
