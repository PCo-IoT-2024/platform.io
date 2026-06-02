# Course Context and Learning Goals

This project is a teaching project for building an Internet-of-Things water-monitoring buoy. The goal is not only to make an ESP32 send values to TTN, but to understand how such a system is designed from the sensor up to the cloud data view.

Students work with a realistic, but still manageable system:

```text
sensors -> ESP32 -> LoRa radio -> LoRaWAN -> TTN -> decoded data
```

The system is intentionally modular. Each sensor is a building block, and the generator selects which blocks are used for a particular buoy.

## What students should learn

After completing this project, students should be able to explain and perform these tasks:

- describe the role of a microcontroller in an IoT sensing node
- explain why LoRaWAN is useful for low-power long-range sensing
- distinguish between device firmware, network infrastructure, and application data
- configure a TTN OTAA device
- generate a PlatformIO configuration from device settings
- flash an ESP32 firmware
- read serial diagnostics
- install and test a TTN payload formatter
- understand why analog sensors need calibration
- use raw ADC values for calibration
- distinguish between water temperature and board temperature
- reason about low-power design and sensor power gating

## Why this is a lego workflow

The project is called “lego” because the firmware can be assembled from selectable blocks.

Examples:

```text
GPS only
GPS + temperature
pH + temperature
pH + TDS + turbidity
full water-quality buoy
```

The current generator hides most compile-time complexity. Students select sensors and pins in the UI. The generator then writes the necessary build flags into `platformio.ini`.

## Important engineering idea

The important engineering idea is that hardware, firmware, and cloud decoding must match.

These three things belong together:

```text
selected sensors in generator
build flags in platformio.ini
fPort cases in payload-formatter.js
```

If one part is changed but the others are not regenerated, the system may still compile and send uplinks, but TTN may decode them incorrectly.

## Current course branch

Use:

```text
course/lego-configurable
```

Do not start from `main` or an older experimental branch unless instructed.

## Student deliverables

A reasonable student group deliverable could include:

- working fork and branch
- generated `platformio.ini`
- documented pin mapping
- recorded calibration values
- screenshots or exports of TTN decoded uplinks
- short explanation of the selected sensors
- short discussion of limitations and expected accuracy
- optional notes about power design and deep sleep

## Safety and responsibility

Do not publish real AppKey or NwkKey values. Treat LoRaWAN root keys as secrets.

Do not put electronics directly into water. Use proper waterproofing and strain relief.

Do not assume low-cost sensors are laboratory instruments. They are useful for learning, trend observation, and relative changes, but most of them require calibration and careful interpretation.
