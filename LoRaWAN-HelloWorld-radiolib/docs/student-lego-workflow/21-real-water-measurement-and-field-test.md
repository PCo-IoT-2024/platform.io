# Real Water Measurement and Field Test

This chapter prepares the group for the real measurement session at the lake.

A field test is different from a lab test. In the lab, students can reset devices, reconnect cables, open serial monitors, and change settings. At the lake, the system must work as an integrated measurement chain.

The goal is not only to collect numbers. The goal is to collect interpretable water-quality data together with enough context to understand what the numbers mean.

---

## Field-test goal

The minimum field-test goal is:

```text
A real sensor value is measured in lake water, sent by the ESP32 buoy, decoded in TTN, bridged to the Raspberry Pi, stored in MariaDB, and shown in the dashboard.
```

A good field test also records environmental context:

- date and time
- weather
- sun or shade
- approximate water condition
- sensor depth
- GPS position
- battery state
- visible disturbances
- calibration state

Without context, measured values are much harder to interpret.

---

## Before going to the lake

Do not start the field test with an untested system.

The group should complete this checklist indoors first:

```text
[ ] ESP32 firmware builds and uploads.
[ ] TTN live data shows decoded uplinks.
[ ] payload formatter works without errors.
[ ] Raspberry Pi is reachable by SSH.
[ ] mqttbroker runs.
[ ] mqttbridge forwards TTN uplinks.
[ ] mqttcli stores rows in MariaDB.
[ ] dashboard shows recent measurements.
[ ] battery is charged.
[ ] sensor wiring is mechanically stable.
[ ] electronics are protected from water.
```

The field test should confirm the complete system, not discover basic setup problems for the first time.

---

## Mechanical preparation

Before placing anything near the lake, check:

```text
[ ] electronics enclosure closed
[ ] cable entries sealed
[ ] sensor cables strain-relieved
[ ] battery fixed mechanically
[ ] antenna mounted safely
[ ] GPS has sky view if possible
[ ] sensors can reach water
[ ] buoy cannot drift away unintentionally
[ ] recovery line or safe access planned
```

Do not place an unprotected development board directly above open water.

---

## Sensor preparation

Before measurement:

- rinse pH probe carefully
- check pH calibration values
- check TDS probe is clean
- check turbidity sensor window is clean
- check DS18B20 waterproof probe condition
- verify analog sensor power
- verify sensor readings in clean or reference water if possible

For pH, use calibration buffers before the real measurement session if time allows.

For TDS and turbidity, at least perform plausibility checks in clear water and a known more turbid or more conductive sample.

---

## Measurement procedure at the lake

Suggested procedure:

1. Start Raspberry Pi backend.
2. Start mqttbroker, mqttbridge, storage process, and dashboard.
3. Open TTN live data on a laptop or tablet.
4. Power the ESP32 buoy.
5. Wait for LoRaWAN join or session restore.
6. Wait for the complete fPort cycle.
7. Put sensors into the water in a stable position.
8. Record time and context.
9. Observe TTN live data.
10. Observe the local dashboard.
11. Let the system run for several cycles.
12. Export or record database/dashboard evidence.

A single uplink is not enough to judge stability. Let the system run long enough to see repeated values.

---

## Context log template

Each group should create a short context log.

Example:

```text
Group:
Date:
Location:
Start time:
End time:
Weather:
Sun/shade:
Water appearance:
Approximate sensor depth:
Buoy position:
Battery state before test:
Battery state after test:
Calibration performed at:
Observed problems:
```

This log belongs into the final report.

---

## What to watch during the measurement

Technical observations:

- Does the fPort sequence continue?
- Does TTN receive uplinks reliably?
- Does GPS get a fix?
- Does the dashboard update?
- Do database rows increase?
- Are values plausible?

Physical observations:

- Is the sensor fully in the water?
- Is the buoy moving?
- Is sunlight heating the enclosure?
- Are bubbles or dirt affecting sensors?
- Is the water visibly clear, cloudy, green, or disturbed?

---

## Safety near water

Basic safety rules:

- do not work alone near water
- do not enter unsafe areas
- keep electronics protected
- keep batteries protected
- recover all equipment
- do not leave probes, cables, or plastic parts behind

The field test is a technical exercise, not a reason to take personal risks.

---

## You are done when...

The field test is complete when:

```text
[ ] at least one full fPort cycle was observed
[ ] TTN decoded real lake measurements
[ ] Raspberry Pi stored the values in MariaDB
[ ] dashboard displayed the values
[ ] field context was recorded
[ ] at least one problem or limitation was documented
[ ] equipment was recovered safely
```

A good group can explain not only what values were measured, but also how reliable those values are and what might have influenced them.
