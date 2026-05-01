# DS18B20 Temperature Module

This module adds a waterproof DS18B20 temperature sensor.

## Files

- `DS18B20.h`
- `DS18B20.cpp`
- `platformio.fragment.ini`

## Wiring

| DS18B20 | ESP32 |
|---|---|
| VCC | 3.3 V |
| GND | GND |
| DATA | GPIO 27 by default |

Use a 4.7 kOhm pull-up resistor between DATA and 3.3 V.

## PlatformIO

The module requires:

- `PaulStoffregen/OneWire`
- `milesburton/DallasTemperature`

The default data pin is configured by:

```ini
-D DALLAS_TEMPERATURE_PIN=27
```

## Application Integration

Create one sensor instance:

```cpp
static temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
```

During acquisition:

```cpp
temp.setup();
if (temp.isValid()) {
    float temperatureC = temp.getTemperature();
}
```

The sensor conversion delay is handled inside `setup()`.
