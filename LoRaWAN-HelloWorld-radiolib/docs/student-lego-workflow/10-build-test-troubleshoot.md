# 08 — Build, Flash, Test, and Troubleshoot

This chapter gives the practical command-line workflow and typical debugging steps.

All commands are run from:

```text
LoRaWAN-HelloWorld-radiolib/
```

## Build

After generating `platformio.ini` and copying it into the project directory, build with:

```bash
pio run
```

The generated file sets `default_envs`, so no `-e` is usually required.

If you want to specify the environment explicitly:

```bash
pio run -e <environment-name>
```

## Upload

Upload to the ESP32:

```bash
pio run -t upload
```

Explicit environment variant:

```bash
pio run -e <environment-name> -t upload
```

## Serial monitor

Open the monitor:

```bash
pio device monitor
```

Useful things to check:

- boot reason
- boot count
- selected sensor configuration
- selected sensor index
- selected sensor name
- LoRaWAN join or session restore
- fPort
- diagnostic messages
- calibration output

## Expected startup information

Typical serial output contains a sensor configuration block:

```text
[APP] Sensor configuration:
[APP]   GPS: 1
[APP]   temperature: 1
[APP]   pH: 1
[APP]   PH4502C board temperature: 1
[APP]   TDS: 1
[APP]   turbidity: 1
```

The exact values depend on the generated configuration.

## TTN live data check

In TTN live data, check:

- join accepted
- uplinks arriving
- correct fPort
- decoded payload present
- no JavaScript formatter error
- no unexpected decoded fields

Expected decoded examples:

TDS:

```json
{
  "tds_ppm": 350.0
}
```

pH:

```json
{
  "ph_level": 7.12
}
```

PH4502C board temperature:

```json
{
  "ph_board_temperature_c": 26.0
}
```

## Common problem: formatter syntax error

Symptom in TTN:

```text
as.up.data.decode.fail
SyntaxError
```

Fix:

1. Reload the generator page.
2. Press **Generate payload formatter**.
3. Copy the complete generated formatter.
4. Replace the JavaScript formatter in TTN.
5. Save and wait for the next uplink.

## Common problem: wrong decoded fields

Symptom:

TTN decodes a field that should no longer exist, for example `payload_raw` or TDS temperature.

Cause:

The installed formatter is stale.

Fix:

Regenerate and reinstall `payload-formatter.js`.

## Common problem: GPS waits forever

Symptom:

```text
RadioLib experiment device: Waiting for GPS
```

Possible causes:

- GPS has no sky view
- test is indoors
- RX/TX are swapped
- GPS module is not powered
- wrong baud rate or pins

Fix:

- test outside or near a window
- check GPS TX to ESP32 RX
- check GPS RX to ESP32 TX
- check supply voltage
- wait longer for first fix

## Common problem: pH unrealistic

Possible causes:

- missing calibration
- probe not stable
- wrong analog pin
- changed voltage divider
- pH board potentiometer changed
- probe dry, dirty, old, or badly stored

Fix:

Use pH 4, pH 7, and pH 10 buffer solutions and update raw ADC values in the generator.

## Common problem: TDS zero or too low

Possible causes:

- sensor not powered
- wrong ADC pin
- bad calibration points
- probe not in liquid
- fallback temperature unrealistic

Fix:

Use TDS calibration mode and record raw ADC values for known reference solutions.

## Common problem: turbidity inverted

Some turbidity boards produce lower ADC values for more turbid water. Others may differ.

The firmware can handle either direction as long as the calibration points are entered as measured.

## Common problem: join failure after nonce reset

LoRaWAN nonces must not be reused. If the device clears local nonces but TTN still remembers old nonces, OTAA join may fail.

Fix:

- use factory reset for normal session reset
- use dangerous nonce reset only with TTN nonce reset or new DevEUI

## Debugging discipline

Debug in layers:

```text
1. Does the firmware build?
2. Does upload work?
3. Does serial output start?
4. Does LoRaWAN join or restore session?
5. Does the selected sensor read something plausible?
6. Does TTN receive the fPort?
7. Does TTN decode the payload correctly?
```

Do not change five things at once. Change one thing, test, then continue.
