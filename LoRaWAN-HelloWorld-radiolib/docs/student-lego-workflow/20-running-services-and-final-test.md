# Running Processes and Final End-to-End Test

**Primary workstreams:** Student 4 — backend processes; Student 5 — integration test and evidence

This chapter explains how to start the final course backend manually and verify the full data path.

The course setup does not require systemd services. Students start the required processes in terminals so they can see logs and understand the system.

## Manual startup order

Start in this order:

1. MariaDB
2. local `mqttbroker`
3. `mqttbridge` with `bridge-config.json`
4. local MQTT inspection with `mqttcli`
5. provided `mqttcli` storage/dashboard process

The provided `mqttcli` from the `mqttcli-mariadb` branch is the course process that stores numeric values in MariaDB and serves the dashboard on port 8080. The dashboard part is implemented in the course version of `mqttcli` before the workshop starts.

Do not use invented command-line forms. For MQTTBridge, the course workflow uses a bridge definition file. For storage/dashboard, use the final `mqttcli` command from the implemented course binary.

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
~/water-buoy/bin/mqttbroker \
  in-mqtt \
    local --host 0.0.0.0 \
          --port 1883
```

The local broker listens on port 1883.

## Terminal 2: start mqttbridge

Start the bridge with the JSON definition file created in the MQTTBridge chapter:

```bash
~/water-buoy/bin/mqttbridge \
  bridge --definition ~/water-buoy/config/bridge-config.json
```

The file contains the TTN broker, local broker, credentials, topics, and prefixes.

## Terminal 3: inspect local MQTT

Use `mqttcli` with the documented MQTT publish/subscribe syntax:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

You are done with this check when TTN uplinks appear on the local broker.

## Terminal 4: start provided mqttcli storage/dashboard

Start the provided `mqttcli` storage/dashboard process from the `mqttcli-mariadb` branch.

The process must do this:

```text
local MQTT topic ttn/#
  -> parse decoded TTN JSON
  -> insert numeric values into MariaDB measurements table
  -> serve dashboard on http://groupN.local:8080/
```

The exact command must come from the final implemented `mqttcli --help` output before the course starts:

```bash
~/water-buoy/bin/mqttcli <course-storage-dashboard-options>
```

The dashboard endpoint is:

```text
http://groupN.local:8080/
```

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
3. Local MQTT subscriber sees a bridged uplink under `ttn/#`.
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
[ ] mqttbridge runs with bridge-config.json.
[ ] TTN uplinks appear on local ttn/# topics.
[ ] provided mqttcli storage/dashboard process inserts numeric measurements into MariaDB.
[ ] provided mqttcli storage/dashboard process serves port 8080.
[ ] browser shows recent measurements at http://groupN.local:8080/.
[ ] students can explain the full data path.
```

If a value is missing in the dashboard, debug backwards:

```text
dashboard -> MariaDB -> mqttcli storage/dashboard -> local MQTT -> mqttbridge -> TTN -> ESP32
```

If a value never reaches TTN, debug forwards:

```text
ESP32 -> LoRaWAN join -> TTN live data -> formatter
```
