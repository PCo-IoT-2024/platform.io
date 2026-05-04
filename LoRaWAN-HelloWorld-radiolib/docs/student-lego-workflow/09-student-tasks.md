# 09 — Student Tasks and Suggested Exercises

This chapter contains suggested exercises for student groups. The exercises move from simple configuration to interpretation and reflection.

## Task 1 — Repository setup

Goal: create a personal working branch.

Steps:

1. Fork the repository.
2. Clone the fork.
3. Add upstream.
4. Create a branch from `upstream/course/lego-configurable`.
5. Enter `LoRaWAN-HelloWorld-radiolib/`.

Deliverable:

```text
screenshot or terminal log showing current branch
```

## Task 2 — Generate first firmware configuration

Goal: use the generator to create `platformio.ini`.

Steps:

1. Open the generator.
2. Select radio module.
3. Select LoRaWAN version.
4. Enter TTN credentials.
5. Keep default sensors enabled.
6. Generate `platformio.ini`.
7. Copy it into the project directory.

Deliverable:

```text
platformio.ini in the student branch
```

## Task 3 — Generate and install payload formatter

Goal: make TTN decode the uplinks correctly.

Steps:

1. Generate `payload-formatter.js`.
2. Install it in TTN.
3. Wait for uplinks.
4. Verify decoded fields.

Deliverable:

```text
TTN screenshot showing decoded payload
```

## Task 4 — Build and upload

Goal: compile and flash the firmware.

Commands:

```bash
pio run
pio run -t upload
pio device monitor
```

Deliverable:

```text
serial monitor output showing sensor configuration and LoRaWAN status
```

## Task 5 — Verify fPort rotation

Goal: understand that the firmware sends one sensor per wake-up.

Steps:

1. Observe several wake cycles.
2. Record the fPort sequence.
3. Compare it with the enabled sensors.

Expected with all current measurement channels:

```text
1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 1 -> ...
```

Deliverable:

```text
short table: wake number, sensor, fPort, decoded field
```

## Task 6 — pH calibration

Goal: calibrate the pH sensor using buffer solutions.

Steps:

1. Enter pH calibration mode.
2. Measure raw ADC in pH 4, pH 7, and pH 10 buffer.
3. Enter values into generator.
4. Regenerate `platformio.ini`.
5. Flash and verify pH value.

Deliverable:

```text
recorded pH 4 / pH 7 / pH 10 raw ADC values
```

## Task 7 — TDS or turbidity calibration

Goal: understand two-point linear calibration.

Steps:

1. Choose TDS or turbidity.
2. Prepare two reference conditions.
3. Record raw ADC values.
4. Enter calibration values into generator.
5. Verify decoded TTN output.

Deliverable:

```text
calibration table and interpretation of uncertainty
```

## Task 8 — Explain one measurement limitation

Goal: think critically about sensor data.

Choose one sensor and explain:

- what it measures directly
- what physical or chemical property we infer from it
- what can disturb the measurement
- why calibration is needed
- whether the value is absolute or mainly useful as a trend

Deliverable:

```text
short written explanation, 0.5 to 1 page
```

## Task 9 — Power discussion

Goal: connect firmware behavior to battery/solar operation.

Explain why this firmware measures only one sensor per wake-up. Discuss what would happen if all sensors stayed powered all the time.

Deliverable:

```text
short power-domain diagram and explanation
```

## Task 10 — Final mini report

A final report should include:

- selected sensor set
- radio module
- LoRaWAN version
- TTN device setup notes
- pin mapping
- calibration values
- payload formatter status
- example decoded TTN payloads
- problems encountered
- interpretation of measured values
- limitations and possible improvements

Suggested structure:

```text
1. System configuration
2. Hardware wiring
3. Firmware and generator settings
4. Calibration
5. TTN results
6. Discussion and limitations
7. Improvements
```

## Optional advanced task — power prototype

Design a small power system for the buoy:

```text
solar panel -> charger -> battery -> regulators -> switched sensor rails
```

Discuss:

- battery chemistry
- solar panel size
- 3.3 V rail
- 5 V sensor rail
- sensor power gating
- expected deep-sleep current
- what you would measure to validate the design
