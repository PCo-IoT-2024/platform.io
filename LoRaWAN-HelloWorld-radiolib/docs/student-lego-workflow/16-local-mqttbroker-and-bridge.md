# Running the Local MQTT Broker and TTN Bridge

This chapter explains how the Raspberry Pi receives TTN MQTT messages and republishes them into the local course MQTT broker.

The target path is:

```text
TTN MQTT -> mqttbridge -> local mqttbroker -> local subscribers
```

The local broker is the central message hub on the Pi.

---

## Start the local mqttbroker

First locate the `mqttbroker` binary from the MQTTSuite build.

Example placeholder:

```bash
~/water-buoy/bin/mqttbroker
```

Start the broker:

```bash
~/water-buoy/bin/mqttbroker
```

If the broker supports configuration files, use a course configuration file, for example:

```bash
~/water-buoy/bin/mqttbroker --config ~/water-buoy/config/mqttbroker.conf
```

The exact command depends on the current MQTTSuite CLI options. Use:

```bash
~/water-buoy/bin/mqttbroker --help
```

You are done with this step when the broker starts and listens on the configured MQTT port.

---

## Test the local broker

Open two SSH terminals to the Raspberry Pi.

Terminal 1: subscribe to all local messages.

```bash
~/water-buoy/bin/mqttcli subscribe --host localhost --topic '#'
```

Terminal 2: publish a test message.

```bash
~/water-buoy/bin/mqttcli publish --host localhost --topic test/hello --message 'Hello MQTT'
```

The exact option names may differ. Check:

```bash
~/water-buoy/bin/mqttcli --help
```

You are done when the subscriber terminal receives the test message.

---

## Configure TTN MQTT access

From the TTN cloud setup chapter, collect:

| Value | Example / meaning |
|---|---|
| TTN MQTT server | cluster-specific host |
| TTN application ID | application name |
| TTN MQTT username | application/tenant-specific username |
| TTN MQTT password | API key |
| uplink topic | `v3/<application-id>@ttn/devices/+/up` |

Do not store real API keys in Git.

Create a local config file outside the public repository, for example:

```bash
mkdir -p ~/water-buoy/config
nano ~/water-buoy/config/ttn-mqtt.env
```

Example structure:

```text
TTN_MQTT_HOST=<cluster-mqtt-host>
TTN_MQTT_USERNAME=<application-id>@ttn
TTN_MQTT_PASSWORD=<api-key>
TTN_UPLINK_TOPIC=v3/<application-id>@ttn/devices/+/up
LOCAL_MQTT_HOST=localhost
LOCAL_MQTT_TOPIC_PREFIX=ttn
```

Adapt this to the actual mqttbridge configuration format.

---

## Run mqttbridge

`mqttbridge` connects two MQTT worlds:

```text
TTN MQTT broker
local mqttbroker
```

Conceptual command:

```bash
~/water-buoy/bin/mqttbridge --config ~/water-buoy/config/ttn-bridge.conf
```

Use the tool help to confirm current options:

```bash
~/water-buoy/bin/mqttbridge --help
```

The bridge should:

1. connect to TTN MQTT using the API key
2. subscribe to TTN uplink topics
3. connect to the local broker
4. republish uplinks locally

---

## Recommended local topic convention

A clear local topic structure is important.

Suggested convention:

```text
ttn/<application-id>/<device-id>/up
```

Examples:

```text
ttn/water-buoy-course/sx1262-v11-v104-01/up
ttn/water-buoy-course/buoy-02/up
```

This makes it easy to subscribe to:

```text
ttn/+/+/up
```

for all uplinks.

---

## Verify bridge operation

With the broker and bridge running, subscribe locally:

```bash
~/water-buoy/bin/mqttcli subscribe --host localhost --topic 'ttn/#'
```

Then wait for an ESP32 uplink.

You are done when a TTN uplink appears as a local MQTT message on the Pi.

---

## What a local uplink message should contain

A bridged TTN message should contain enough information to store a measurement:

- application ID
- device ID
- time
- fPort
- decoded payload
- raw payload if available
- radio metadata if useful

The database storage process can later select what it needs.

---

## Common problems

| Symptom | Likely cause |
|---|---|
| mqttbridge cannot connect to TTN | wrong host, username, API key, network issue |
| mqttbridge connects but no messages arrive | wrong topic or no device uplinks |
| local subscriber sees nothing | local broker not running or wrong local topic |
| repeated disconnects | network instability or authentication failure |

Debug order:

```text
TTN live data first
TTN MQTT credentials second
local broker third
mqttbridge fourth
local subscriber fifth
```

---

## You are done when...

This chapter is complete when:

```text
[ ] mqttbroker runs on the Raspberry Pi.
[ ] mqttcli can publish and subscribe locally.
[ ] mqttbridge connects to TTN MQTT.
[ ] TTN uplinks appear on local MQTT topics.
[ ] The local topic convention is documented for the group.
```
