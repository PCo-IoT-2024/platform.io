# 05 — Sensors and Measurement Theory

This chapter explains what the sensors measure and how their values should be interpreted.

The important lesson is that sensors do not directly produce truth. They produce electrical signals. Firmware converts those signals into numbers, and those numbers must be interpreted carefully.

## GPS

GPS provides position data.

The firmware sends GPS on fPort 1:

```text
latitude, longitude, altitude, hdop
```

Decoded TTN fields:

```text
latitude
longitude
altitude
hdop
```

HDOP means horizontal dilution of precision. Lower HDOP values usually indicate a better horizontal position estimate. A high HDOP value means the position should be treated with caution.

GPS problems are common indoors. If the GPS module has no fix, the firmware sends an info message on fPort 221.

## DS18B20 water temperature

The DS18B20 is a digital temperature sensor. It is used as the water-temperature sensor.

The firmware sends it on fPort 2 as:

```text
temperature_c
```

The DS18B20 is usually easier to handle than analog sensors because the measurement is already digital. Still, wiring matters. The OneWire data line requires a pull-up resistor, typically 4.7 kΩ.

## pH

pH describes how acidic or alkaline a liquid is.

Simplified interpretation:

| pH | Meaning |
|---:|---|
| below 7 | acidic |
| 7 | neutral |
| above 7 | alkaline / basic |

The PH4502C board converts the pH probe signal into an analog voltage. The ESP32 reads this voltage as a raw ADC value. The firmware then converts the raw ADC value to pH using calibration values.

The firmware sends pH on fPort 3 as:

```text
ph_level
```

pH probes require calibration fluids and careful handling. A dry, old, dirty, or badly stored probe can produce unreliable results.

## PH4502C board temperature

The PH4502C board also provides a temperature-related analog output on T1. In this project it is used as PH4502C board/onboard temperature.

It is sent on fPort 6 as:

```text
ph_board_temperature_c
```

This is not the water temperature. It is a diagnostic value for the electronics around the PH4502C board. It may be affected by sunlight, enclosure heating, self-heating, air movement, and board placement.

Use the DS18B20 for water temperature.

## TDS

TDS means Total Dissolved Solids. A TDS sensor estimates dissolved ions and solids indirectly from electrical conductivity.

The firmware sends TDS on fPort 4 as:

```text
tds_ppm
```

TDS is often given in ppm. It is useful for observing trends, but it is not a full chemical analysis. Two different liquids can have the same TDS value but very different chemical composition.

TDS depends on temperature. The firmware may use the last valid DS18B20 water temperature internally for compensation. The compensation temperature is not sent together with the TDS uplink.

## Turbidity

Turbidity describes how cloudy the water is. Low-cost turbidity modules estimate this optically. Suspended particles affect how much light reaches the detector.

The firmware sends turbidity on fPort 5 as:

```text
turbidity_ntu
```

NTU means Nephelometric Turbidity Unit. In this course setup, the low-cost sensor and simple linear calibration are suitable for trends and experiments, not for laboratory-grade turbidity certification.

## Measurement limitations

The following points should be discussed in student reports:

- sensor calibration state
- probe aging and contamination
- temperature effects
- analog noise
- supply-voltage stability
- waterproofing quality
- mechanical placement in water
- whether the value is absolute or mainly useful as a trend

## Why multiple sensors are useful

One sensor value alone rarely explains a water system. The combination is more useful:

| Combination | Interpretation example |
|---|---|
| GPS + turbidity | where cloudy water was observed |
| temperature + TDS | conductivity-related changes with temperature context |
| pH + temperature | chemical condition with environmental context |
| turbidity + GPS | possible algae, mud, disturbance, or suspended particles |

The dashboard or later database should keep timestamps, location, and fPort-specific values together.
