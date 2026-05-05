# Dashboard with mqttcli and SNode.C

**Primary workstream:** Student 5 — dashboard, documentation, and integration test

**Interfaces:** Student 4 provides the running Raspberry Pi backend and MariaDB tables; Students 1–3 provide the fPort and decoded-field contract.

The dashboard is provided by the course version of `mqttcli` from the `mqttcli-mariadb` branch. It is the same process that subscribes to the local MQTT stream and stores values in MariaDB.

The dashboard part is currently under course preparation and will be implemented before the workshop starts. Students will run the provided `mqttcli` binary; they do not need a separate dashboard repository.

For the course, the browser-facing endpoint is:

```text
http://groupN.local:8080/
```

where `groupN` is `group1`, `group2`, `group3`, or `group4`.

## What the dashboard should show

A useful first dashboard should show:

- latest value per scalar sensor
- last update time
- device ID
- fPort for scalar measurements
- latest GPS position
- GPS history table or map link
- pH history
- TDS history
- turbidity history
- water-temperature history
- PH4502C board-temperature history
- simple status information

The first dashboard does not need to be visually perfect. It should prove the full data path.

## Dashboard input data

The dashboard reads from two MariaDB tables:

| Table | Dashboard use |
|---|---|
| `measurements` | scalar sensor values |
| `gps_positions` | GPS position values |

The `measurements` table contains:

```text
id
received_at
application_id
device_id
f_port
value
```

The dashboard must interpret the numeric `value` column through the `f_port` column.

| fPort | Dashboard label |
|---:|---|
| 2 | water temperature °C |
| 3 | pH |
| 4 | TDS ppm |
| 5 | turbidity NTU |
| 6 | PH4502C board temperature °C |

The `gps_positions` table contains:

```text
id
received_at
application_id
device_id
latitude
longitude
altitude
hdop
```

GPS fPort 1 is shown from the `gps_positions` table, not from the scalar `measurements` table.

## Start mqttcli storage/dashboard

Use the provided `mqttcli` binary from the `mqttcli-mariadb` branch. The exact command-line syntax must match the final course implementation.

Documented course intent:

```text
mqttcli subscribes to ttn/# on localhost:1883
mqttcli inserts scalar values into water_buoy.measurements
mqttcli inserts GPS values into water_buoy.gps_positions
mqttcli serves the dashboard on port 8080 at /
```

Before the course starts, replace this placeholder with the exact command printed by the final `mqttcli --help` output:

```bash
~/water-buoy/bin/mqttcli <course-storage-dashboard-options>
```

Do not replace this with guessed syntax. The command must be taken from the implemented `mqttcli-mariadb` version.

## Open the dashboard

From a laptop in the same network:

```text
http://group1.local:8080/
http://group2.local:8080/
http://group3.local:8080/
http://group4.local:8080/
```

If `.local` does not resolve, use the Pi IP address:

```text
http://<raspberry-pi-ip>:8080/
```

## Example queries behind the dashboard

Latest scalar values:

```sql
SELECT device_id, f_port, value, received_at
FROM measurements
ORDER BY received_at DESC
LIMIT 20;
```

TDS history:

```sql
SELECT received_at, value
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

Latest GPS positions:

```sql
SELECT device_id, latitude, longitude, altitude, hdop, received_at
FROM gps_positions
ORDER BY received_at DESC
LIMIT 20;
```

A simple dashboard can show latitude and longitude as numbers. A better version can add an OpenStreetMap link for the latest position.

## Minimum viable dashboard

A minimum dashboard is enough if it proves the full pipeline.

```text
[ ] browser can open http://groupN.local:8080/
[ ] provided mqttcli process can connect to MariaDB
[ ] latest scalar measurements are displayed
[ ] latest GPS position is displayed
[ ] fPort is visible or correctly translated into scalar sensor labels
[ ] at least one historical table or chart is visible
```

## Evidence for the final report

Student 5 should collect:

```text
[ ] screenshot of the dashboard root page
[ ] screenshot showing at least one real scalar sensor value
[ ] screenshot or table showing one GPS position
[ ] matching MariaDB SELECT result from measurements
[ ] matching MariaDB SELECT result from gps_positions
[ ] matching TTN live-data entry
[ ] short explanation of fPort-to-label mapping and GPS table separation
```

## Done when

```text
[ ] Provided mqttcli storage/dashboard process starts on the Raspberry Pi.
[ ] Browser opens http://groupN.local:8080/.
[ ] Dashboard shows scalar values from measurements.
[ ] Dashboard shows GPS values from gps_positions.
[ ] A displayed scalar value can be traced back to fPort, MariaDB row, local MQTT message, TTN uplink, and ESP32 serial output.
[ ] A displayed GPS position can be traced back to gps_positions, local MQTT message, TTN uplink, and ESP32 serial output.
```
