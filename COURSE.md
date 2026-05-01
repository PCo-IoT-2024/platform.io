# LoRaWAN Water Monitoring Buoy Course

This branch is the safe course base for a modular PlatformIO LoRaWAN water monitoring buoy project.

Existing branches such as `main`, `radiolib-master`, `add_temperature`, `add_ph`, `add_tds`, and `add_turbidity` are intentionally left untouched.

## Branches created for the course

- `course/base-lorawan`: minimal RadioLib LoRaWAN base
- `course/temperature-ds18b20`: DS18B20 temperature module branch
- `course/module-ph-ph4502c`: pH module branch
- `course/mod03`: Gravity TDS module branch
- `course/mod04`: analog turbidity module branch
- `course/full`: validated integration branch for the complete buoy

## Validated full integration branch

The branch `course/full` is the currently validated complete firmware.

Validation status:

- PlatformIO build: OK
- hardware upload/run: OK
- TTN uplinks: OK
- pin mapping checked against the existing `add_turbidity` branch

Build command:

```bash
cd LoRaWAN-HelloWorld-radiolib
pio run -c platformio.course-full.ini
```

Upload command:

```bash
pio run -c platformio.course-full.ini -t upload
```

## Confirmed pin mapping

| Function | GPIO |
|---|---:|
| GPS RX | 16 |
| GPS TX | 17 |
| DS18B20 temperature | 27 |
| pH analog | 34 |
| pH temperature | 35 |
| Gravity TDS | 32 |
| Turbidity | 33 |
| LoRa module bitmap | `5, 2, 14, 4` |

The Gravity TDS library expects an Arduino-style `A1` symbol. In the validated course configuration this is mapped to the TDS pin:

```ini
-D TDS_SENSOR_PIN=32
-D A1=TDS_SENSOR_PIN
```

## Intended workflow

Students start from `course/base-lorawan` and merge only the module branches they need.

For the complete validated reference firmware, use:

```bash
git fetch origin
git switch course/full
cd LoRaWAN-HelloWorld-radiolib
pio run -c platformio.course-full.ini
```
