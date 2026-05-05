# Storing MQTT Data in MariaDB with mqttcli

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides decoded TTN payloads; Student 5 reads these rows for the dashboard and final report.

This chapter explains how local MQTT messages become persistent database rows.

The course storage path is:

```text
local mqttbroker on port 1883
  -> mqttcli subscriber/storage mode
  -> MariaDB table measurements
```

The storage model is intentionally simple: store only numeric sensor values and keep the `f_port` column so the meaning of each value is clear.

## Why the schema is simple

For this course, students do not need a complex data warehouse. They need a transparent table that proves the end-to-end data path.

The table stores one numeric value per row:

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

GPS fPort 1 has multiple values and should be stored later in a separate table if needed. The simple course table focuses on the one-number sensor measurements.

## Create the table

The Raspberry Pi setup chapter already creates the database and table. To check or recreate it:

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

Check:

```sql
DESCRIBE measurements;
EXIT;
```

## What mqttcli should store

`mqttcli` subscribes to local bridged TTN uplinks under:

```text
ttn/<application-id>/<device-id>/up
```

For each message it should:

```text
1. parse the JSON uplink
2. read application ID and device ID
3. read fPort
4. read the decoded numeric value
5. insert one row into measurements
```

The stored value should be the decoded measurement value, not a raw ADC value and not a full JSON blob.

Examples:

| Decoded TTN field | fPort | Stored value |
|---|---:|---:|
| `temperature_c` | 2 | `21.75` |
| `ph_level` | 3 | `7.12` |
| `tds_ppm` | 4 | `350.0` |
| `turbidity_ntu` | 5 | `42.0` |
| `ph_board_temperature_c` | 6 | `26.0` |

## Run mqttcli storage

Use the `mqttcli-mariadb` branch build from the MQTTSuite chapter.

Course command pattern:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt remote --host localhost --port 1883 sub 'ttn/#' \
  out-mariadb \
    --host localhost \
    --database water_buoy \
    --user water_buoy \
    --password 'water-buoy-pass' \
    --table measurements
```

If your local `mqttcli --help` prints slightly different option names, keep the same data values:

```text
MQTT host: localhost
MQTT port: 1883
MQTT topic: ttn/#
MariaDB host: localhost
MariaDB database: water_buoy
MariaDB user: water_buoy
MariaDB table: measurements
```

The important behavior is not the exact spelling of one option. The important behavior is that `mqttcli` subscribes to local MQTT and inserts numeric decoded values into MariaDB.

## Verify stored data

After an uplink arrives, check:

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

## Query by sensor type

Because `f_port` identifies the measurement type, students can query one sensor value at a time.

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

## Done when

```text
[ ] mqttcli subscribes to local ttn/# messages.
[ ] The measurements table receives rows.
[ ] Each row contains application_id, device_id, f_port, and value.
[ ] SELECT queries show recent sensor values.
[ ] Students can explain why f_port is needed to interpret value.
```
