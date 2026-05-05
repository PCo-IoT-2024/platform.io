# End-to-End Buoy System Architecture

Until now, the book mainly described the ESP32 buoy node and the TTN uplink path. A complete course system needs more: the measured data should not stop in TTN. It should flow into a local backend on a Raspberry Pi, be stored in a database, and be shown in a dashboard.

The complete target architecture is:

```text
water sensors
  -> ESP32 firmware
  -> LoRa radio
  -> LoRaWAN gateway
  -> The Things Network / The Things Stack
  -> TTN MQTT integration
  -> mqttbridge on Raspberry Pi
  -> local mqttbroker on Raspberry Pi
  -> mqttcli storage process
  -> MariaDB database
  -> SNode.C / MQTTSuite dashboard backend
  -> browser dashboard
```

This chapter gives the overall map. The following chapters explain each part in more detail.

---

## What runs where?

A common source of confusion is that this project uses several computers and services. The following table is the most important orientation point.

| Component | Runs on | Purpose |
|---|---|---|
| ESP32 firmware | ESP32 buoy node | reads sensors and sends LoRaWAN uplinks |
| payload formatter | TTN cloud | decodes uplink bytes into fields such as `ph_level` |
| TTN MQTT integration | TTN cloud | exposes decoded uplinks as MQTT messages |
| mqttbridge | Raspberry Pi | connects TTN MQTT to the local MQTT broker |
| mqttbroker | Raspberry Pi | local MQTT message broker / message hub |
| mqttcli | Raspberry Pi | subscribes to messages and stores them in MariaDB |
| MariaDB | Raspberry Pi | persistent storage of measurements |
| SNode.C / dashboard server | Raspberry Pi | serves API and web dashboard |
| browser | student laptop | displays dashboard and TTN console |

The Raspberry Pi is the local backend server. The ESP32 is the field device. TTN is the cloud/network part.

---

## Why use TTN and a local Raspberry Pi?

TTN is excellent for receiving LoRaWAN uplinks and managing LoRaWAN devices, but TTN is not the final data platform for this course. Students should learn the full path from sensor to local application.

The Raspberry Pi adds:

- a local MQTT broker
- a local database
- local processing tools
- a dashboard server
- a place where students can inspect logs and run commands

This makes the system more understandable than a pure black-box cloud dashboard.

---

## Data flow in stages

The system can be built in stages.

| Level | Goal | You are done when... |
|---:|---|---|
| 1 | ESP32 sends data to TTN | TTN live data shows decoded fields |
| 2 | TTN exposes MQTT data | an MQTT client can subscribe to TTN uplinks |
| 3 | Raspberry Pi receives TTN data | mqttbridge forwards uplinks to the local broker |
| 4 | Local broker distributes messages | mqttcli can subscribe locally |
| 5 | Database stores history | MariaDB contains measurement rows |
| 6 | Dashboard shows data | browser shows current and historical measurements |

This staged workflow is important: do not try to debug the dashboard before TTN uplinks are decoded correctly.

---

## Message versus measurement

Students should distinguish between a message and a measurement.

A message is a transport unit. It may include metadata, device identifiers, timestamps, fPort, and decoded payload.

A measurement is the scientific or environmental value we care about, for example:

```text
ph_level = 7.12
tds_ppm = 350
turbidity_ntu = 42
```

MQTT transports messages. MariaDB stores selected parts of those messages as measurements.

---

## Why MQTT appears twice

There are two MQTT worlds in this system:

1. TTN MQTT, provided by The Things Stack.
2. Local MQTT, provided by `mqttbroker` on the Raspberry Pi.

The bridge connects them:

```text
TTN MQTT -> mqttbridge -> local mqttbroker
```

This separation is useful because local tools can subscribe to the local broker without each tool needing TTN credentials.

---

## Why MariaDB is needed

MQTT is a live message system. It is excellent for distributing fresh messages, but it is not a historical database.

MariaDB stores history. That enables:

- time-range queries
- dashboards with historical charts
- comparing buoys
- exporting data for analysis
- checking calibration changes over time

In short:

```text
MQTT = live stream
MariaDB = memory
Dashboard = human view
```

---

## Course responsibility model

The course system contains several responsibilities:

| Responsibility | Tool / component |
|---|---|
| physical measurement | sensors and calibration |
| embedded control | ESP32 firmware |
| long-range communication | LoRaWAN / TTN |
| message integration | MQTT / mqttbridge |
| local message distribution | mqttbroker |
| persistence | mqttcli + MariaDB |
| user interface | SNode.C / dashboard frontend |

A complete IoT system is only as good as the weakest link in this chain.
