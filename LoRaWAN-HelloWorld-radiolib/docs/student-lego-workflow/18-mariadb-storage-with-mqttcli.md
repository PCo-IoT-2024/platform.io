# Storing MQTT Data in MariaDB with mqttcli

This chapter explains how live MQTT messages become persistent database rows.

The target path is:

```text
local mqttbroker -> mqttcli storage process -> MariaDB
```

MQTT is the live message stream. MariaDB is the long-term memory of the buoy system.

---

## Why store data in a database?

TTN live data and MQTT subscribers show current messages. They are useful for debugging, but they are not the same as a database.

A database allows:

- storing history
- querying time ranges
- comparing devices
- exporting measurements
- feeding a dashboard
- detecting trends

Without a database, the system forgets old measurements.

---

## Minimal database model

A simple course database can start with one measurement table.

Example:

```sql
CREATE TABLE measurements (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128),
  device_id VARCHAR(128),
  f_port INT,
  field_name VARCHAR(128),
  numeric_value DOUBLE,
  text_value TEXT,
  raw_json JSON
);
```

This generic schema is not perfect, but it is easy to teach. Each decoded value becomes one row.

Example rows:

| device_id | f_port | field_name | numeric_value |
|---|---:|---|---:|
| buoy-01 | 3 | ph_level | 7.12 |
| buoy-01 | 4 | tds_ppm | 350 |
| buoy-01 | 5 | turbidity_ntu | 42 |
| buoy-01 | 6 | ph_board_temperature_c | 26 |

---

## Create the table

Open MariaDB:

```bash
mariadb -u water_buoy -p water_buoy
```

Create the table:

```sql
CREATE TABLE IF NOT EXISTS measurements (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128),
  device_id VARCHAR(128),
  f_port INT,
  field_name VARCHAR(128),
  numeric_value DOUBLE,
  text_value TEXT,
  raw_json JSON
);
```

Check:

```sql
SHOW TABLES;
DESCRIBE measurements;
```

Exit:

```sql
EXIT;
```

---

## What mqttcli should do

In this workflow, `mqttcli` subscribes to local MQTT uplink topics and writes selected values to MariaDB.

Conceptually:

```text
subscribe to ttn/+/+/up
parse JSON
find decoded_payload
extract fPort and device ID
insert rows into MariaDB
```

The exact command depends on the current `mqttcli` implementation in the `mqttcli-mariadb` branch. Check:

```bash
~/water-buoy/bin/mqttcli --help
```

and look for MariaDB or storage-related options.

---

## Example storage command pattern

The following is a conceptual command pattern. Adapt it to the actual `mqttcli` options:

```bash
~/water-buoy/bin/mqttcli subscribe \
  --host localhost \
  --topic 'ttn/+/+/up' \
  --mariadb-host localhost \
  --mariadb-database water_buoy \
  --mariadb-user water_buoy \
  --mariadb-password 'change-this-password' \
  --store-measurements
```

If the project provides a configuration-file based workflow, prefer that for teaching because it avoids very long command lines.

Do not commit real database passwords to Git.

---

## Verify stored data

After an uplink arrives, check MariaDB:

```bash
mariadb -u water_buoy -p water_buoy
```

Run:

```sql
SELECT id, received_at, device_id, f_port, field_name, numeric_value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

You are done when new rows appear after ESP32 uplinks.

---

## Raw JSON versus normalized values

There are two useful storage layers:

1. raw JSON message
2. normalized measurement fields

Raw JSON is useful because it preserves the original message. Normalized fields are useful because they are easy to query.

For a teaching project, storing both is reasonable:

```text
raw_json = original TTN/MQTT message
field_name + numeric_value = easy dashboard/query form
```

---

## Suggested indexes

For larger datasets, add indexes:

```sql
CREATE INDEX idx_measurements_device_time
ON measurements (device_id, received_at);

CREATE INDEX idx_measurements_field_time
ON measurements (field_name, received_at);
```

This helps dashboard queries such as:

```sql
SELECT received_at, numeric_value
FROM measurements
WHERE device_id = 'buoy-01'
  AND field_name = 'tds_ppm'
ORDER BY received_at DESC
LIMIT 100;
```

---

## You are done when...

This chapter is complete when:

```text
[ ] The measurements table exists.
[ ] mqttcli subscribes to local MQTT uplinks.
[ ] A TTN uplink creates at least one database row.
[ ] SELECT queries show recent measurements.
[ ] Students can explain the difference between live MQTT and stored history.
```
