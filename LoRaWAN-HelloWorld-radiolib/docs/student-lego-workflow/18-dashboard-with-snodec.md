# Dashboard with mqttcli and SNode.C

**Primary workstream:** Student 5 — dashboard, documentation, and integration test

**Interfaces:** Student 4 provides the running Raspberry Pi backend and MariaDB table; Students 1–3 provide the fPort and decoded-field contract.

The dashboard is served by `mqttcli` on the Raspberry Pi. It uses the SNode.C/MQTTSuite backend code and listens on port 8080.

The target path is:

```text
MariaDB measurements table
  -> mqttcli dashboard mode
  -> HTTP server on port 8080
  -> browser
```

There is no separate dashboard repository for this course setup.

## What the dashboard should show

A useful first dashboard should show:

- latest value per sensor
- last update time
- device ID
- fPort
- pH history
- TDS history
- turbidity history
- water-temperature history
- PH4502C board-temperature history
- simple status information

The first dashboard does not need to be visually perfect. It should prove the full data path.

## Start the dashboard

The course uses `mqttcli` as the dashboard process.

Command pattern:

```bash
~/water-buoy/bin/mqttcli \
  dashboard \
  --http-port 8080 \
  --mariadb-host localhost \
  --mariadb-database water_buoy \
  --mariadb-user water_buoy \
  --mariadb-password 'water-buoy-pass' \
  --table measurements
```

If your local `mqttcli --help` prints different option names, keep the same values:

```text
HTTP port: 8080
MariaDB host: localhost
MariaDB database: water_buoy
MariaDB user: water_buoy
MariaDB table: measurements
```

The root endpoint is:

```text
/
```

## Open the dashboard

From a laptop in the same network, open the group Pi address.

Examples:

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

## Frontend, backend, database

For this course setup:

| Part | Course component |
|---|---|
| frontend | browser page served by mqttcli/SNode.C |
| backend | mqttcli dashboard process |
| database | MariaDB `water_buoy.measurements` table |

The browser does not connect directly to MariaDB. The dashboard process reads from MariaDB and serves the page through HTTP.

## How dashboard values are interpreted

The dashboard must interpret the numeric `value` column through the `f_port` column.

| fPort | Dashboard label |
|---:|---|
| 2 | water temperature °C |
| 3 | pH |
| 4 | TDS ppm |
| 5 | turbidity NTU |
| 6 | PH4502C board temperature °C |

Example SQL query for latest values:

```sql
SELECT device_id, f_port, value, received_at
FROM measurements
ORDER BY received_at DESC
LIMIT 20;
```

Example SQL query for TDS history:

```sql
SELECT received_at, value
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

## Minimum viable dashboard

A minimum dashboard is enough if it proves the full pipeline.

```text
[ ] browser can open http://groupN.local:8080/
[ ] dashboard process can connect to MariaDB
[ ] latest measurements are displayed
[ ] fPort is visible or correctly translated into sensor labels
[ ] at least one historical table or chart is visible
```

## Evidence for the final report

Student 5 should collect:

```text
[ ] screenshot of the dashboard root page
[ ] screenshot showing at least one real sensor value
[ ] matching MariaDB SELECT result
[ ] matching TTN live-data entry
[ ] short explanation of fPort-to-label mapping
```

## Done when

```text
[ ] mqttcli dashboard starts on port 8080.
[ ] Browser opens http://groupN.local:8080/.
[ ] Dashboard shows values from MariaDB.
[ ] A displayed value can be traced back to fPort, MariaDB row, local MQTT message, TTN uplink, and ESP32 serial output.
```
