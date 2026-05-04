# 02 — LoRaWAN and TTN Theory

LoRaWAN is a low-power wide-area network technology. It is designed for devices that send small amounts of data over long distances while consuming little energy.

This project uses LoRaWAN because a water-monitoring buoy should not need Wi-Fi and should be able to run from a battery and solar panel.

## LoRa versus LoRaWAN

LoRa and LoRaWAN are not the same thing.

| Term | Meaning |
|---|---|
| LoRa | radio modulation used for long-range communication |
| LoRaWAN | network protocol built on top of LoRa |

LoRa is the physical radio layer. LoRaWAN defines how devices join a network, how messages are encrypted, how frame counters work, how gateways and network servers handle traffic, and how applications receive uplinks.

## Main LoRaWAN components

| Component | Role |
|---|---|
| End device | the ESP32 buoy node |
| Gateway | receives LoRa radio packets and forwards them to the network server |
| Network Server | handles LoRaWAN MAC layer, frame counters, routing, security |
| Join Server | handles OTAA join keys and session generation |
| Application Server | provides decoded application data to the user |

In this project, TTN / The Things Stack provides the server-side infrastructure.

## OTAA

The firmware uses OTAA:

```text
Over-The-Air Activation
```

With OTAA, the device does not start with a permanent session. It joins the network using root keys and receives session keys.

Important OTAA identifiers and keys:

| Name | Purpose |
|---|---|
| DevEUI | unique device identifier |
| JoinEUI | join/application identifier; zero in this course setup |
| AppKey | application root key |
| NwkKey | network root key for LoRaWAN 1.1.0 |

For this course setup:

```text
JoinEUI = 0000000000000000
```

## LoRaWAN 1.1.0 versus 1.0.x

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

## Regions

LoRaWAN regions define frequency plans and data-rate rules.

For the course in Europe, use:

```text
EU868
```

The generator also offers other RadioLib-supported regions such as US915, AU915, AS923 variants, KR920, and IN865. These are not interchangeable. The selected firmware region must match the physical location and the TTN device configuration.

## fPorts

A LoRaWAN fPort is a small number that identifies the type of application payload.

In this project:

```text
fPort 3 means pH
fPort 4 means TDS
fPort 6 means PH4502C board temperature
```

The TTN payload formatter switches on `input.fPort`. If the fPort mapping is wrong, the decoded data will be wrong.

## Payload size

LoRaWAN payloads should be small. Sending long debug strings or raw calibration data in every uplink wastes airtime and energy.

This is why the current firmware sends only the measurement value during normal operation:

```text
fPort 3 -> pH only
fPort 4 -> TDS ppm only
fPort 5 -> turbidity NTU only
fPort 6 -> board temperature only
```

Calibration raw ADC values stay on the serial monitor.

## Frame counters and nonces

LoRaWAN protects against replay attacks. Devices and servers track counters and nonces.

If a device locally forgets its nonces but TTN still remembers old values, a join can fail. This is why the firmware distinguishes between:

| Reset type | Meaning |
|---|---|
| Factory reset | clears saved session but preserves nonces |
| Dangerous nonce reset | clears session and nonces |

Use dangerous nonce reset only when TTN was also reset appropriately or when using a new DevEUI.

## TTN live data

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

The serial monitor tells what the device tried to send. TTN tells what the network received and decoded.
