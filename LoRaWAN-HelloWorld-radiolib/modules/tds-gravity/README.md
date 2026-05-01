# Gravity TDS Module

This module adds a Gravity TDS sensor.

## Files

- `TdS.h`
- `TdS.cpp`
- `platformio.fragment.ini`

## Wiring

| TDS board | ESP32 |
|---|---|
| Analog output | GPIO 32 by default |
| VCC | sensor-specific supply voltage |
| GND | GND |

Check the exact sensor board voltage and ADC scaling before connecting it to the ESP32.

## Validated Course Mapping

This module was validated as part of `course/full` / PR #12 with:

```ini
-D TDS_SENSOR_PIN=32
-D A1=TDS_SENSOR_PIN
```

The `A1` definition is required because the current Gravity TDS library constructor refers to Arduino-style `A1`. Mapping `A1` to `TDS_SENSOR_PIN` keeps GPIO 32 as the single source of truth.

Validation status from `course/full`:

- PlatformIO build: OK
- hardware upload/run: OK
- TTN uplinks: OK

## Temperature Compensation

The Gravity TDS library supports temperature compensation. The application should pass the current water temperature if a temperature sensor is available.

```cpp
float tdsValue = tdsSensor.getValue(temperatureC);
```

If no temperature sensor is present, use a documented default such as 22 or 25 degrees Celsius and explain this in the experiment protocol.

## PlatformIO

The module requires:

```ini
https://github.com/PCo-IoT-2024/GravityTDS.git
eeprom
```
