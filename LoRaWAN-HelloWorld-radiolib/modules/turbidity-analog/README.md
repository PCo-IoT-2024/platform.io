# Analog Turbidity Module

This module adds an analog turbidity sensor with a simple two-point linear calibration.

## Files

- `TurbiditySensor.h`
- `TurbiditySensor.cpp`
- `platformio.fragment.ini`

## Wiring

| Turbidity board | ESP32 |
|---|---|
| Analog output | GPIO 33 by default |
| VCC | sensor-specific supply voltage |
| GND | GND |

Check the sensor board voltage and output range before connecting it to the ESP32 ADC.

## Calibration

The default fragment uses two calibration points:

```ini
-D TURBIDITY_CLEAR_WATER_VOLTAGE=1.780
-D TURBIDITY_CLEAR_WATER_NTU=0.0
-D TURBIDITY_TURBID_WATER_VOLTAGE=0.840
-D TURBIDITY_TURBID_WATER_NTU=600.0
```

The implementation maps voltage to NTU linearly. This is useful for a first experiment, but students should document their calibration water samples.

## Application Integration

```cpp
static turbidity::TurbiditySensor turbiditySensor(
    TURBIDITY_PIN,
    TURBIDITY_VCC,
    TURBIDITY_ADC_MAX,
    TURBIDITY_CLEAR_WATER_VOLTAGE,
    TURBIDITY_CLEAR_WATER_NTU,
    TURBIDITY_TURBID_WATER_VOLTAGE,
    TURBIDITY_TURBID_WATER_NTU
);
```

During acquisition:

```cpp
turbiditySensor.setup();
float ntu = turbiditySensor.getNTU();
```
