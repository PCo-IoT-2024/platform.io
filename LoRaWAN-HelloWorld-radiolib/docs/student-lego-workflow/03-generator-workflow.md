# 03 — Generator Workflow

The generator is the central tool for this branch. It creates the firmware configuration and the matching TTN payload formatter.

Open:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

The generator runs locally in the browser. No server is required.

## Why the generator exists

Without the generator, students would need to manually edit many PlatformIO build flags. That is error-prone because the following things must be consistent:

```text
selected sensors
ESP32 pins
LoRaWAN version
TTN keys
calibration values
payload formatter fPorts
```

The generator reduces this complexity by collecting all decisions in one page.

## Generated files

The generator creates two files:

| File | Used by | Purpose |
|---|---|---|
| `platformio.ini` | PlatformIO / firmware build | compile-time configuration |
| `payload-formatter.js` | TTN | uplink decoding |

These files must be generated from the same settings.

## Device and LoRaWAN card

This card configures:

- environment name
- radio module
- LoRaWAN version
- region
- uplink interval

The environment name becomes the PlatformIO environment:

```ini
[env:water_buoy_sx1262]
```

The generator also writes:

```ini
[platformio]
default_envs = water_buoy_sx1262
```

Therefore students can usually build with:

```bash
pio run
```

and upload with:

```bash
pio run -t upload
```

## TTN credentials card

The credential card contains:

```text
DevEUI
AppKey
NwkKey
```

The NwkKey is enabled only for LoRaWAN 1.1.0. For LoRaWAN 1.0.x the older key model is used and the generated INI does not emit a separate NwkKey.

Supported key input formats:

| Format | Description |
|---|---|
| Direct hex | continuous hex string |
| MSB comma bytes | byte list in normal order |
| LSB comma bytes | byte list in reversed byte order |

The generated firmware flags use the format expected by the C++ code and RadioLib.

## Maintenance and calibration pins

The maintenance card contains reset pins and analog calibration pins.

Reset pins:

| Pin | Purpose |
|---|---|
| Factory reset | clear saved session while preserving nonces |
| Dangerous nonce reset | clear session and nonces |

Analog calibration pins:

| Pin | Mode |
|---|---|
| pH calibration pin | raw ADC and pH |
| pH board temp. calibration pin | raw ADC and board temperature |
| TDS calibration pin | raw ADC and ppm |
| Turbidity calibration pin | raw ADC and NTU |

The electrical idea is simple:

```text
ESP32 GPIO ---- button ---- GND
```

The firmware uses internal pull-ups. When the button is pressed during startup, the pin reads LOW.

## Sensor cards

Each sensor card contains:

- enable checkbox if the sensor is optional
- ESP32 pin fields
- calibration values if needed
- short explanation
- fPort badge

The PH4502C board-temperature channel is currently mandatory. It belongs to the PH4502C board and is decoded on fPort 6.

## PlatformIO INI output

The `platformio.ini` output is empty at page load except for the instruction text:

```text
Press “Generate platformio.ini”.
```

This prevents students from using stale generated content.

After pressing the button, copy or download the generated file and place it in:

```text
LoRaWAN-HelloWorld-radiolib/platformio.ini
```

## Payload formatter output

The payload formatter output is also generated on demand.

It includes only the selected optional sensor fPorts, plus mandatory fPort 6 and diagnostic fPorts.

The current generated formatter does not include `payload_raw` for measurement payloads.

## Regeneration rule

Regenerate both files whenever one of these changes:

- selected sensors
- pins
- LoRaWAN version
- credentials
- region
- calibration values
- uplink interval

A good rule for students is:

```text
If you change anything in the generator, regenerate both files.
```
