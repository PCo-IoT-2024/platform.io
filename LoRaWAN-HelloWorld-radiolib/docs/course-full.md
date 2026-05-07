# Course Full Integration Branch

Branch: `course/full`

This branch integrates the module branches into one complete water monitoring buoy firmware.

## Sensors and fPorts

| fPort | Payload | Meaning |
|---:|---|---|
| 1 | `lat,lon,alt,hdop` | GPS position |
| 2 | `temperatureC` | DS18B20 water temperature |
| 3 | `ph` | PH4502C pH value |
| 4 | `tds,temperatureC` | Gravity TDS value with compensation temperature |
| 5 | `ntu` | Turbidity value |
| 221 | text message | diagnostic or sensor error |

## Sensor Scheduling

The firmware uses the RTC `bootCount` and rotates through five measurements:

```cpp
currentSensor = (bootCount - 1) % 5;
```

This keeps each wake cycle short and avoids sending all measurements in every LoRaWAN uplink.

## Build

The original `platformio.ini` is left untouched. The full course branch provides a compact build configuration:

```bash
cd LoRaWAN-HelloWorld-radiolib
pio run -c platformio.course-full.ini
```

Upload:

```bash
pio run -c platformio.course-full.ini -t upload
```

## Important Notes

The compact PlatformIO file currently uses the same example TTN credentials and SX1262 pin mapping as the previous course branch. Before using this with students, replace these values with per-device credentials or a documented local credential procedure.

TDS currently uses `TDS_DEFAULT_TEMPERATURE_C=22.0f` for compensation. A later improvement should reuse the last measured DS18B20 temperature from RTC memory or NVS.
