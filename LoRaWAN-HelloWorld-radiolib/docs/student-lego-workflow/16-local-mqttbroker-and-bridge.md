# Running the Local MQTT Broker and TTN Bridge

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides TTN MQTT access; Student 5 verifies that forwarded messages appear in the dashboard path.

This chapter connects TTN MQTT to the group Raspberry Pi.

The target path is:

```text
TTN MQTT on eu1.cloud.thethings.network
  -> mqttbridge
  -> local mqttbroker on groupN.local:1883
  -> mqttcli subscriber/storage/dashboard
```

## Start the local mqttbroker

On the Raspberry Pi:

```bash
~/water-buoy/bin/mqttbroker
```

The lab setup uses the default MQTT port:

```text
1883
```

No username/password is used on the local broker in the teaching network.

Keep this terminal open during the first test. Later, the process can be started from a script, but no systemd service is required for this course setup.

## Test the local broker

Open two SSH terminals to the Raspberry Pi.

Terminal 1 subscribes to all local topics:

```bash
~/water-buoy/bin/mqttcli in-mqtt remote --host localhost --port 1883 sub '#'
```

Terminal 2 publishes a local test message:

```bash
~/water-buoy/bin/mqttcli in-mqtt remote --host localhost --port 1883 pub test/hello 'Hello MQTT'
```

You are done with the local broker test when Terminal 1 receives the message.

## TTN MQTT values

The TTN Sandbox EU cluster MQTT host is:

```text
eu1.cloud.thethings.network
```

The usual TTN username format is:

```text
<application-id>@ttn
```

The password is the TTN API key created for application traffic reading.

The standard TTN uplink topic pattern is:

```text
v3/<application-id>@ttn/devices/<device-id>/up
```

For all devices in an application, use:

```text
v3/<application-id>@ttn/devices/+/up
```

The application ID and device ID are chosen by the students in TTN.

## Store TTN credentials locally

Do not commit real TTN API keys.

Create a local environment file on the Raspberry Pi:

```bash
mkdir -p ~/water-buoy/config
nano ~/water-buoy/config/ttn-mqtt.env
```

Example:

```bash
export TTN_MQTT_HOST='eu1.cloud.thethings.network'
export TTN_APPLICATION_ID='water-buoy-group1'
export TTN_MQTT_USERNAME='water-buoy-group1@ttn'
export TTN_MQTT_PASSWORD='NNSXS.YOUR_API_KEY_HERE'
export TTN_UPLINK_TOPIC='v3/water-buoy-group1@ttn/devices/+/up'
```

Load it before starting the bridge:

```bash
source ~/water-buoy/config/ttn-mqtt.env
```

## Run mqttbridge

The bridge connects the remote TTN broker to the local broker.

Use the exact option names shown by your local build if they differ:

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

If `mqttbridge --help` shows a different naming scheme, keep the same values but adapt the option names. The required configuration is always:

```text
remote host: eu1.cloud.thethings.network
remote username: <application-id>@ttn
remote password: TTN API key
remote topic: v3/<application-id>@ttn/devices/+/up
local host: localhost
local port: 1883
local topic convention: ttn/<application-id>/<device-id>/up
```

## Recommended local topic convention

Use:

```text
ttn/<application-id>/<device-id>/up
```

Example:

```text
ttn/water-buoy-group1/buoy-01/up
```

This allows a simple local subscription for all group uplinks:

```bash
~/water-buoy/bin/mqttcli in-mqtt remote --host localhost --port 1883 sub 'ttn/#'
```

## Verify bridge operation

With `mqttbroker` and `mqttbridge` running, wait for the ESP32 to send an uplink.

In another terminal:

```bash
~/water-buoy/bin/mqttcli in-mqtt remote --host localhost --port 1883 sub 'ttn/#'
```

You are done when a TTN uplink appears as a local MQTT message on the Raspberry Pi.

## Debug order

If no local message appears, debug in this order:

```text
1. TTN live data shows decoded uplinks.
2. TTN API key has application traffic read rights.
3. TTN MQTT host, username, and topic are correct.
4. local mqttbroker is running on port 1883.
5. mqttbridge is running and connected to both sides.
6. mqttcli subscribes to the correct local topic.
```

## Done when

```text
[ ] mqttbroker runs locally on port 1883.
[ ] mqttcli can publish and subscribe locally.
[ ] mqttbridge connects to TTN MQTT.
[ ] TTN uplinks appear under local ttn/# topics.
[ ] The group has documented its application ID and device ID.
```
