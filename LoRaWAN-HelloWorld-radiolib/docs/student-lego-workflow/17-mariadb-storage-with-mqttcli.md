# Storing MQTT Data in MariaDB

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides decoded TTN payloads; Student 5 reads database rows for the final data view and report.

This chapter defines the database target and the verification workflow. It deliberately does **not** invent a fake `mqttcli out-mariadb` command. The MQTTSuite README documents `mqttcli` as the command-line MQTT publish/subscribe client, and the course should only use command forms that are actually supported by the built tool.

The verified MQTT inspection command is:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

This proves that TTN messages have reached the local broker. The MariaDB insertion step must be implemented by the `mqttcli-mariadb` branch or by a small course storage adapter, but the exact invocation must be taken from the built tool's `--help` output or the course-provided wrapper. Do not document or use unverified command-line syntax.

## Database model

The course storage model is intentionally simple: store one numeric value per row and keep `f_port` so the meaning of the value remains clear.

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

GPS fPort 1 has multiple values and should be stored later in a separate table if needed. The simple course table focuses on one-number sensor measurements.

## Create the table

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

## Required storage behavior

The storage process, whether implemented directly by the `mqttcli-mariadb` branch or by a course wrapper, must do this for each bridged TTN uplink:

```text
1. subscribe to local MQTT topic ttn/#
2. parse the TTN uplink JSON
3. read application ID
4. read device ID
5. read fPort
6. read the decoded numeric measurement value
7. insert one row into measurements
```

The stored value must be the decoded measurement value, not the ESP32 raw ADC value and not the full JSON blob.

Examples:

| Decoded TTN field | fPort | Stored value |
|---|---:|---:|
| `temperature_c` | 2 | `21.75` |
| `ph_level` | 3 | `7.12` |
| `tds_ppm` | 4 | `350.0` |
| `turbidity_ntu` | 5 | `42.0` |
| `ph_board_temperature_c` | 6 | `26.0` |

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

## Verify stored data

After the storage process is running and an uplink arrives, check:

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
[ ] mqttcli shows bridged TTN messages on local topic ttn/#.
[ ] The storage implementation inserts numeric values into measurements.
[ ] Each database row contains application_id, device_id, f_port, and value.
[ ] SELECT queries show recent sensor values.
[ ] Students can explain why f_port is needed to interpret value.
```
