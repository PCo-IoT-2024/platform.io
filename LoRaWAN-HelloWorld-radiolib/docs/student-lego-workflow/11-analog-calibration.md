# 06 — Analog Calibration

Analog sensors are not used directly. The ESP32 reads a raw ADC value, and firmware converts this raw value into a calibrated measurement.

In this project all analog calibration values in the generator are raw ESP32 ADC values.

They are not voltages.

## Why raw ADC values?

The raw ADC value is what the firmware actually sees. It already includes effects from:

- sensor board supply voltage
- sensor output circuit
- ESP32 ADC behavior
- chosen ADC pin
- voltage dividers
- RC filters
- cable length
- electrical noise
- tolerances

Therefore the calibration workflow is:

```text
known reference condition -> read raw_adc from serial monitor -> enter raw_adc into generator
```

This avoids needing to calculate voltage first.

## Calibration mode output

The firmware uses the same serial format for all analog calibration modes:

```text
[CAL] sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>
```

Examples:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
[CAL] sensor=turbidity, raw_adc=1200.00, calibrated_ntu=600.000
[CAL] sensor=ph_board_temperature, raw_adc=302.00, calibrated_temperature_c=26.000
```

The first readings after power-up may drift. Use stable values.

## pH calibration

pH uses three reference points:

| Field | Reference |
|---|---|
| pH 4 ADC | pH 4 buffer solution |
| pH 7 ADC | pH 7 buffer solution |
| pH 10 ADC | pH 10 buffer solution |

The firmware uses these points for a quadratic fit.

Recommended procedure:

1. Prepare pH 4, pH 7, and pH 10 buffer solutions.
2. Rinse the probe before each liquid.
3. Put the probe into pH 7 first.
4. Wait until the reading stabilizes.
5. Record raw ADC.
6. Repeat for pH 4 and pH 10.
7. Enter the values into the generator.
8. Generate and flash a new `platformio.ini`.

Do not stir aggressively with the glass probe. Do not wipe the probe membrane dry with force.

## PH4502C board temperature calibration

The PH4502C board-temperature channel uses two points:

| Field | Current default |
|---|---:|
| Low temp ADC | 0 |
| Low temp °C | 0 |
| High temp ADC | 302 |
| High temp °C | 26 |

The firmware uses a linear fit:

```text
temperature_c = a * raw_adc + b
```

This value should be interpreted as board/onboard temperature. It is not water temperature.

If the ADC value slowly falls or rises after power-up, this can be normal. The board and analog circuit may be warming up or stabilizing. Wait until the reading changes only slowly before recording calibration values.

## TDS calibration

TDS uses two points:

| Field | Meaning |
|---|---|
| Low TDS ADC | raw ADC in the low/reference solution |
| Low TDS ppm | known ppm of the low/reference solution |
| High TDS ADC | raw ADC in the high/reference solution |
| High TDS ppm | known ppm of the high/reference solution |

The firmware uses a linear fit:

```text
tds_ppm = a * raw_adc + b
```

The DS18B20 water temperature may be used internally for temperature compensation. If no valid water temperature exists, the fallback temperature from the generator is used.

Only `tds_ppm` is sent via LoRaWAN.

## Turbidity calibration

Turbidity uses two points:

| Field | Meaning |
|---|---|
| Clear water ADC | raw ADC in clear water |
| Clear water NTU | assigned/reference NTU for clear water |
| Turbid water ADC | raw ADC in turbid reference sample |
| Turbid water NTU | assigned/reference NTU for turbid sample |

The firmware uses a linear fit:

```text
turbidity_ntu = a * raw_adc + b
```

The ADC direction depends on the module. Some modules produce lower ADC values for higher turbidity. Others may behave differently. Enter the values exactly as measured.

## General calibration rules

- Use stable readings.
- Use the same supply voltage during calibration and normal measurement.
- Do not change voltage dividers after calibration.
- Do not change ADC pins after calibration.
- Keep analog wires short.
- Keep analog wiring away from radio and switching regulators.
- Regenerate and reflash after changing calibration values.

## Why calibration belongs in the generator

Calibration values are compile-time configuration in this project. The generated `platformio.ini` contains the values used by firmware.

That means a calibration change requires:

```text
update generator values -> regenerate platformio.ini -> flash firmware
```

This is simple and visible for teaching. A later advanced version could store calibration values in flash or receive them by downlink, but that is not the goal of this beginner-friendly workflow.
