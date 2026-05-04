# 00a — Technology Primer for Managers

This chapter is intentionally written for students who do not have a strong electronics or embedded-systems background. The goal is to build a mental model before looking at pins, firmware, and calibration values.

A water-monitoring buoy is not just a gadget. It is a small cyber-physical system. It observes the physical world, converts observations into electrical signals, converts electrical signals into digital values, sends those values through a communication network, and finally makes them visible as data.

The complete chain is:

```text
water condition
  -> sensor
  -> electrical signal
  -> microcontroller reading
  -> firmware interpretation
  -> radio transmission
  -> network server
  -> application decoder
  -> data field in TTN
  -> human interpretation
```

Every arrow in this chain is a possible source of error or uncertainty.

---

## 1. What is an embedded system?

An embedded system is a computer that is built into a larger object or process. It usually does not look like a laptop or phone. It has no screen, keyboard, or normal user interface.

In this project, the embedded system is the ESP32-based buoy node.

It has three main jobs:

1. control sensors
2. process measurement values
3. communicate data

A useful analogy is a very small field worker:

```text
wake up
look at one instrument
write down one number
send the number to headquarters
go back to sleep
```

This is exactly what the firmware does.

---

## 2. What is a microcontroller?

A microcontroller is a small computer on one chip. It contains:

- processor
- memory
- input/output pins
- timers
- communication interfaces
- analog-to-digital converter
- sleep modes

The ESP32 is the microcontroller in this project.

It is not as powerful as a laptop, but it is very good at direct interaction with hardware. It can read pins, communicate with sensors, control a radio module, and enter deep sleep to save energy.

A manager-level way to think about it:

```text
A microcontroller is the operational controller at the edge of the system.
```

It makes local decisions before any cloud service sees the data.

---

## 3. What is a sensor?

A sensor converts a physical or chemical property into an electrical signal.

Examples:

| Physical / chemical property | Sensor output |
|---|---|
| water temperature | digital number from DS18B20 |
| pH | analog voltage from PH4502C board |
| TDS | analog voltage from conductivity circuit |
| turbidity | analog voltage from optical circuit |
| position | serial digital messages from GPS |

A sensor never directly sends “truth”. It sends a signal that must be interpreted.

That interpretation depends on:

- calibration
- noise
- sensor quality
- environment
- wiring
- power supply
- firmware model

This is why measurement systems are not only technical systems but also interpretation systems.

---

## 4. Digital versus analog sensors

Some sensors output digital data. Others output analog voltages.

### Digital sensor

A digital sensor already communicates numbers.

Example:

```text
DS18B20 -> 21.75 °C
GPS -> latitude and longitude
```

The ESP32 reads the value through a digital protocol.

### Analog sensor

An analog sensor outputs a voltage. The ESP32 must convert this voltage into a number.

Example:

```text
PH4502C output voltage -> ESP32 ADC raw value -> calibrated pH
```

Analog sensors need more care because the voltage can be affected by many things:

- power supply
- cable length
- electrical noise
- ADC behavior
- resistor dividers
- temperature
- sensor drift

For this reason, all analog sensors in this project use calibration.

---

## 5. What is an ADC?

ADC means:

```text
Analog-to-Digital Converter
```

It converts a voltage into a number.

A simplified example:

```text
0.0 V -> ADC value 0
1.65 V -> ADC value around 2048
3.3 V -> ADC value around 4095
```

Real ESP32 ADC behavior is not perfectly linear and not perfectly precise. That is one reason why this project calibrates with raw ADC values instead of pretending that the voltage is exact.

Manager-level interpretation:

```text
The ADC is the translation layer between the physical electronics world and the digital software world.
```

---

## 6. What is calibration?

Calibration means comparing a sensor reading with known references and creating a mapping.

Example for pH:

```text
known pH 4 solution -> raw ADC value A
known pH 7 solution -> raw ADC value B
known pH 10 solution -> raw ADC value C
```

The firmware then uses those points to estimate pH for unknown water samples.

Calibration answers the question:

```text
When this sensor and this ESP32 produce this raw number, what real-world value should it mean?
```

Without calibration, analog sensor values are often only rough indications.

---

## 7. What is firmware?

Firmware is software that runs directly on a device.

In this project, firmware is the C++ code that runs on the ESP32.

It defines:

- which sensors are used
- which pins are used
- how values are read
- how calibration is applied
- which fPort is used
- how LoRaWAN is configured
- when the ESP32 sleeps

The generated `platformio.ini` does not contain the firmware itself. It configures how the firmware is compiled.

---

## 8. What is PlatformIO?

PlatformIO is a build and upload tool for embedded projects.

It answers questions like:

```text
Which board are we building for?
Which libraries are needed?
Which compile-time flags are active?
Which serial port should be used for upload?
```

In this project, students usually use:

```bash
pio run
pio run -t upload
pio device monitor
```

These commands mean:

| Command | Meaning |
|---|---|
| `pio run` | compile firmware |
| `pio run -t upload` | flash firmware to ESP32 |
| `pio device monitor` | open serial monitor |

---

## 9. What is LoRaWAN?

LoRaWAN is the communication system used to send sensor data over long distances with low energy.

A simplified communication path:

```text
ESP32 + LoRa radio
  -> radio signal
  -> LoRaWAN gateway
  -> TTN network server
  -> TTN application
  -> decoded payload
```

The buoy does not connect to Wi-Fi. It sends a small LoRaWAN packet. A gateway receives it and forwards it to the internet.

This makes LoRaWAN suitable for outdoor environmental sensing.

---

## 10. What is TTN?

TTN means The Things Network. In this course we use The Things Networks Sandbox / TTN as the LoRaWAN backend.

TTN provides:

- device registration
- OTAA join handling
- live data view
- payload formatter
- decoded application data

The ESP32 sends bytes. TTN receives bytes. The payload formatter explains what those bytes mean.

---

## 11. What is a payload formatter?

A payload formatter is JavaScript code installed in TTN.

It converts raw uplink bytes into human-readable fields.

Example:

```text
raw payload bytes -> "350.000000" -> tds_ppm = 350
```

The firmware and the payload formatter must agree on fPorts.

Example:

```text
fPort 4 means TDS
fPort 6 means PH4502C board temperature
```

If TTN uses an old formatter, decoded fields may be wrong even when the device firmware is correct.

---

## 12. What is deep sleep?

Deep sleep is a low-power state of the ESP32.

Instead of running continuously, the device does this:

```text
wake briefly
measure
send
sleep for a long time
```

This is essential for battery and solar operation.

A useful analogy is a lighthouse that only flashes briefly. It does not keep a floodlight on all the time.

---

## 13. Why this matters for managers

Managers often need to make decisions based on technical systems without designing every circuit themselves. This course is about building enough technical understanding to ask better questions.

Examples of better questions:

- Is this value calibrated?
- What does the sensor actually measure?
- Is this absolute data or trend data?
- How often is the value updated?
- What happens when the battery is low?
- What part of the system can fail?
- Is the cloud value decoded correctly?
- Are we seeing a real environmental effect or a sensor artifact?

The goal is not to turn every student into an embedded engineer. The goal is to make technical systems understandable enough to lead projects involving such systems responsibly.
