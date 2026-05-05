# Building SNode.C and MQTTSuite on the Raspberry Pi

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 5 uses the resulting dashboard/mqttcli service; Student 3 provides TTN MQTT credentials for the bridge chapter.

This chapter builds and installs the C++ backend tools on the Raspberry Pi.

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

## Build and install SNode.C

Clone the repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/snode.c.git snode.c
cd snode.c
git checkout master
```

Create the sibling build directory, configure, build, and install:

```bash
cd ~/water-buoy
mkdir -p snode.c-build
cd snode.c-build
cmake ../snode.c
make
sudo make install
```

After installing SNode.C, update the dynamic linker cache before configuring MQTTSuite:

```bash
sudo ldconfig
```

This step is important because MQTTSuite links against installed SNode.C libraries.

You are done with SNode.C when the build and installation finish without errors and `sudo ldconfig` has been executed.

## Build and install MQTTSuite

Clone the repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/mqttsuite.git mqttsuite
cd mqttsuite
git checkout mqttcli-mariadb
```

Create the sibling build directory, configure, build, and install:

```bash
cd ~/water-buoy
mkdir -p mqttsuite-build
cd mqttsuite-build
cmake ../mqttsuite
make
sudo make install
```

The binaries are installed into system-wide directories by `sudo make install`. Therefore students do not need to search for the binaries or create symbolic links manually.

Check the installed tools:

```bash
mqttbroker --help
mqttbridge --help
mqttcli --help
```

If a command is not found, open a new shell and check the installation prefix used by CMake. On a normal system-wide install, the commands should be available through the shell `PATH`.

## What each MQTTSuite tool does

| Tool | Role in the course |
|---|---|
| `mqttbroker` | local MQTT broker on port 1883 |
| `mqttbridge` | connects TTN MQTT to the local broker using `bridge-config.json` |
| `mqttcli` | subscribes/publishes MQTT messages and, in the course branch, stores values in MariaDB and serves the dashboard on port 8080 |

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
[ ] SNode.C is installed with sudo make install.
[ ] sudo ldconfig has been executed after installing SNode.C.
[ ] MQTTSuite is cloned on branch mqttcli-mariadb.
[ ] MQTTSuite builds in ~/water-buoy/mqttsuite-build.
[ ] MQTTSuite is installed with sudo make install.
[ ] mqttbroker --help works.
[ ] mqttbridge --help works.
[ ] mqttcli --help works.
```
