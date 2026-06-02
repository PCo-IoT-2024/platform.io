# 02 — LoRaWAN and TTN Theory

LoRaWAN is a low-power wide-area network technology. It is designed for devices that send small amounts of data over long distances while consuming little energy.

This project uses LoRaWAN because a water-monitoring buoy should not need Wi-Fi and should be able to run from a battery and solar panel.

For management students, the most important idea is this:

```text
LoRaWAN is not just radio. It is an operating model for low-power distributed sensing.
```

It defines how a small device can safely and efficiently send data into an application without having a permanent internet connection of its own.

---

## 1. Why not Wi-Fi?

Wi-Fi is good when a device is near an access point and can use more energy. A buoy or environmental sensing node has different requirements.

| Requirement | Wi-Fi | LoRaWAN |
|---|---|---|
| short range around building | very good | possible, but not main strength |
| long range outdoor sensing | limited | strong |
| high data rate | very good | low |
| low energy | moderate to poor | very good |
| small sensor messages | possible | ideal |
| battery/solar operation | harder | intended use case |

The buoy sends small messages such as:

```text
pH = 7.12
TDS = 350 ppm
turbidity = 42 NTU
```

It does not send video, audio, or large files. That makes LoRaWAN a good match.

---

## 2. LoRa versus LoRaWAN

LoRa and LoRaWAN are not the same thing.

| Term | Meaning |
|---|---|
| LoRa | radio modulation used for long-range communication |
| LoRaWAN | network protocol and system architecture built on top of LoRa |

An analogy:

```text
LoRa is like the acoustic properties of a language.
LoRaWAN is like the postal system, addresses, rules, envelopes, and delivery process.
```

LoRa defines how the radio signal is shaped. LoRaWAN defines how devices join the network, how messages are encrypted, how counters are checked, how gateways forward data, and how the application receives uplinks.

---

## 3. LoRaWAN as a multi-layer system

A LoRaWAN uplink passes through several layers:

```text
sensor value
  -> ESP32 firmware payload
  -> LoRaWAN encrypted frame
  -> LoRa radio transmission
  -> gateway reception
  -> network server processing
  -> application server
  -> payload formatter
  -> decoded application data
```

Each layer has a different responsibility.

| Layer | Question it answers |
|---|---|
| sensor layer | What physical value was measured? |
| firmware layer | How do we encode this value? |
| LoRaWAN layer | Is this device allowed to send? Is the frame valid? |
| gateway layer | Who heard the radio message? |
| network server | Is the frame counter valid? Which application owns it? |
| application layer | What does this payload mean? |
| payload formatter | Which JSON fields should TTN show? |

This separation is powerful, but it also means debugging must be systematic.

---

## 4. Main LoRaWAN components

| Component | Role |
|---|---|
| End device | the ESP32 buoy node |
| Gateway | receives LoRa radio packets and forwards them to the internet |
| Network Server | handles LoRaWAN MAC layer, frame counters, routing, security |
| Join Server | handles OTAA join and session-key generation |
| Application Server | provides application data to the user |

In this project, TTN / The Things Stack provides the server-side infrastructure.

The end device does not know which gateway will receive the packet. It simply transmits. One or more gateways may hear it. The network server then decides what to do with the packet.

This is different from Wi-Fi, where a device connects to a specific access point.

---

## 5. Uplink and downlink

A message from the device to TTN is an uplink.

A message from TTN to the device is a downlink.

In this course project, the main direction is uplink:

```text
buoy -> TTN
```

Downlinks are possible but should be used carefully. They consume gateway airtime and device energy. LoRaWAN is not designed for constant two-way chat.

The system is best understood as:

```text
small sensor reports, sent occasionally
```

not:

```text
continuous live connection
```

---

## 6. OTAA: joining the network

The firmware uses OTAA:

```text
Over-The-Air Activation
```

With OTAA, the device does not permanently store all final session keys. Instead, it joins the network using root keys. During join, session keys are created.

A simplified OTAA story:

```text
Device: Hello, I am this DevEUI and I know the correct secret.
Network: I verify you and create session keys.
Device: From now on I use these session keys for data messages.
```

Important OTAA identifiers and keys:

| Name | Purpose |
|---|---|
| DevEUI | unique device identity |
| JoinEUI | join/application identity; zero in this course setup |
| AppKey | application root key |
| NwkKey | network root key for LoRaWAN 1.1.0 |

For this course setup:

```text
JoinEUI = 0000000000000000
```

---

## 7. DevEUI, AppKey, and NwkKey as management concepts

A useful way to think about the identifiers:

| Item | Analogy |
|---|---|
| DevEUI | device passport number |
| AppKey | secret used to prove application identity |
| NwkKey | secret used for network identity in LoRaWAN 1.1.0 |
| JoinEUI | which join/application domain the device belongs to |

The DevEUI is not secret. The keys are secret.

Do not put real AppKeys or NwkKeys into public screenshots, public repositories, reports, or slides.

---

## 8. LoRaWAN 1.1.0 versus 1.0.x

The generator offers:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

For LoRaWAN 1.1.0, the generator uses:

```text
AppKey + NwkKey
```

For LoRaWAN 1.0.x, the generator uses:

```text
AppKey only
```

This is why the NwkKey input is disabled when `LoRaWAN 1.0.x` is selected.

Manager-level meaning:

```text
Different protocol versions have different security/key models.
The device configuration and TTN configuration must match.
```

If the firmware assumes LoRaWAN 1.1.0 but TTN is configured differently, joining can fail.

---

## 9. Regions and frequency plans

LoRaWAN regions define frequency plans and data-rate rules.

For the course in Europe, use:

```text
EU868
```

The generator also offers other RadioLib-supported regions such as:

```text
US915
EU433
AU915
CN470
AS923
AS923_2
AS923_3
AS923_4
KR920
IN865
```

These are not just labels. They describe legal and technical radio behavior.

A device configured for the wrong region may:

- transmit on the wrong frequencies
- fail to join
- violate local rules
- not be heard by nearby gateways

For students in Austria / Europe, EU868 is the correct course default.

---

## 10. fPorts

A LoRaWAN fPort identifies the type of application payload.

In this project:

```text
fPort 1 -> GPS
fPort 2 -> water temperature
fPort 3 -> pH
fPort 4 -> TDS
fPort 5 -> turbidity
fPort 6 -> PH4502C board temperature
```

The TTN payload formatter switches on `input.fPort`.

A helpful analogy:

```text
The fPort is like the label on a small envelope.
The payload formatter opens the envelope differently depending on the label.
```

If the label says “TDS”, the formatter expects a TDS value. If the label says “pH”, it expects a pH value.

---

## 11. Payloads and payload formatters

The firmware sends bytes. TTN cannot automatically know what those bytes mean.

Example raw payload text:

```text
350.000000
```

Without context, that could mean:

- 350 ppm TDS
- 350 meters altitude
- 350 NTU turbidity
- 350 seconds
- a diagnostic number

The fPort and payload formatter provide the context.

Current formatter behavior:

| fPort | Decoded result |
|---:|---|
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

The current measurement formatter intentionally does not emit `payload_raw`. Students should see clean decoded measurement fields.

---

## 12. Payload size and airtime

LoRaWAN is optimized for small messages. Sending too much data is bad for:

- battery life
- radio airtime
- network fairness
- gateway capacity

This is why the current firmware sends only the measurement value during normal operation:

```text
fPort 3 -> pH only
fPort 4 -> TDS ppm only
fPort 5 -> turbidity NTU only
fPort 6 -> board temperature only
```

Raw ADC calibration values stay on the serial monitor. They are important during calibration, but they should not be sent in every field uplink.

---

## 13. Frame counters and nonces

LoRaWAN protects against replay attacks. A replay attack means someone records a valid radio message and sends it again later.

To protect against this, LoRaWAN uses counters and nonces.

Simplified:

```text
The network remembers what it has already seen.
Old or repeated values are rejected.
```

This is why resets matter.

| Reset type | Meaning |
|---|---|
| Factory reset | clears saved session but preserves nonces |
| Dangerous nonce reset | clears session and nonces |

Use dangerous nonce reset only when TTN was also reset appropriately or when using a new DevEUI.

Otherwise the device may try to reuse values that TTN rejects.

---

## 14. TTN live data as the project control room

TTN live data is the main debugging view. It shows:

- join attempts
- accepted joins
- uplinks
- fPorts
- raw payloads
- decoded payload fields
- payload formatter errors

When debugging, always check both sides:

```text
serial monitor on the ESP32
TTN live data in the browser
```

The serial monitor tells what the device tried to do. TTN tells what the network actually received and decoded.

---

## 15. Typical LoRaWAN failure categories

| Symptom | Possible category |
|---|---|
| no join | wrong keys, wrong LoRaWAN version, wrong region, bad radio wiring |
| join succeeds but no decoded data | formatter missing or wrong |
| decoded fields wrong | stale formatter or wrong fPort mapping |
| sporadic uplinks | coverage, antenna, power, timing, gateway availability |
| join worked before reset but not after | nonce/session problem |

The important skill is not guessing. Work layer by layer.

---

## 16. Management takeaway

For managers, LoRaWAN is interesting because it separates responsibilities:

| Responsibility | Technical owner |
|---|---|
| physical measurement | sensor/probe design |
| local control | firmware |
| radio access | LoRa radio + antenna |
| network operation | gateways + network server |
| application interpretation | payload formatter + database/dashboard |
| decision making | humans and organizations |

A reliable IoT project needs all layers to fit together. A perfect dashboard cannot fix bad calibration. A perfect sensor cannot help if the payload formatter decodes the wrong field. A good project leader must understand the interfaces between layers.
