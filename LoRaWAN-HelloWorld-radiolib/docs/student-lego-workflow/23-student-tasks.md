# Student Tasks and Suggested Exercises

This chapter turns the complete buoy workflow into concrete student tasks.

The group consists of five students. Each student owns one workstream, but the final result is shared. The goal is not that every student does everything. The goal is that every student contributes one essential part and understands how it connects to the other parts.

The five workstreams are:

| Student | Responsibility |
|---:|---|
| 1 | ESP32 firmware and LoRaWAN device |
| 2 | Sensors and calibration |
| 3 | TTN cloud setup and payload formatter |
| 4 | Raspberry Pi backend, MQTT, and MariaDB |
| 5 | Dashboard, documentation, and integration test |

Each student should document their own work. The final report then combines these five parts into one coherent system description.

---

## Shared task — Repository and working setup

This task is done by all students, but one student may coordinate it.

Goal: every group member can access the project and understands where the relevant files are.

Steps:

1. Fork or clone the repository as instructed.
2. Use the course branch:

```text
course/lego-configurable
```

3. Locate the firmware project:

```text
LoRaWAN-HelloWorld-radiolib/
```

4. Locate the generator:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

5. Locate the student book:

```text
LoRaWAN-HelloWorld-radiolib/docs/student-lego-workflow/
```

Deliverables:

```text
[ ] repository available locally
[ ] correct branch checked out
[ ] group knows where generator, firmware, and documentation are
[ ] one shared document or notes file exists for group decisions
```

Done when:

```text
Every student can explain where their own workstream starts in the repository or on the Raspberry Pi.
```

---

## Student 1 — ESP32 firmware and LoRaWAN device

Student 1 owns the embedded-device workflow.

Goal: build, flash, and operate the ESP32 firmware so that the device sends uplinks on the expected fPorts.

Main tasks:

1. Generate `platformio.ini` together with Student 2 and Student 3.
2. Copy the generated `platformio.ini` into:

```text
LoRaWAN-HelloWorld-radiolib/platformio.ini
```

3. Build the firmware:

```bash
pio run
```

4. Upload the firmware:

```bash
pio run -t upload
```

5. Open the serial monitor:

```bash
pio device monitor
```

6. Verify startup output.
7. Verify selected sensors.
8. Verify LoRaWAN join or restored session.
9. Verify fPort rotation.
10. Share serial logs with the group.

Expected fPort sequence with all measurement channels enabled:

```text
1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 1 -> ...
```

Deliverables:

```text
[ ] generated platformio.ini committed or attached as evidence
[ ] successful pio run log
[ ] successful upload log
[ ] serial monitor screenshot or text log
[ ] fPort sequence table
[ ] short explanation of one-sensor-per-wake behavior
```

Suggested fPort table:

| Wake | Sensor | fPort | Expected decoded field |
|---:|---|---:|---|
| 1 | GPS | 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | water temperature | 2 | `temperature_c` |
| 3 | pH | 3 | `ph_level` |
| 4 | TDS | 4 | `tds_ppm` |
| 5 | turbidity | 5 | `turbidity_ntu` |
| 6 | PH4502C board temperature | 6 | `ph_board_temperature_c` |

Interfaces to coordinate:

| Needed from | Information |
|---|---|
| Student 2 | sensor pins and calibration values |
| Student 3 | TTN credentials and LoRaWAN version |
| Student 4 | fPort/field expectations for storage |
| Student 5 | evidence for final report and dashboard trace |

Done when:

```text
The ESP32 sends real uplinks and the serial monitor output can be matched to TTN live data.
```

---

## Student 2 — Sensors and calibration

Student 2 owns the physical measurement workflow.

Goal: wire the sensors, record calibration values, and explain the reliability of the measurements.

Main tasks:

1. Prepare a wiring table.
2. Check sensor supply voltages.
3. Check ESP32 pin mapping.
4. Use calibration buttons for analog sensors.
5. Record raw ADC values from serial output.
6. Enter calibration values into the generator.
7. Verify plausible values after flashing.
8. Document limitations of each sensor.

Important rule:

```text
All analog calibration values are raw ESP32 ADC values, not voltages.
```

Required wiring table:

| Sensor | ESP32 pin | Sensor-side label | Notes |
|---|---:|---|---|
| GPS RX pin | | TX | GPS TX goes to ESP32 RX |
| GPS TX pin | | RX | GPS RX goes to ESP32 TX |
| OneWire data pin | | T2 | DS18B20 data |
| pH analog pin | | P0 | analog pH output |
| pH board temperature pin | | T1 | PH4502C onboard temperature |
| TDS analog pin | | A | analog TDS output |
| Turbidity analog pin | | A | analog turbidity output |

Calibration tasks:

### pH calibration

Use pH buffer liquids if available.

Record:

| Reference | Raw ADC |
|---|---:|
| pH 4 | |
| pH 7 | |
| pH 10 | |

### TDS calibration

Use reference solution if available. If no certified liquid is available, document the improvised reference and its uncertainty.

Record:

| Reference | Raw ADC | Known/assigned ppm |
|---|---:|---:|
| low TDS | | |
| high TDS | | |

### Turbidity calibration

Use clear water and a known or assigned turbid sample if no certified NTU solution is available.

Record:

| Reference | Raw ADC | Known/assigned NTU |
|---|---:|---:|
| clear water | | |
| turbid sample | | |

### PH4502C board temperature calibration

Record or verify:

| Reference | Raw ADC | Known °C |
|---|---:|---:|
| low temperature | | |
| high temperature | | |

Deliverables:

```text
[ ] complete wiring table
[ ] pH calibration table
[ ] TDS calibration table
[ ] turbidity calibration table
[ ] PH4502C board-temperature calibration table
[ ] short explanation of which values are reliable and which are mainly trend values
[ ] notes about sensor handling and observed drift
```

Interfaces to coordinate:

| Needed by | Information |
|---|---|
| Student 1 | pins and calibration values for generator/firmware |
| Student 3 | expected field names and fPorts |
| Student 4 | database field meanings |
| Student 5 | measurement-quality discussion for final report |

Done when:

```text
The group has documented calibration values and the ESP32 reports plausible calibrated measurements.
```

---

## Student 3 — TTN cloud setup and payload formatter

Student 3 owns the cloud and LoRaWAN application setup.

Goal: make TTN receive and decode the ESP32 uplinks and provide MQTT access for the Raspberry Pi backend.

Main tasks:

1. Create or use the TTN application.
2. Create the end device.
3. Configure the correct LoRaWAN version.
4. Configure the correct region, usually EU868 for the course.
5. Provide DevEUI, AppKey, and NwkKey if needed.
6. Install the generated `payload-formatter.js`.
7. Verify TTN live data.
8. Create an MQTT API key for the Raspberry Pi backend.
9. Document TTN MQTT connection information.

Required TTN information table:

| Value | Entry |
|---|---|
| application ID | |
| device ID | |
| DevEUI | |
| JoinEUI | `0000000000000000` |
| LoRaWAN version | |
| region / frequency plan | |
| AppKey stored securely | yes/no |
| NwkKey stored securely, if used | yes/no |
| MQTT server | |
| MQTT username | |
| MQTT uplink topic | |

Do not put real AppKey, NwkKey, or API key values into public reports or screenshots.

Payload formatter verification:

| fPort | Expected field |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

Deliverables:

```text
[ ] TTN application screenshot
[ ] TTN end-device screenshot without visible secrets
[ ] TTN live data screenshot with decoded payload
[ ] installed payload-formatter.js noted
[ ] MQTT API key created and stored securely
[ ] MQTT topic and connection information shared with Student 4
[ ] short explanation of DevEUI, AppKey, NwkKey, and JoinEUI
```

Interfaces to coordinate:

| Needed by | Information |
|---|---|
| Student 1 | credentials, LoRaWAN version, region |
| Student 2 | fPort meanings and decoded fields |
| Student 4 | MQTT access data and topic pattern |
| Student 5 | screenshots and cloud setup evidence |

Done when:

```text
TTN live data shows decoded payloads from the buoy and Student 4 can use the TTN MQTT integration information.
```

---

## Student 4 — Raspberry Pi backend, MQTT, and MariaDB

Student 4 owns the local backend pipeline.

Goal: receive TTN MQTT messages on the Raspberry Pi, republish them locally, and store measurements in MariaDB.

Main tasks:

1. Prepare Raspberry Pi OS.
2. Enable and test SSH access.
3. Install build tools and MariaDB.
4. Create the `water_buoy` database and user.
5. Clone and build SNode.C.
6. Clone and build MQTTSuite branch:

```text
mqttcli-mariadb
```

7. Run `mqttbroker` locally.
8. Test local publish/subscribe with `mqttcli`.
9. Configure and run `mqttbridge` from TTN MQTT to local MQTT.
10. Configure `mqttcli` storage into MariaDB.
11. Verify database rows.

Backend path:

```text
TTN MQTT -> mqttbridge -> local mqttbroker -> mqttcli -> MariaDB
```

Required backend evidence:

```text
[ ] SSH login to Raspberry Pi works
[ ] MariaDB status output
[ ] database and user created
[ ] SNode.C build output
[ ] MQTTSuite build output
[ ] mqttbroker running
[ ] local mqttcli publish/subscribe test
[ ] TTN uplink visible on local MQTT topic
[ ] MariaDB SELECT query showing measurements
```

Suggested SQL verification:

```sql
SELECT id, received_at, device_id, f_port, field_name, numeric_value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

Interfaces to coordinate:

| Needed from | Information |
|---|---|
| Student 3 | TTN MQTT server, username, API key, topic pattern |
| Student 1 | fPort sequence and device ID |
| Student 2 | meaning of fields and calibration notes |
| Student 5 | dashboard query/API needs |

Done when:

```text
A real TTN uplink creates at least one measurement row in MariaDB on the Raspberry Pi.
```

---

## Student 5 — Dashboard, documentation, and integration test

Student 5 owns the visible result and the final integration story.

Goal: show stored buoy data in a browser dashboard and coordinate the proof that the complete system works.

Main tasks:

1. Define which values the dashboard should show.
2. Coordinate with Student 4 about database fields.
3. Run or adapt the SNode.C dashboard server.
4. Verify that the dashboard can query MariaDB.
5. Show latest measurements.
6. Show at least one historical table or chart.
7. Collect screenshots and logs from all students.
8. Coordinate the field-test evidence.
9. Assemble the final report structure.

Minimum dashboard content:

```text
[ ] device ID
[ ] last update time
[ ] latest pH
[ ] latest TDS
[ ] latest turbidity
[ ] latest water temperature
[ ] latest PH4502C board temperature
[ ] GPS position if available
[ ] one history table or chart
```

Integration trace:

Student 5 should trace at least one value from physical measurement to dashboard:

```text
sensor reading
  -> ESP32 serial output
  -> TTN live data
  -> local MQTT message
  -> MariaDB row
  -> dashboard display
```

Deliverables:

```text
[ ] dashboard screenshot
[ ] API or backend endpoint notes
[ ] database query used by dashboard
[ ] end-to-end trace of one value
[ ] final field-test evidence collection
[ ] final report assembled from all workstreams
```

Interfaces to coordinate:

| Needed from | Information |
|---|---|
| Student 1 | firmware/fPort evidence |
| Student 2 | calibration and measurement limitations |
| Student 3 | TTN decoded payload screenshots |
| Student 4 | database schema and query access |

Done when:

```text
The dashboard shows recent measurement data and the group can prove where the displayed value came from.
```

---

## Shared field-test task

All students participate in the real water measurement.

Goal: test the complete system with real lake water.

Before the field test:

```text
[ ] battery charged
[ ] electronics protected from water
[ ] sensors mounted safely
[ ] TTN live data works
[ ] Raspberry Pi backend works
[ ] dashboard works
[ ] calibration state documented
```

During the field test, record:

```text
location
date and time
weather
water appearance
sensor depth
GPS behavior
TTN reception
dashboard behavior
observed problems
```

Deliverable:

```text
field-test context log plus screenshots from TTN, MariaDB, and dashboard
```

Done when:

```text
At least one real lake-water measurement is visible in the final dashboard and stored in MariaDB.
```

---

## Shared final report task

The final report is a group document. Every student contributes their own section.

Suggested final report structure:

```text
1. Introduction and project goal
2. Group organization and responsibilities
3. System architecture
4. Hardware and sensor setup
5. Firmware and generator configuration
6. TTN cloud setup
7. Raspberry Pi backend
8. Database and dashboard
9. Calibration
10. Field measurement
11. Results
12. Limitations
13. Improvements
14. Conclusion
```

Required final evidence:

```text
[ ] photo or diagram of setup
[ ] pin mapping table
[ ] calibration tables
[ ] generated configuration notes
[ ] TTN decoded payload screenshot
[ ] local MQTT evidence
[ ] MariaDB query result
[ ] dashboard screenshot
[ ] field-test context log
[ ] explanation of limitations
```

A good final report does not only state that the system worked. It explains what was measured, how the data moved through the system, how reliable the values are, and what should be improved.

---

## Optional advanced tasks

Groups that finish early can extend the system.

Possible extensions:

- create a better power-domain diagram
- measure ESP32 active and sleep current
- build a simple solar/battery prototype
- add CSV export from the dashboard
- add a GPS map view
- add a multi-device selector
- improve MariaDB schema
- add calibration metadata to the database
- create systemd services for all backend processes
- write a troubleshooting guide from the group experience

Optional advanced deliverable:

```text
short appendix explaining what was extended, why it is useful, and how it was tested
```
