# Running Processes and Final End-to-End Test

**Primary workstreams:** Student 4 — backend processes; Student 5 — integration test and evidence

This chapter explains how to start the final course backend manually and verify the full data path.

The course setup does not require systemd services. Students start the required processes in terminals so they can see logs and understand the system.

## Manual startup order

Start in this order:

1. MariaDB
2. local `mqttbroker`
3. `mqttbridge`
4. `mqttcli` storage process
5. `mqttcli` dashboard on port 8080

Why this order?

```text
mqttbridge needs the local broker
mqttcli storage needs the local broker and MariaDB
dashboard needs MariaDB
```

## Check MariaDB

```bash
sudo systemctl status mariadb
```

Start if needed:

```bash
sudo systemctl start mariadb
```

## Terminal 1: start mqttbroker

```bash
~/water-buoy/bin/mqttbroker
```

The local broker listens on port 1883.

## Terminal 2: start mqttbridge

Load TTN credentials:

```bash
source ~/water-buoy/config/ttn-mqtt.env
```

Start the bridge using the course values from the broker/bridge chapter. Command pattern:

```bash
~/water-buoy/bin/mqttbridge \
  --from-host "${TTN_MQTT_HOST}" \
  --from-port 1883 \
  --from-username "${TTN_MQTT_USERNAME}" \
  --from-password "${TTN_MQTT_PASSWORD}" \
  --from-topic "${TTN_UPLINK_TOPIC}" \
  --to-host localhost \
  --to-port 1883 \
  --to-topic 'ttn/{application_id}/{device_id}/up'
```

If your local `mqttbridge --help` uses different option names, adapt the option names but keep the same values.

## Terminal 3: start mqttcli storage

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

This process subscribes to local TTN uplinks and inserts numeric values into MariaDB.

## Terminal 4: start mqttcli dashboard

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

Open from a laptop:

```text
http://groupN.local:8080/
```

Replace `groupN` with `group1`, `group2`, `group3`, or `group4`.

## Final end-to-end test

The complete system is working when one real ESP32 uplink travels through the whole chain.

Trace one value:

```text
ESP32 serial monitor
  -> TTN live data
  -> local MQTT topic on Raspberry Pi
  -> MariaDB row
  -> dashboard display
```

Example trace for TDS:

1. ESP32 serial monitor shows fPort 4 uplink.
2. TTN live data decodes `tds_ppm`.
3. Local MQTT subscriber or storage process sees a bridged uplink.
4. MariaDB contains a row with `f_port = 4` and the TDS numeric value.
5. Dashboard shows the latest TDS value.

## Verification SQL

```bash
mariadb -u water_buoy -p water_buoy
```

```sql
SELECT id, received_at, application_id, device_id, f_port, value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

## Final checklist

```text
[ ] ESP32 firmware flashed.
[ ] TTN device joins successfully.
[ ] TTN payload formatter decodes fPorts 1..6.
[ ] TTN MQTT API key exists.
[ ] Raspberry Pi is reachable by SSH as water@groupN.local.
[ ] MariaDB is running.
[ ] mqttbroker is running locally on port 1883.
[ ] mqttbridge forwards TTN uplinks to local MQTT.
[ ] mqttcli stores numeric measurements in MariaDB.
[ ] mqttcli dashboard runs on port 8080.
[ ] browser shows recent measurements at http://groupN.local:8080/.
[ ] students can explain the full data path.
```

If a value is missing in the dashboard, debug backwards:

```text
dashboard -> MariaDB -> mqttcli storage -> local MQTT -> mqttbridge -> TTN -> ESP32
```

If a value never reaches TTN, debug forwards:

```text
ESP32 -> LoRaWAN join -> TTN live data -> formatter
```
