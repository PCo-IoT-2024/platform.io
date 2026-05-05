# Group Work Organization

The buoy project is designed for groups of five students.

Each student should own one clearly defined workstream. The workstreams are independent enough that each student can make visible progress, but they are not isolated. The final result only works when all five parts are integrated.

This is intentional. A real IoT system is not built by one isolated specialist. It is a system-integration project involving hardware, firmware, cloud services, backend infrastructure, databases, dashboards, testing, and documentation.

---

## Recommended group structure

| Student | Workstream | Main result |
|---:|---|---|
| 1 | ESP32 firmware and LoRaWAN device | firmware builds, flashes, joins TTN, sends correct fPorts |
| 2 | Sensors and calibration | sensors wired, calibrated, documented, plausible values measured |
| 3 | TTN cloud setup and payload formatter | TTN application/device works, formatter decodes uplinks, MQTT access prepared |
| 4 | Raspberry Pi backend, MQTT, and MariaDB | Pi runs broker, bridge, storage process, database receives rows |
| 5 | Dashboard, documentation, and integration test | dashboard shows data, final test path is documented, report is assembled |

The group should not interpret this table as five isolated mini-projects. The interfaces between workstreams are the most important part.

---

## Workstream 1 — ESP32 firmware and LoRaWAN device

This student owns the embedded firmware workflow.

Main responsibilities:

- generate `platformio.ini`
- build the firmware with PlatformIO
- flash the ESP32
- verify serial output
- verify LoRa radio initialization
- verify OTAA join or session restore
- verify fPort rotation
- coordinate firmware settings with the TTN and sensor students

Important files and tools:

```text
tools/platformio-ini-generator/www/index.html
platformio.ini
pio run
pio run -t upload
pio device monitor
```

Deliverables:

```text
[ ] generated platformio.ini
[ ] successful build log
[ ] successful upload
[ ] serial monitor screenshot or log
[ ] fPort sequence documentation
```

Done when:

```text
The ESP32 sends uplinks on the expected fPorts and TTN receives them.
```

Interfaces to other students:

| Interface | Partner |
|---|---|
| sensor pins and enabled sensors | Student 2 |
| DevEUI, AppKey, NwkKey, LoRaWAN version | Student 3 |
| fPort mapping and payload fields | Students 3, 4, 5 |

---

## Workstream 2 — Sensors and calibration

This student owns the measurement side.

Main responsibilities:

- wire the sensors correctly
- check sensor supply voltages
- document ESP32 pin mapping
- enter calibration mode for analog sensors
- record raw ADC calibration values
- enter calibration values into the generator
- explain measurement limitations

Sensors:

| Sensor | Main value |
|---|---|
| GPS | position |
| DS18B20 | water temperature |
| PH4502C pH | pH value |
| PH4502C board temperature | board/onboard temperature |
| Gravity TDS | TDS ppm |
| turbidity sensor | turbidity NTU |

Important rule:

```text
All analog calibration values are raw ESP32 ADC values, not voltages.
```

Deliverables:

```text
[ ] wiring table
[ ] calibration table
[ ] raw ADC values
[ ] notes about reference liquids or reference conditions
[ ] short discussion of sensor reliability
```

Done when:

```text
The firmware produces plausible calibrated values for all connected sensors.
```

Interfaces to other students:

| Interface | Partner |
|---|---|
| pin mapping | Student 1 |
| calibration values in generator | Student 1 |
| decoded field interpretation | Students 3 and 5 |
| database field names | Student 4 |

---

## Workstream 3 — TTN cloud setup and payload formatter

This student owns the LoRaWAN cloud side.

Main responsibilities:

- create TTN application
- create TTN end device
- configure LoRaWAN version and region
- provide DevEUI, AppKey, and NwkKey if needed
- install generated payload formatter
- verify TTN live data
- create MQTT API key for backend access
- document TTN MQTT connection information

Important TTN values:

```text
application ID
device ID
DevEUI
JoinEUI
AppKey
NwkKey, if LoRaWAN 1.1.0
region / frequency plan
MQTT server
MQTT username
MQTT API key
uplink topic pattern
```

Do not publish real keys.

Deliverables:

```text
[ ] TTN application exists
[ ] TTN end device exists
[ ] payload formatter installed
[ ] live data screenshot with decoded payload
[ ] MQTT API key created and stored securely
[ ] TTN MQTT topic information documented
```

Done when:

```text
TTN live data shows decoded fields for the buoy uplinks and the backend team has the MQTT access information.
```

Interfaces to other students:

| Interface | Partner |
|---|---|
| LoRaWAN credentials | Student 1 |
| payload fields | Students 2, 4, 5 |
| MQTT API key and topic | Student 4 |
| screenshots/results | Student 5 |

---

## Workstream 4 — Raspberry Pi backend, MQTT, and MariaDB

This student owns the local backend infrastructure.

Main responsibilities:

- install and prepare Raspberry Pi OS
- install build tools and MariaDB
- build SNode.C
- build MQTTSuite branch `mqttcli-mariadb`
- run local `mqttbroker`
- configure and run `mqttbridge`
- subscribe to local MQTT messages
- store measurements in MariaDB using `mqttcli`
- document commands and configuration

Target data path:

```text
TTN MQTT -> mqttbridge -> local mqttbroker -> mqttcli -> MariaDB
```

Deliverables:

```text
[ ] Raspberry Pi reachable by SSH
[ ] MariaDB running
[ ] SNode.C builds
[ ] MQTTSuite builds on branch mqttcli-mariadb
[ ] mqttbroker runs locally
[ ] mqttbridge forwards TTN messages
[ ] mqttcli stores measurements in MariaDB
[ ] SELECT query shows recent rows
```

Done when:

```text
A real TTN uplink creates a measurement row in MariaDB on the Raspberry Pi.
```

Interfaces to other students:

| Interface | Partner |
|---|---|
| TTN MQTT credentials | Student 3 |
| field names and fPorts | Students 1, 2, 3 |
| database schema/API needs | Student 5 |
| final integration test | all students |

---

## Workstream 5 — Dashboard, documentation, and integration test

This student owns the final user-visible result and the integration story.

Main responsibilities:

- define what the dashboard should show
- run or adapt the SNode.C dashboard backend
- connect dashboard/backend to MariaDB
- display latest measurements
- display at least one history view
- collect screenshots and logs
- coordinate final end-to-end test
- assemble the final report

Target user path:

```text
browser -> SNode.C dashboard -> MariaDB -> stored measurements
```

Deliverables:

```text
[ ] dashboard opens in browser
[ ] dashboard shows latest values
[ ] dashboard shows historical values or table
[ ] dashboard can be traced back to database rows
[ ] final integration checklist completed
[ ] final report assembled
```

Done when:

```text
A value measured by the buoy appears in the browser dashboard and can be traced back through MariaDB, MQTT, TTN, and the ESP32 serial output.
```

Interfaces to other students:

| Interface | Partner |
|---|---|
| database schema and queries | Student 4 |
| decoded field meanings | Students 2 and 3 |
| fPort mapping | Student 1 |
| screenshots and test evidence | all students |

---

## Required interface contracts

Each group must agree on these contracts early.

### fPort contract

| fPort | Meaning | Field name |
|---:|---|---|
| 1 | GPS | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | water temperature | `temperature_c` |
| 3 | pH | `ph_level` |
| 4 | TDS | `tds_ppm` |
| 5 | turbidity | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

### Pin mapping contract

The group must maintain one table containing:

```text
sensor
ESP32 pin
sensor-side label
notes
```

### MQTT topic contract

The group must define the local topic convention, for example:

```text
ttn/<application-id>/<device-id>/up
```

### Database contract

The group must agree on:

```text
database name
table names
field names
timestamp format
device identifier
which decoded fields are stored
```

### Dashboard contract

The group must agree on:

```text
which values are shown
which time range is shown
which device is selected
which API endpoints are needed
```

---

## Suggested group workflow

### Phase 1 — Parallel setup

| Student | Task |
|---:|---|
| 1 | prepare PlatformIO firmware build |
| 2 | wire and check sensors |
| 3 | create TTN application/device |
| 4 | prepare Raspberry Pi and MariaDB |
| 5 | sketch dashboard and report structure |

### Phase 2 — First integration

```text
Student 1 + Student 3: ESP32 joins TTN
Student 1 + Student 2: sensor readings are plausible
Student 3 + Student 4: TTN MQTT messages arrive on Pi
Student 4 + Student 5: database fields match dashboard needs
```

### Phase 3 — End-to-end test

All students participate.

Trace one value:

```text
sensor -> ESP32 serial output -> TTN live data -> local MQTT -> MariaDB -> dashboard
```

### Phase 4 — Final report

The final report should not be written by one person only. Each student contributes their own section, and Student 5 coordinates the final document.

---

## Coordination meetings

A good lightweight rhythm is:

| Time | Purpose |
|---|---|
| start of session | agree goals and interfaces |
| middle of session | check blockers |
| end of session | record what works and what is missing |

Use a shared checklist. The final system has many small parts, and it is easy to lose track.

---

## Final group success criterion

The group is successful when all students can explain the complete system at a high level, even if each student implemented only one workstream in depth.

Minimum final demonstration:

```text
A real sensor value is measured by the ESP32 buoy, decoded in TTN, bridged to the Raspberry Pi, stored in MariaDB, and shown in the dashboard.
```
