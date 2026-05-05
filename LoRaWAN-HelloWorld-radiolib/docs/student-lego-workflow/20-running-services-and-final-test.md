# Running Services and Final End-to-End Test

This chapter explains how to move from many manually started terminal commands to a repeatable backend setup.

The final system should run these components on the Raspberry Pi:

```text
mqttbroker
mqttbridge
mqttcli storage process
dashboard server
MariaDB
```

During early testing, start tools manually. For a more autonomous setup, create systemd services.

---

## Manual startup order

Start in this order:

1. MariaDB
2. local mqttbroker
3. mqttbridge
4. mqttcli storage process
5. dashboard server

Why this order?

```text
mqttbridge needs the local broker
mqttcli needs the local broker and MariaDB
dashboard needs MariaDB
```

---

## Check MariaDB

```bash
sudo systemctl status mariadb
```

Start if needed:

```bash
sudo systemctl start mariadb
```

---

## Start local mqttbroker

Example placeholder:

```bash
~/water-buoy/bin/mqttbroker --config ~/water-buoy/config/mqttbroker.conf
```

Check with mqttcli in another terminal:

```bash
~/water-buoy/bin/mqttcli subscribe --host localhost --topic '#'
```

---

## Start mqttbridge

Example placeholder:

```bash
~/water-buoy/bin/mqttbridge --config ~/water-buoy/config/ttn-bridge.conf
```

You are done when TTN uplinks appear on local MQTT topics.

---

## Start mqttcli storage

Example conceptual command:

```bash
~/water-buoy/bin/mqttcli subscribe \
  --host localhost \
  --topic 'ttn/+/+/up' \
  --store-measurements
```

Use the actual `mqttcli --help` output for the exact command.

You are done when MariaDB receives new rows.

---

## Start dashboard server

Example placeholder:

```bash
~/water-buoy/bin/dashboard-server --config ~/water-buoy/config/dashboard.conf
```

Open in browser:

```text
http://waterbuoy-pi.local:<port>/
```

---

## Optional systemd services

For a more stable setup, use systemd.

Example service names:

```text
waterbuoy-mqttbroker.service
waterbuoy-mqttbridge.service
waterbuoy-storage.service
waterbuoy-dashboard.service
```

A simple service pattern:

```ini
[Unit]
Description=Water Buoy MQTT Broker
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/water-buoy
ExecStart=/home/pi/water-buoy/bin/mqttbroker --config /home/pi/water-buoy/config/mqttbroker.conf
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

Install a service file under:

```text
/etc/systemd/system/
```

Reload and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now waterbuoy-mqttbroker.service
```

Check logs:

```bash
journalctl -u waterbuoy-mqttbroker.service -f
```

Adapt the command and user to the actual setup.

---

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
3. Local MQTT subscriber sees a bridged uplink.
4. MariaDB contains a row with `field_name = 'tds_ppm'`.
5. Dashboard shows the latest TDS value.

---

## Final checklist

```text
[ ] ESP32 firmware flashed.
[ ] TTN device joins successfully.
[ ] TTN payload formatter decodes fPorts 1..6.
[ ] TTN MQTT API key exists.
[ ] Raspberry Pi is reachable by SSH.
[ ] MariaDB is running.
[ ] mqttbroker is running locally.
[ ] mqttbridge forwards TTN uplinks to local MQTT.
[ ] mqttcli stores measurements in MariaDB.
[ ] dashboard server runs.
[ ] browser shows recent measurements.
[ ] students can explain the full data path.
```

If a value is missing in the dashboard, debug backwards:

```text
dashboard -> MariaDB -> mqttcli -> local MQTT -> mqttbridge -> TTN -> ESP32
```

If a value never reaches TTN, debug forwards:

```text
ESP32 -> LoRaWAN join -> TTN live data -> formatter
```
