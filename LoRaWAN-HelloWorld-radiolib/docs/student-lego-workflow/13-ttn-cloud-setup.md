# TTN Cloud Setup

This chapter describes the cloud part of the project: setting up The Things Network / The Things Stack so the ESP32 buoy can join and send decoded uplinks.

The goal is that students can create a TTN application, register a device, install the payload formatter, and prepare MQTT access for the Raspberry Pi backend.

---

## What TTN does in this project

TTN is the LoRaWAN network backend. It is responsible for:

- receiving packets from LoRaWAN gateways
- handling OTAA joins
- checking frame counters and security
- routing uplinks to the correct application
- running the payload formatter
- exposing data through live data views and MQTT

TTN is not the ESP32 firmware and not the local database. It is the cloud/network layer between LoRaWAN radio and application software.

---

## Create an application

In the TTN console:

1. Log in to The Things Stack / TTN.
2. Open **Applications**.
3. Create a new application.
4. Choose a clear application ID, for example:

```text
water-buoy-course
```

The application is the container for all buoy devices belonging to this project.

For management students, a good analogy is:

```text
TTN application = project folder for LoRaWAN devices
TTN end device = one physical buoy
```

---

## Create an end device

Inside the application:

1. Create a new end device.
2. Choose OTAA activation.
3. Select the LoRaWAN version matching the generator.
4. Select the regional parameters / frequency plan matching the firmware region.
5. Use EU868 for the course in Europe.
6. Set or generate a DevEUI.
7. Use the course JoinEUI:

```text
0000000000000000
```

8. Set or generate AppKey.
9. Set or generate NwkKey if LoRaWAN 1.1.0 is used.

The generator and TTN must agree on:

```text
DevEUI
JoinEUI
AppKey
NwkKey, if LoRaWAN 1.1.0
LoRaWAN version
region
```

---

## LoRaWAN version choice

The generator offers:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

For LoRaWAN 1.1.0, both keys are used:

```text
AppKey
NwkKey
```

For LoRaWAN 1.0.x, only the AppKey is used in this workflow. The generator disables NwkKey.

If the device cannot join, check the LoRaWAN version first. A mismatch between generator and TTN can cause confusing join failures.

---

## Install the payload formatter

After generating `payload-formatter.js` in the local generator:

1. Open the TTN application or end-device settings.
2. Go to **Payload formatters**.
3. Select **Uplink**.
4. Choose **Custom JavaScript formatter**.
5. Paste the generated `payload-formatter.js`.
6. Save.

The formatter must match the generated firmware configuration.

The expected measurement fields are:

| fPort | Decoded field |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

You are done with the payload formatter when TTN live data shows decoded measurement fields and no JavaScript formatter error.

---

## Create an MQTT API key

The Raspberry Pi backend needs MQTT access to TTN.

In TTN:

1. Open the application.
2. Open **API keys**.
3. Create a new API key.
4. Give it a clear name, for example:

```text
raspberry-pi-mqtt-bridge
```

5. Give it rights needed for reading application traffic.
6. Copy the generated key immediately.

Treat this key like a password. Do not commit it to GitHub.

---

## TTN MQTT connection information

Students need these values for the mqttbridge configuration:

| Value | Meaning |
|---|---|
| TTN MQTT server | MQTT endpoint of The Things Stack cluster |
| application ID | TTN application identifier |
| username | usually application ID plus tenant suffix, depending on TTN cluster |
| password | API key |
| uplink topic | topic pattern for uplink messages |

A typical The Things Stack uplink topic pattern is conceptually:

```text
v3/<application-id>@ttn/devices/<device-id>/up
```

The exact username and server depend on the TTN cluster and tenant. Students should copy them from the TTN integration documentation or MQTT integration page for the selected cluster.

---

## Live data test

Before connecting the Raspberry Pi, verify TTN itself.

You are done when TTN live data shows:

- successful join or restored session
- uplinks from the correct device
- expected fPort
- decoded payload fields
- no formatter error

Example decoded TDS payload:

```json
{
  "tds_ppm": 350.0
}
```

Example decoded PH4502C board temperature payload:

```json
{
  "ph_board_temperature_c": 26.0
}
```

---

## Common TTN problems

| Symptom | Likely cause |
|---|---|
| no join | wrong keys, wrong region, wrong LoRaWAN version, radio wiring |
| join works but no decoded fields | missing or stale payload formatter |
| formatter syntax error | incomplete JavaScript copied into TTN |
| fPort unexpected | firmware and formatter generated from different settings |
| join worked before reset but not now | nonce/session mismatch |

Debug order:

```text
serial monitor -> TTN live data -> payload formatter -> MQTT integration
```

Do not start debugging MQTT before TTN live data is correct.
