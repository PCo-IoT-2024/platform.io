# 04 — Firmware Architecture

The firmware is organized around one main application file and several sensor classes.

Main application:

```text
src/LoRaWANTemplate.cpp
```

Sensor classes:

```text
src/sensors/
```

LoRaWAN support:

```text
src/LoRaWAN.hpp
```

GPS support:

```text
src/GPS.cpp
src/GPS.h
```

## Compile-time configuration

The generator writes C/C++ preprocessor definitions into `platformio.ini`.

Examples:

```ini
-D APP_HAS_GPS=1
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=1
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=1
-D APP_HAS_PH_BOARD_TEMPERATURE=1
```

The C++ code uses these definitions with `#if` and `#endif`.

This means unused sensor code is not part of the active build path.

## Sensor object construction

Sensor objects are created only when their feature flags are enabled.

Examples:

```cpp
#if APP_HAS_TEMPERATURE
static temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
#endif
```

The PH4502C object is used for both pH and board temperature:

```text
pH value                  -> fPort 3
PH4502C board temperature -> fPort 6
```

## Prepared uplink

The firmware uses a small structure for the next uplink:

```cpp
struct PreparedUplink {
    uint8_t fPort = 221;
    std::string payload;
};
```

Each sensor preparation function writes:

```text
fPort
payload string
```

Examples:

```text
pH       -> fPort 3, payload = "7.120000"
TDS      -> fPort 4, payload = "350.000000"
turbidity -> fPort 5, payload = "42.000000"
```

## Sensor slot table

Enabled sensors are placed into a compile-time table.

Conceptually:

```text
sensorSlots[] = {
  GPS,
  temperature,
  pH,
  TDS,
  turbidity,
  PH4502C board temperature
}
```

The actual entries depend on generator flags.

The current sensor is selected with the RTC boot count:

```text
currentSensor = (bootCount - 1) % sensorCount
```

This creates a deterministic rotation through the enabled sensors.

## Why not measure everything at once?

Measuring all sensors on every wake-up would be simpler, but it would increase:

- wake time
- sensor power consumption
- LoRa payload size
- TTN airtime
- battery drain

The one-sensor-per-wake strategy is better for battery and solar operation.

## Calibration mode

Calibration modes are entered before normal LoRaWAN operation.

Startup sequence, simplified:

```text
start serial
print wake reason
check calibration buttons
if calibration requested: run calibration loop
otherwise continue normal LoRaWAN setup
```

Calibration loops print raw ADC values and the currently calculated calibrated value.

Example:

```text
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
```

When the calibration button is released, the ESP32 restarts.

## LoRaWAN persistence

The firmware stores LoRaWAN session information so that the device does not need to perform a full OTAA join after every deep sleep.

Important concepts:

| Concept | Meaning |
|---|---|
| session | active LoRaWAN state after join |
| nonce | value used to prevent replay and duplicate joins |
| frame counter | uplink/downlink counter tracked by LoRaWAN |

Factory reset clears the session but preserves nonces. Dangerous nonce reset clears more state and must be used carefully.

## Deep sleep

After the LoRaWAN loop has sent the uplink, the firmware puts the radio to sleep and starts ESP32 deep sleep.

The wake-up source is normally the timer configured from the generated uplink interval.

Deep sleep keeps RTC memory, so the boot count and the last valid water temperature can survive sleep.

## Last water temperature

The DS18B20 water-temperature value can be stored and reused for TDS temperature compensation.

This is useful because TDS and temperature may not be measured during the same wake-up.

If no valid water temperature is available, TDS uses the fallback value from the generator.
