# MQTT Theory for the Buoy Backend

MQTT is the message system used between TTN, the Raspberry Pi, and the local backend tools.

The important idea is:

```text
MQTT distributes messages between programs.
It does not permanently store measurement history by itself.
```

For this project, MQTT is the live message bus. MariaDB is the long-term memory.

---

## Why MQTT is used

MQTT is popular in IoT because it is simple, lightweight, and based on a broker model.

A buoy system produces small messages:

```text
new pH value
new TDS value
new turbidity value
new GPS position
```

MQTT is well suited for distributing these messages to several consumers.

Examples of consumers:

- database storage process
- live dashboard
- debugging terminal
- alerting process
- data export tool

---

## Broker, publisher, subscriber

MQTT has three basic roles.

| Role | Meaning | Example in this project |
|---|---|---|
| Broker | message hub | `mqttbroker` on Raspberry Pi |
| Publisher | sends messages to broker | `mqttbridge` |
| Subscriber | receives messages from broker | `mqttcli`, dashboard backend |

A useful analogy:

```text
Broker = post office
Topic = mailbox name
Publisher = person who sends letters
Subscriber = person who receives letters from selected mailboxes
```

Publishers and subscribers do not need to know each other directly. They only need to agree on topic names.

---

## Topics

A topic is a hierarchical name for messages.

Example topic ideas:

```text
ttn/water-buoy-course/sx1262-v11-v104-01/up
water/sx1262-v11-v104-01/ph
water/sx1262-v11-v104-01/tds
water/sx1262-v11-v104-01/turbidity
```

Topic design matters. A good topic structure makes debugging and data processing easier.

A simple course convention should include:

- source system, for example `ttn`
- application ID
- device ID
- message type, for example `up`

---

## Payload

The payload is the message content.

In TTN MQTT, the payload is usually JSON. It contains much more than only the measurement value:

- device identifiers
- application ID
- time
- fPort
- decoded payload
- radio metadata
- gateway information

The local storage process should select the parts that are useful for the database.

---

## Wildcards

MQTT supports wildcards for subscriptions.

| Wildcard | Meaning |
|---|---|
| `+` | one topic level |
| `#` | all remaining topic levels |

Examples:

```text
ttn/+/+/up
```

matches uplinks from many applications/devices if the local topic convention has exactly those levels.

```text
#
```

subscribes to everything. This is useful for debugging, but too broad for production.

---

## QoS

QoS means Quality of Service.

Common MQTT QoS levels:

| QoS | Meaning |
|---:|---|
| 0 | at most once |
| 1 | at least once |
| 2 | exactly once, more complex |

For a teaching setup, QoS 0 or QoS 1 is normally enough. QoS does not fix bad sensor calibration and does not replace database transactions.

---

## Retained messages

An MQTT retained message is the last message kept by the broker for a topic. A new subscriber immediately receives it.

Retained messages are useful for state, for example:

```text
system/status
```

They are usually less useful for raw measurement streams, because the database should store history.

---

## Local broker versus TTN broker

There are two broker contexts:

| Broker | Location | Purpose |
|---|---|---|
| TTN MQTT broker | TTN cloud | exposes TTN application traffic |
| local mqttbroker | Raspberry Pi | local message hub for the course backend |

The bridge connects them:

```text
TTN MQTT broker -> mqttbridge -> local mqttbroker
```

This prevents every local tool from needing direct TTN credentials.

---

## Why a bridge is used

A bridge forwards messages between two MQTT systems.

In this course:

```text
mqttbridge subscribes to TTN uplinks
mqttbridge republishes them locally
```

Benefits:

- one place for TTN credentials
- local backend can continue using local topics
- easier debugging
- local broker can serve several subscribers
- architecture is visible and teachable

---

## You are done when...

You understand MQTT well enough when you can explain:

```text
[ ] what the broker does
[ ] what a topic is
[ ] what publisher and subscriber mean
[ ] why TTN MQTT and local MQTT are different
[ ] why mqttbridge exists
[ ] why MQTT is not the database
```

The next chapters use these concepts with the MQTTSuite tools.
