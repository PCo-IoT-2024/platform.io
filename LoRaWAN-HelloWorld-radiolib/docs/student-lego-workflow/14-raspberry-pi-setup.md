# Raspberry Pi Setup

The Raspberry Pi is the local backend server of the buoy system. It receives MQTT messages from TTN through `mqttbridge`, runs the local `mqttbroker`, stores data in MariaDB through `mqttcli`, and serves the dashboard.

This chapter prepares the Pi for that role.

---

## Role of the Raspberry Pi

The Pi is not the buoy. The buoy is the ESP32 device. The Pi is the local server.

| Device | Role |
|---|---|
| ESP32 buoy | reads sensors and sends LoRaWAN uplinks |
| TTN cloud | receives and decodes LoRaWAN uplinks |
| Raspberry Pi | local MQTT/database/dashboard backend |
| student laptop | development, browser, SSH terminal |

The Pi should usually be connected by Ethernet if possible. Wi-Fi also works, but Ethernet is more stable for a lab backend.

---

## Install Raspberry Pi OS

Recommended image:

```text
Raspberry Pi OS Lite, 64-bit
```

Lite is enough because the Pi is used as a server. A desktop GUI is not required.

Basic installation steps:

1. Install Raspberry Pi Imager on the laptop.
2. Choose Raspberry Pi OS Lite 64-bit.
3. Configure hostname, user, password, Wi-Fi if needed, and SSH.
4. Write the image to the SD card.
5. Insert the SD card into the Pi.
6. Boot the Pi.

Suggested hostname:

```text
waterbuoy-pi
```

---

## Connect by SSH

From the laptop:

```bash
ssh <user>@waterbuoy-pi.local
```

or use the IP address:

```bash
ssh <user>@192.168.x.y
```

If the hostname does not resolve, find the IP address in the router or with a network scanner.

You are done when you have a shell prompt on the Pi.

---

## Update the system

Run:

```bash
sudo apt update
sudo apt full-upgrade -y
sudo reboot
```

Reconnect after reboot:

```bash
ssh <user>@waterbuoy-pi.local
```

---

## Install basic tools

Install tools required for building and running the backend stack:

```bash
sudo apt install -y \
  git \
  build-essential \
  cmake \
  ninja-build \
  pkg-config \
  curl \
  ca-certificates \
  mariadb-server \
  mariadb-client
```

Depending on the exact SNode.C and MQTTSuite build configuration, additional development packages may be required later. The build chapters will mention the project-specific dependencies.

---

## Suggested directory layout

Use one working directory for the course software:

```bash
mkdir -p ~/water-buoy
cd ~/water-buoy
```

Suggested structure:

```text
~/water-buoy/
  snode.c/
  mqttsuite/
  config/
  logs/
  dashboard/
```

This keeps course files separate from system directories.

---

## Install and secure MariaDB basics

MariaDB is used to store measurements persistently.

Start and enable MariaDB:

```bash
sudo systemctl enable --now mariadb
sudo systemctl status mariadb
```

Optional basic security step:

```bash
sudo mariadb-secure-installation
```

For a teaching setup, the important minimum is that MariaDB runs and students can create a database and user.

---

## Create a database for the course

Open MariaDB as root through sudo:

```bash
sudo mariadb
```

Create database and user:

```sql
CREATE DATABASE water_buoy CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'water_buoy'@'localhost' IDENTIFIED BY 'change-this-password';
GRANT ALL PRIVILEGES ON water_buoy.* TO 'water_buoy'@'localhost';
FLUSH PRIVILEGES;
EXIT;
```

Test:

```bash
mariadb -u water_buoy -p water_buoy
```

You are done when the login works.

---

## Network and firewall notes

For a simple lab setup, the Pi should be reachable from the student laptop in the same local network.

Useful checks:

```bash
hostname
hostname -I
ip addr
```

If the dashboard will be accessed from other computers, note the Pi IP address.

If a firewall is enabled later, allow the ports used by:

| Service | Typical purpose |
|---|---|
| SSH | remote shell |
| mqttbroker | local MQTT broker |
| dashboard HTTP | browser dashboard |

The exact ports depend on your configuration.

---

## You are done when...

The Raspberry Pi setup is ready when:

```text
[ ] You can SSH into the Pi.
[ ] The Pi has internet access.
[ ] git, cmake, compiler tools, and MariaDB are installed.
[ ] MariaDB is running.
[ ] The water_buoy database exists.
[ ] The water_buoy database user can log in.
[ ] There is a ~/water-buoy working directory.
```

Do not continue with MQTTSuite until this checklist is complete.
