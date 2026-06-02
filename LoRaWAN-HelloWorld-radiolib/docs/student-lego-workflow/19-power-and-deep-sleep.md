# 07 — Power, Deep Sleep, and Solar Operation

A buoy should run without a USB cable. That means power consumption matters.

The firmware is already structured for low-power operation:

```text
wake -> measure one sensor -> send one uplink -> deep sleep
```

The hardware should support the same idea by switching off external sensor boards when they are not needed.

## Why deep sleep matters

An ESP32 that stays fully awake consumes much more current than an ESP32 in deep sleep. A solar/battery node can only work well if most of its time is spent sleeping.

A simplified energy picture:

```text
average current = active current * active time fraction + sleep current * sleep time fraction
```

The active current may be high, especially during LoRa transmission or GPS acquisition. But if the active time is short and sleep current is low, the average current can still be acceptable.

## Current firmware sleep model

The firmware normally does this:

1. wakes from timer
2. increments boot counter
3. checks reset/calibration buttons
4. restores LoRaWAN state or joins if needed
5. reads one sensor
6. sends one uplink
7. puts the radio to sleep
8. enters ESP32 deep sleep

The timer wake-up interval is generated from the `Uplink interval` field in the generator.

## One sensor per wake-up

Measuring all sensors on every wake-up would increase the active time. The current scheduler measures one sensor per wake-up and rotates through enabled sensors.

This has several advantages:

- shorter wake time
- smaller payloads
- less sensor power
- simpler fPort decoding
- lower average energy use

The trade-off is that each individual sensor is updated less frequently. If six measurement channels are enabled and the uplink interval is 60 seconds, each channel is updated approximately every six minutes.

## External sensor power

Many sensor boards do not have a useful software sleep mode. This is especially true for low-cost analog boards.

The practical solution is power gating:

```text
ESP32 GPIO -> load switch or MOSFET -> sensor supply rail
```

Recommended rails:

| Rail | Loads |
|---|---|
| `3V3_MAIN` | ESP32 and LoRa radio |
| switched `3V3_SENS` | GPS and DS18B20 |
| switched `5V_SENS` | PH4502C, TDS, turbidity |

## Why analog boards should be switched

PH4502C, TDS, and turbidity boards can consume current continuously if left powered. They may also warm up and drift.

For a real buoy, they should be powered only for measurement. After enabling the sensor rail, the firmware should wait long enough for the analog output to stabilize before reading.

## Solar and battery concept

A practical outdoor concept is:

```text
solar panel -> LiFePO4 solar charger -> LiFePO4 battery -> regulators -> ESP32 and sensors
```

Recommended class:

- 1S LiFePO4 battery, for example 32700 cell around 6000 mAh
- 6 V solar panel, 5 W as baseline, 10 W for more margin
- LiFePO4-compatible solar charger
- low-quiescent 3.3 V buck-boost regulator
- switched 5 V boost converter for analog sensor boards

## Why LiFePO4

LiFePO4 is attractive for outdoor teaching hardware because it is comparatively robust and thermally stable.

Advantages:

- safer chemistry than many LiPo packs
- long cycle life
- good match for embedded systems
- suitable for repeated charge/discharge cycles

The charger must really support LiFePO4 charge voltage. Do not use a normal 4.2 V LiPo charger for a LiFePO4 cell.

## Measuring power

Students should measure, not guess.

Useful measurements:

| Measurement | Why it matters |
|---|---|
| ESP32 deep-sleep current | determines baseline battery drain |
| active current during sensor read | shows cost of measurement |
| LoRa TX current | shows radio burst cost |
| GPS acquisition time | often dominates energy |
| boost converter off-current | determines whether sensor rail is really off |

## Practical rule

A low-power design is not achieved by calling a sleep function only. The complete system must support sleep:

```text
firmware sleep + radio sleep + sensor power switching + low-IQ regulators + no wasteful dev-board parts
```

A normal ESP32 development board may have high sleep current because of USB-UART chips, power LEDs, and inefficient regulators. For final buoy hardware, a custom low-power board is better.
