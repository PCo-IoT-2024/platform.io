# PlatformIO INI Generator

This directory contains a small SNode.C/express-based teaching web application.

The app helps students create an individual `platformio.ini` for the configurable lego firmware without editing PlatformIO inheritance blocks manually.

## What the App Does

Students open a browser UI, enter:

- environment name
- LoRaWAN version: `1.1.0` or `1.0.1`
- DevEUI
- AppKey
- NwkKey, only for LoRaWAN 1.1.0
- radio module: SX1262 or SX1276
- selected sensors
- pin mapping, pre-filled with the validated course defaults

Then they press **Generate platformio.ini**.

The generated INI is shown in the browser and can be downloaded.

No keys are stored by the server. The current implementation generates the INI in the browser.

## Default Course Pins

| Function | GPIO |
|---|---:|
| GPS RX | 16 |
| GPS TX | 17 |
| DS18B20 temperature | 27 |
| pH analog | 34 |
| pH temperature | 35 |
| TDS analog | 32 |
| Turbidity analog | 33 |
| LoRa module bitmap | `5, 2, 14, 4` |

## Build the Web App

This example assumes SNode.C is installed system-wide.

```bash
cd LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator
cmake -S . -B build -G Ninja
cmake --build build
```

## Run

```bash
./build/platformio-ini-generator --web-root ./www
```

Then open:

```text
http://localhost:8080/
```

Depending on your SNode.C installation and configuration defaults, you may also need to pass a port option accepted by your local SNode.C build.

## Using the Generated INI

1. Download the generated file as `platformio.ini`.
2. Put it into `LoRaWAN-HelloWorld-radiolib/` or copy it into a fresh student project.
3. Build:

```bash
pio run -e <environment-name>
```

4. Upload:

```bash
pio run -e <environment-name> -t upload
```

5. Monitor serial output:

```bash
pio device monitor
```

6. In TTN, install the payload formatter from:

```text
LoRaWAN-HelloWorld-radiolib/ttn/payload-formatter.js
```
