# PH4502C pH Module

This module adds a PH4502C analog pH sensor.

## Files

- `PH4502C.h`
- `PH4502C.cpp`
- `platformio.fragment.ini`

## Wiring

| PH4502C | ESP32 |
|---|---|
| pH analog output | GPIO 35 by default |
| temperature analog output | GPIO 32 by default |
| VCC | sensor-specific supply voltage |
| GND | GND |

Check the exact module voltage requirements before connecting it to the ESP32 ADC.

## Calibration

The default fragment uses three calibration points:

```ini
-D PH10_ADC_VALUE=1615
-D PH7_ADC_VALUE=2080
-D PH4_ADC_VALUE=2503
```

The implementation fits a quadratic calibration curve through these three points.

## Application Integration

Create one sensor instance with the calibration points:

```cpp
static ph::PH4502C pH(
    PH4502C_PH_PIN,
    PH4502C_TEMPERATURE_PIN,
    {{PH10_ADC_VALUE, 10}, {PH7_ADC_VALUE, 7}, {PH4_ADC_VALUE, 4}}
);
```

During acquisition:

```cpp
pH.setup();
float phValue = pH.getPHLevel();
```
