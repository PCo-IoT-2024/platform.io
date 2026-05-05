# Student Tasks and Deliverables

This chapter summarizes the individual and group deliverables. It should be used as a checklist while working through the book.

The project is completed by groups of five students. Each student owns one workstream, but the final result must be integrated as one buoy system.

## Workstream deliverables

| Student | Workstream | Main deliverable |
|---:|---|---|
| 1 | ESP32 firmware and LoRaWAN device | firmware builds, flashes, joins TTN, sends expected fPorts |
| 2 | sensors and calibration | wiring, raw ADC calibration values, plausible sensor readings |
| 3 | TTN cloud and payload formatter | TTN application/device, formatter, MQTT API key |
| 4 | Raspberry Pi backend, MQTT, MariaDB | broker, bridge, provided mqttcli storage/dashboard, database rows |
| 5 | dashboard, documentation, final test | dashboard on port 8080, GPS/scalar evidence, final report |

## Task 1 — Repository and branch setup

Goal: create a reproducible working state.

Steps:

1. Fork or clone the repository as instructed.
2. Use the course branch:

```text
course/lego-configurable
```

3. Document the local working directory.
4. Do not commit secrets such as AppKey, NwkKey, TTN API keys, or database passwords.

Deliverable:

```text
screenshot or terminal log showing repository, branch, and working directory
```

## Task 2 — Group work contract

Goal: assign responsibilities before technical work begins.

Steps:

1. Assign the five workstreams.
2. Agree on group hostname: `group1`, `group2`, `group3`, or `group4`.
3. Agree on TTN application ID and device ID.
4. Agree on local MQTT topic convention.
5. Agree on who records final evidence.

Deliverable:

```text
group table with names, workstreams, interfaces, and responsibilities
```

## Task 3 — TTN cloud setup

Owner: Student 3.

Goal: prepare the cloud side.

Steps:

1. Create TTN application.
2. Create TTN end device.
3. Select correct region and LoRaWAN version.
4. Create or record DevEUI, JoinEUI, AppKey, and NwkKey if needed.
5. Install generated payload formatter.
6. Create TTN MQTT API key.
7. Record TTN MQTT host, username, and uplink topic.

Course values:

```text
TTN MQTT host: eu1.cloud.thethings.network
username format: <application-id>@ttn
uplink topic: v3/<application-id>@ttn/devices/+/up
```

Deliverable:

```text
TTN live-data screenshot with decoded payload and separate secure record of MQTT credentials
```

## Task 4 — Generator, firmware, and ESP32 flashing

Owner: Student 1, with inputs from Students 2 and 3.

Goal: create a working ESP32 firmware configuration.

Steps:

1. Open the generator.
2. Enter TTN credentials.
3. Select radio module, LoRaWAN version, and region.
4. Enter sensor pins.
5. Enter calibration values from Student 2.
6. Generate `platformio.ini`.
7. Generate `payload-formatter.js`.
8. Build and flash:

```bash
pio run
pio run -t upload
pio device monitor
```

Deliverable:

```text
platformio.ini, payload formatter, successful build/upload log, serial monitor output
```

## Task 5 — Sensor wiring and calibration

Owner: Student 2.

Goal: make the measured values plausible.

Steps:

1. Wire all sensors.
2. Document ESP32 pin mapping.
3. Enter calibration mode for analog sensors.
4. Record raw ADC values.
5. Enter raw ADC calibration values into the generator.
6. Verify normal measurement values after reflashing.

Important rule:

```text
Use raw ESP32 ADC values for analog calibration, not voltages.
```

Deliverable:

```text
wiring table, calibration table, raw ADC values, notes about references and plausibility
```

## Task 6 — Verify fPort rotation and decoded payloads

Owners: Students 1 and 3.

Goal: prove that firmware and TTN formatter match.

Expected fPort mapping:

| fPort | Meaning | Field |
|---:|---|---|
| 1 | GPS | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | water temperature | `temperature_c` |
| 3 | pH | `ph_level` |
| 4 | TDS | `tds_ppm` |
| 5 | turbidity | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

Deliverable:

```text
table with wake number, fPort, sensor, TTN decoded field, and example value
```

## Task 7 — Raspberry Pi preparation

Owner: Student 4.

Goal: prepare the backend computer.

Steps:

1. Install Raspberry Pi OS Lite Bookworm.
2. Set user to `water`.
3. Set hostname to group number, for example `group1`.
4. Enable SSH.
5. Install baseline packages.
6. Create `~/water-buoy`.
7. Install and start MariaDB.
8. Create database `water_buoy`, user `water_buoy`, table `measurements`, and table `gps_positions`.

Deliverable:

```text
SSH proof, package installation proof, MariaDB DESCRIBE output for measurements and gps_positions
```

## Task 8 — Build SNode.C and MQTTSuite

Owner: Student 4.

Goal: build the local backend tools.

Repositories:

```text
SNode.C:    https://github.com/SNodeC/snode.c      branch master
MQTTSuite:  https://github.com/SNodeC/mqttsuite    branch mqttcli-mariadb
```

Build style:

```text
sibling build directory beside each cloned source directory
```

Deliverable:

```text
successful build logs and working mqttbroker, mqttbridge, mqttcli binaries
```

## Task 9 — Local MQTT broker and TTN bridge

Owner: Student 4, with credentials from Student 3.

Goal: forward TTN uplinks to local MQTT.

Steps:

1. Start local `mqttbroker` on port 1883.
2. Test local publish/subscribe with `mqttcli`.
3. Create `bridge-config.json`.
4. Make sure the TTN broker entry subscribes to the TTN uplink topic.
5. Make sure the local broker entry does not subscribe to `#`.
6. Start `mqttbridge` with `bridge --definition`.
7. Subscribe to local `ttn/#` topics.
8. Wait for ESP32 uplink.

Deliverable:

```text
terminal output showing TTN uplink arriving on local MQTT and bridge-config.json without local '#' subscription
```

## Task 10 — Store measurements, GPS, and serve dashboard with mqttcli

Owner: Student 4 starts the process; Student 5 verifies the browser view.

Goal: use the provided `mqttcli` from `mqttcli-mariadb` as the single storage/dashboard process.

Steps:

1. Start the provided `mqttcli` storage/dashboard process.
2. It subscribes to local `ttn/#` messages.
3. It inserts scalar numeric values into `measurements`.
4. It inserts GPS positions into `gps_positions`.
5. It serves the dashboard on port 8080.
6. Query both database tables.
7. Open the dashboard.

Expected `measurements` table columns:

```text
id, received_at, application_id, device_id, f_port, value
```

Expected `gps_positions` table columns:

```text
id, received_at, application_id, device_id, latitude, longitude, altitude, hdop
```

Dashboard URL:

```text
http://groupN.local:8080/
```

Deliverable:

```text
SELECT output showing scalar sensor rows, SELECT output showing GPS rows, plus dashboard screenshot from port 8080
```

## Task 11 — Final end-to-end test

Owner: all students, coordinated by Student 5.

Trace one scalar value through the complete chain:

```text
sensor
  -> ESP32 serial output
  -> TTN live data
  -> local MQTT on Raspberry Pi
  -> MariaDB measurements row
  -> dashboard display
```

Also trace one GPS position:

```text
GPS
  -> ESP32 serial output
  -> TTN live data
  -> local MQTT on Raspberry Pi
  -> MariaDB gps_positions row
  -> dashboard display or GPS table
```

Deliverable:

```text
one documented scalar trace and one documented GPS trace through all layers
```

## Task 12 — Real water measurement at the lake

Owner: all students.

Goal: perform a real measurement with context.

Steps:

1. Charge battery.
2. Check waterproofing and wiring.
3. Start Pi backend.
4. Start ESP32 buoy.
5. Wait for full fPort cycle.
6. Put sensors into lake water.
7. Record environmental context.
8. Verify dashboard update for scalar measurements and GPS.

Deliverable:

```text
field context log, dashboard screenshot, database rows from measurements and gps_positions, and measurement discussion
```

## Task 13 — Final report and presentation

Owner: Student 5 coordinates, all students contribute.

The final report should include:

```text
1. Introduction and project goal
2. Group organization and responsibilities
3. End-to-end architecture
4. Hardware and sensors
5. Firmware and generator configuration
6. TTN setup
7. Raspberry Pi backend
8. MQTT bridge and MariaDB storage
9. Dashboard
10. Calibration
11. Field measurement
12. Results
13. Limitations
14. Improvements
15. Conclusion
```

Each student contributes the section that belongs to their workstream.

## Minimum success criterion

The group succeeds when it can demonstrate and explain:

```text
A real sensor value and a GPS position are measured by the ESP32 buoy, decoded in TTN, bridged to the Raspberry Pi, stored in MariaDB, and shown in the dashboard.
```

The group should also explain the limitations of the measurement and the reliability risks of the system.
