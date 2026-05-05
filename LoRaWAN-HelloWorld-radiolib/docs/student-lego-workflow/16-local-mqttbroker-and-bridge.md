# Running the Local MQTT Broker and TTN Bridge

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides TTN MQTT access; Student 5 verifies that forwarded messages appear in the dashboard path.

This chapter connects TTN MQTT to the group Raspberry Pi.

The target path is:

```text
TTN MQTT on eu1.cloud.thethings.network
  -> mqttbridge using bridge-config.json
  -> local mqttbroker on groupN.local:1883
  -> local mqttcli subscriber / later storage workflow
```

The important correction is this: **MQTTBridge is configured by a bridge JSON file.** For the course workflow, students should not build a long `mqttbridge` command line. They should create a bridge configuration file and run the bridge with that file.

## Start the local mqttbroker

The MQTTSuite README shows the MQTTBroker command structure with connection instances. For a plain MQTT listener on TCP/IPv4, use the `in-mqtt` instance with a local endpoint.

On the Raspberry Pi:

```bash
~/water-buoy/bin/mqttbroker \
  in-mqtt \
    local --host 0.0.0.0 \
          --port 1883
```

For the first lab test, keep this process in the foreground so logs are visible.

The local course broker uses:

```text
host: group Raspberry Pi
port: 1883
authentication: none
TLS: no
```

The MQTTSuite broker can expose more instances and a web interface, but the course bridge path only needs plain local MQTT on port 1883.

## Test the local broker with mqttcli

Open two SSH terminals to the Raspberry Pi.

Terminal 1 subscribes to all local topics:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic '#'
```

Terminal 2 publishes a local test message:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    pub --topic 'test/hello' \
        --message 'Hello MQTT'
```

You are done with the local broker test when Terminal 1 receives the message.

## TTN MQTT values

For The Things Stack Sandbox in Europe, use:

```text
host: eu1.cloud.thethings.network
port: 8883
TLS: yes
username format: <application-id>@ttn
password: TTN API key
topic: v3/<application-id>@ttn/devices/+/up
```

The application ID and device ID are chosen by the students in TTN.

## Create the bridge configuration file

Create a configuration directory:

```bash
mkdir -p ~/water-buoy/config
```

Create the bridge configuration:

```bash
nano ~/water-buoy/config/bridge-config.json
```

Template for group 1, application `water-buoy-group1`:

```json
{
  "bridges": [
    {
      "name": "ttn-to-local",
      "prefix": "",
      "brokers": [
        {
          "network": {
            "instance_name": "ttn-mqtts",
            "protocol": "in",
            "in": {
              "host": "eu1.cloud.thethings.network",
              "port": 8883
            },
            "encryption": "tls",
            "transport": "stream"
          },
          "prefix": "ttn/",
          "mqtt": {
            "client_id": "group1-ttn-bridge",
            "clean_session": true,
            "username": "water-buoy-group1@ttn",
            "password": "NNSXS.YOUR_TTN_API_KEY_HERE",
            "loop_prevention": true
          },
          "topics": [
            {
              "topic": "v3/water-buoy-group1@ttn/devices/+/up",
              "qos": 0
            }
          ]
        },
        {
          "network": {
            "instance_name": "local-mqtt",
            "protocol": "in",
            "in": {
              "host": "127.0.0.1",
              "port": 1883
            },
            "encryption": "legacy",
            "transport": "stream"
          },
          "prefix": "local/",
          "mqtt": {
            "client_id": "group1-local-bridge",
            "clean_session": true,
            "loop_prevention": true
          },
          "topics": [
            {
              "topic": "#",
              "qos": 0
            }
          ]
        }
      ]
    }
  ]
}
```

Important notes:

- Replace `water-buoy-group1` with the real TTN application ID.
- Replace the API key placeholder with the real TTN API key.
- Do not commit this file to a public repository if it contains a real TTN API key.
- MQTTBridge forwards messages between all brokers in the same bridge. Prefixes are applied according to the MQTTSuite bridge model: bridge prefix plus source-broker prefix plus original topic.

With the template above, TTN-originated messages forwarded to the local broker appear with the `ttn/` prefix.

## Run mqttbridge

Run the bridge with the JSON definition file:

```bash
~/water-buoy/bin/mqttbridge \
  bridge --definition ~/water-buoy/config/bridge-config.json
```

This is the course workflow. Do not use a long list of bridge connection options on the command line.

## Verify bridge operation

With `mqttbroker` and `mqttbridge` running, wait for the ESP32 to send an uplink.

In another terminal, subscribe to the local broker:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

You are done when a TTN uplink appears as a local MQTT message on the Raspberry Pi.

## Debug order

If no local message appears, debug in this order:

```text
1. TTN live data shows decoded uplinks.
2. TTN API key has application traffic read rights.
3. bridge-config.json contains the correct TTN host, port, username, password, and topic.
4. local mqttbroker is running on port 1883.
5. mqttbridge is running with the bridge definition file.
6. mqttcli subscribes to the correct local prefixed topic.
```

## Done when

```text
[ ] mqttbroker runs locally on port 1883.
[ ] mqttcli can publish and subscribe locally.
[ ] bridge-config.json exists and contains the TTN and local broker definitions.
[ ] mqttbridge starts with bridge --definition.
[ ] TTN uplinks appear under local ttn/# topics.
[ ] The group has documented its application ID and device ID.
```
