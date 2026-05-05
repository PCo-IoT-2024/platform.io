# Building SNode.C and MQTTSuite on the Raspberry Pi

This chapter prepares the software tools that run on the Raspberry Pi backend.

Two projects are important:

| Project | Role |
|---|---|
| SNode.C | C++ networking / HTTP framework used for backend and dashboard services |
| MQTTSuite | MQTT tools: broker, bridge, CLI, and storage-related workflows |

The course backend uses the `mqttcli-mariadb` branch of MQTTSuite.

---

## Important note about repository URLs

This chapter uses placeholder repository URLs. Replace them with the official course repositories if they differ.

Suggested placeholders:

```text
https://github.com/SNodeC/snode.c.git
https://github.com/SNodeC/mqttsuite.git
```

The branch name that matters for this workflow is:

```text
mqttcli-mariadb
```

---

## Install build dependencies

Start from the Raspberry Pi setup chapter. Then install typical build dependencies:

```bash
sudo apt update
sudo apt install -y \
  git \
  build-essential \
  cmake \
  ninja-build \
  pkg-config \
  libssl-dev \
  zlib1g-dev \
  libmariadb-dev \
  mariadb-client
```

Depending on the current state of the repositories, additional packages may be needed. If CMake reports a missing package, install the corresponding `-dev` package.

---

## Build SNode.C

Clone SNode.C:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/snode.c.git
cd snode.c
```

Create a build directory:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If the project provides tests or examples, run a minimal one to check that the build works.

You are done with this step when SNode.C builds without errors.

---

## What SNode.C is used for

SNode.C is not the ESP32 firmware. It runs on the Raspberry Pi or a development computer.

In this course it is used for the local web/backend part:

```text
browser -> SNode.C HTTP server -> MariaDB -> stored measurements
```

The dashboard can use SNode.C to serve:

- static HTML/CSS/JavaScript
- REST API endpoints
- database query results
- current and historical measurements

---

## Build MQTTSuite

Clone MQTTSuite:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/mqttsuite.git
cd mqttsuite
```

Checkout the required branch:

```bash
git checkout mqttcli-mariadb
```

Build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

After building, locate the binaries. Depending on the project layout, they may be in a build subdirectory. Expected tools include:

```text
mqttbroker
mqttbridge
mqttcli
```

Use `find` if needed:

```bash
find build -type f -name 'mqtt*' -perm -111
```

---

## What each MQTTSuite tool does

| Tool | Role |
|---|---|
| mqttbroker | local MQTT broker on the Raspberry Pi |
| mqttbridge | connects TTN MQTT to local MQTT |
| mqttcli | command-line MQTT client; used for testing and storage workflows |

A simple local test path is:

```text
mqttcli publish -> mqttbroker -> mqttcli subscribe
```

The real backend path is:

```text
TTN MQTT -> mqttbridge -> mqttbroker -> mqttcli -> MariaDB
```

---

## Recommended binary directory

For a teaching setup, it is often useful to define shell variables or copy wrapper scripts instead of installing globally immediately.

Example:

```bash
mkdir -p ~/water-buoy/bin
```

Then either copy binaries or create symbolic links from the build output.

Example placeholder:

```bash
ln -sf ~/water-buoy/mqttsuite/build/path/to/mqttbroker ~/water-buoy/bin/mqttbroker
ln -sf ~/water-buoy/mqttsuite/build/path/to/mqttbridge ~/water-buoy/bin/mqttbridge
ln -sf ~/water-buoy/mqttsuite/build/path/to/mqttcli ~/water-buoy/bin/mqttcli
```

Adjust the paths to the actual build output.

---

## You are done when...

This chapter is complete when:

```text
[ ] SNode.C repository is cloned.
[ ] SNode.C builds successfully.
[ ] MQTTSuite repository is cloned.
[ ] MQTTSuite branch mqttcli-mariadb is checked out.
[ ] mqttbroker binary exists.
[ ] mqttbridge binary exists.
[ ] mqttcli binary exists.
```

Do not continue with the local broker chapter until the MQTTSuite binaries are available.
