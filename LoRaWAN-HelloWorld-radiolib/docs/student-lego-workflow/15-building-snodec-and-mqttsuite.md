# Building SNode.C and MQTTSuite on the Raspberry Pi

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 5 uses the resulting dashboard/mqttcli service; Student 3 provides TTN MQTT credentials for the bridge chapter.

This chapter builds the C++ backend tools on the Raspberry Pi.

| Project | Repository | Branch | Role |
|---|---|---|---|
| SNode.C | `https://github.com/SNodeC/snode.c` | `master` | C++ networking / HTTP framework |
| MQTTSuite | `https://github.com/SNodeC/mqttsuite` | `mqttcli-mariadb` | MQTT broker, bridge, CLI, dashboard/storage workflow |

The build directories are siblings of the cloned source directories, as used in the course setup.

## Install project build packages

Start with the packages from the Raspberry Pi setup chapter. Then install the development packages required by SNode.C/MQTTSuite on Raspberry Pi OS Bookworm:

```bash
sudo apt update
sudo apt install -y \
  git \
  build-essential \
  cmake \
  pkg-config \
  libssl-dev \
  zlib1g-dev \
  libmariadb-dev \
  mariadb-client
```

If CMake reports an additional missing package, install the matching Debian `-dev` package and rerun CMake.

## Build SNode.C

Clone the repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/snode.c.git snode.c
cd snode.c
git checkout master
```

Create the sibling build directory and build:

```bash
cd ~/water-buoy
mkdir -p snode.c-build
cd snode.c-build
cmake ../snode.c
make
```

You are done with SNode.C when the build finishes without errors.

## Build MQTTSuite

Clone the repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/mqttsuite.git mqttsuite
cd mqttsuite
git checkout mqttcli-mariadb
```

Create the sibling build directory and build:

```bash
cd ~/water-buoy
mkdir -p mqttsuite-build
cd mqttsuite-build
cmake ../mqttsuite
make
```

You are done with MQTTSuite when the build finishes without errors.

## Locate the binaries

After the build, locate the tools:

```bash
cd ~/water-buoy
find mqttsuite-build -type f -perm -111 \( -name 'mqttbroker' -o -name 'mqttbridge' -o -name 'mqttcli' \)
```

Create a convenient `bin` directory and symbolic links. Adjust the source paths if the `find` command shows different locations:

```bash
mkdir -p ~/water-buoy/bin
ln -sf "$(find ~/water-buoy/mqttsuite-build -type f -perm -111 -name mqttbroker | head -n1)" ~/water-buoy/bin/mqttbroker
ln -sf "$(find ~/water-buoy/mqttsuite-build -type f -perm -111 -name mqttbridge | head -n1)" ~/water-buoy/bin/mqttbridge
ln -sf "$(find ~/water-buoy/mqttsuite-build -type f -perm -111 -name mqttcli | head -n1)" ~/water-buoy/bin/mqttcli
```

Check:

```bash
~/water-buoy/bin/mqttbroker --help
~/water-buoy/bin/mqttbridge --help
~/water-buoy/bin/mqttcli --help
```

## What each MQTTSuite tool does

| Tool | Role in the course |
|---|---|
| `mqttbroker` | local MQTT broker on port 1883 |
| `mqttbridge` | connects TTN MQTT to the local broker |
| `mqttcli` | subscribes/publishes MQTT messages, stores values in MariaDB, and serves the dashboard on port 8080 |

The final backend chain is:

```text
TTN MQTT
  -> mqttbridge
  -> local mqttbroker on groupN.local:1883
  -> mqttcli storage/dashboard
  -> MariaDB + browser dashboard
```

## Done when

```text
[ ] SNode.C is cloned on branch master.
[ ] SNode.C builds in ~/water-buoy/snode.c-build.
[ ] MQTTSuite is cloned on branch mqttcli-mariadb.
[ ] MQTTSuite builds in ~/water-buoy/mqttsuite-build.
[ ] ~/water-buoy/bin/mqttbroker exists.
[ ] ~/water-buoy/bin/mqttbridge exists.
[ ] ~/water-buoy/bin/mqttcli exists.
[ ] All three tools print help output.
```
