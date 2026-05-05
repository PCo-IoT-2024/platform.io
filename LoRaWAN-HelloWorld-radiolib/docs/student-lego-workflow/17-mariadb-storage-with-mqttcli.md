# Storing MQTT Data in MariaDB with mqttcli

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides decoded TTN payloads; Student 5 uses the same `mqttcli` process for the web dashboard and final report evidence.

This chapter defines how the provided `mqttcli` from the `mqttcli-mariadb` branch stores measurement values in MariaDB.

The course model is:

```text
local MQTT topic ttn/#
  -> provided mqttcli from mqttcli-mariadb
  -> MariaDB tables measurements and gps_positions
  -> mqttcli dashboard on port 8080
```

Important: `mqttcli` is the course process for both database storage and the web dashboard. The dashboard part is part of the course version of `mqttcli` and will be available before the course starts.

## Database model

The database uses two tables:

| Table | Purpose |
|---|---|
| `measurements` | scalar numeric sensor values with one value per row |
| `gps_positions` | GPS position records with latitude, longitude, altitude, and HDOP |

This keeps the schema simple. Most sensors produce one number per uplink. GPS produces several values and therefore gets its own table.

## Scalar sensor table: measurements

The `measurements` table stores one numeric value per row and keeps `f_port` so the meaning of the value remains clear.

```text
device_id + f_port + value + received_at
```

The meaning of `value` comes from `f_port`.

| fPort | Stored numeric value |
|---:|---|
| 2 | water temperature in °C |
| 3 | pH |
| 4 | TDS in ppm |
| 5 | turbidity in NTU |
| 6 | PH4502C board temperature in °C |

Create the table:

```bash
mariadb -u water_buoy -p water_buoy
```

```sql
CREATE TABLE IF NOT EXISTS measurements (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128) NOT NULL,
  device_id VARCHAR(128) NOT NULL,
  f_port INT NOT NULL,
  value DOUBLE NOT NULL
);

CREATE INDEX idx_measurements_device_time
ON measurements (device_id, received_at);

CREATE INDEX idx_measurements_fport_time
ON measurements (f_port, received_at);
```

## GPS table: gps_positions

GPS fPort 1 contains multiple values. It is stored in a separate table.

| Column | Meaning |
|---|---|
| `latitude` | GPS latitude in decimal degrees |
| `longitude` | GPS longitude in decimal degrees |
| `altitude` | GPS altitude, if available |
| `hdop` | horizontal dilution of precision |

Create the table:

```sql
CREATE TABLE IF NOT EXISTS gps_positions (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128) NOT NULL,
  device_id VARCHAR(128) NOT NULL,
  latitude DOUBLE NOT NULL,
  longitude DOUBLE NOT NULL,
  altitude DOUBLE,
  hdop DOUBLE
);

CREATE INDEX idx_gps_positions_device_time
ON gps_positions (device_id, received_at);
```

Check both tables:

```sql
DESCRIBE measurements;
DESCRIBE gps_positions;
EXIT;
```

## Required mqttcli behavior

The provided `mqttcli` storage/dashboard process must do this for each bridged TTN uplink:

```text
1. subscribe to local MQTT topic ttn/#
2. parse the TTN uplink JSON
3. read application ID
4. read device ID
5. read fPort
6. for fPort 1, insert one row into gps_positions
7. for fPorts 2..6, insert one row into measurements
8. serve the dashboard view on port 8080
```

The stored values must be decoded measurement values, not ESP32 raw ADC values and not full JSON blobs.

Examples for `measurements`:

| Decoded TTN field | fPort | Stored table | Stored value |
|---|---:|---|---:|
| `temperature_c` | 2 | `measurements` | `21.75` |
| `ph_level` | 3 | `measurements` | `7.12` |
| `tds_ppm` | 4 | `measurements` | `350.0` |
| `turbidity_ntu` | 5 | `measurements` | `42.0` |
| `ph_board_temperature_c` | 6 | `measurements` | `26.0` |

Example for `gps_positions`:

| Decoded TTN fields | fPort | Stored table |
|---|---:|---|
| `latitude`, `longitude`, `altitude`, `hdop` | 1 | `gps_positions` |

## Start mqttcli storage/dashboard

Use the provided `mqttcli` binary from the `mqttcli-mariadb` branch. The exact command-line syntax must match the course implementation available before the workshop.

Documented course intent:

```text
mqttcli subscribes to ttn/# on localhost:1883
mqttcli inserts GPS values into water_buoy.gps_positions
mqttcli inserts scalar sensor values into water_buoy.measurements
mqttcli serves the dashboard on port 8080 at /
```

Before the course starts, replace this placeholder with the exact command printed by the final `mqttcli --help` output:

```bash
~/water-buoy/bin/mqttcli <course-storage-dashboard-options>
```

Do not replace this with guessed syntax. The command must be taken from the implemented `mqttcli-mariadb` version.

## Verify local MQTT input

Before testing database insertion, first prove that the local broker receives TTN messages:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

If no JSON messages arrive here, MariaDB storage cannot work yet. Go back to the MQTTBridge chapter.

## Verify stored scalar measurements

After the provided `mqttcli` storage/dashboard process is running and a scalar uplink arrives, check:

```bash
mariadb -u water_buoy -p water_buoy
```

```sql
SELECT id, received_at, application_id, device_id, f_port, value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

Expected example:

```text
id | received_at          | application_id    | device_id | f_port | value
12 | 2026-05-05 12:10:00  | water-buoy-group1 | buoy-01   | 4      | 350
```

## Verify stored GPS positions

After a GPS uplink arrives, check:

```sql
SELECT id, received_at, application_id, device_id, latitude, longitude, altitude, hdop
FROM gps_positions
ORDER BY id DESC
LIMIT 20;
```

Expected example:

```text
id | received_at          | application_id    | device_id | latitude  | longitude | altitude | hdop
 5 | 2026-05-05 12:12:00  | water-buoy-group1 | buoy-01   | 48.28649  | 14.29902  | 266.5    | 2.7
```

## Query by sensor type

TDS history:

```sql
SELECT received_at, value AS tds_ppm
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

pH history:

```sql
SELECT received_at, value AS ph_level
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 3
ORDER BY received_at DESC
LIMIT 100;
```

GPS history:

```sql
SELECT received_at, latitude, longitude, altitude, hdop
FROM gps_positions
WHERE device_id = 'buoy-01'
ORDER BY received_at DESC
LIMIT 100;
```

## Done when

```text
[ ] mqttcli shows bridged TTN messages on local topic ttn/#.
[ ] The provided mqttcli storage/dashboard process runs.
[ ] The measurements table receives numeric scalar sensor values.
[ ] Each measurements row contains application_id, device_id, f_port, and value.
[ ] The gps_positions table receives GPS rows for fPort 1.
[ ] SELECT queries show recent sensor values and GPS positions.
[ ] Students can explain why GPS is stored separately from scalar measurements.
```
