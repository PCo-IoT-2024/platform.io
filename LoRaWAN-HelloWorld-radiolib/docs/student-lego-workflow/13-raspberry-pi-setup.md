# Raspberry Pi Setup

**Primary workstream:** Student 4 — Raspberry Pi backend, MQTT, and MariaDB

**Interfaces:** Student 3 provides TTN MQTT credentials; Student 5 uses the Pi address and database/dashboard results.

The Raspberry Pi is the local backend server of the buoy system. It receives MQTT messages from TTN through `mqttbridge`, runs the local `mqttbroker`, stores numeric sensor values in MariaDB through `mqttcli`, and serves the dashboard on port 8080.

## Target hardware and operating system

Use this course baseline:

| Item | Course value |
|---|---|
| Raspberry Pi | Raspberry Pi 4, 4 GB |
| OS | Raspberry Pi OS Lite, Bookworm |
| SSH | enabled |
| user | `water` |
| hostnames | `group1`, `group2`, `group3`, `group4` |
| local MQTT port | `1883` |
| local MQTT authentication | none for the lab setup |
| dashboard port | `8080` |

Each group uses its own hostname. Example for group 2:

```bash
ssh water@group2.local
```

If `.local` name resolution does not work, use the IP address shown by the router or by `hostname -I`.

## Install Raspberry Pi OS

Use Raspberry Pi Imager:

1. Choose Raspberry Pi OS Lite, Bookworm, 64-bit if available.
2. Set user to `water`.
3. Set the hostname to the group name, for example `group1`.
4. Enable SSH.
5. Configure Wi-Fi if Ethernet is not used.
6. Write the SD card and boot the Pi.

Ethernet is recommended for the backend if possible. Wi-Fi also works, but it adds one more possible source of instability.

## First login and update

Connect by SSH:

```bash
ssh water@group1.local
```

Update the system:

```bash
sudo apt update
sudo apt full-upgrade -y
sudo reboot
```

Reconnect after reboot.

## Install baseline packages

Install the basic packages needed before building SNode.C and MQTTSuite:

```bash
sudo apt install -y \
  git \
  build-essential \
  cmake \
  pkg-config \
  curl \
  ca-certificates \
  mariadb-server \
  mariadb-client
```

The SNode.C and MQTTSuite build chapter adds the project-specific packages from the upstream README installation sections.

## Directory layout

Use one working directory for the course software:

```bash
mkdir -p ~/water-buoy
cd ~/water-buoy
```

The build chapters use sibling build directories beside the cloned source trees:

```text
~/water-buoy/
  snode.c/
  snode.c-build/
  mqttsuite/
  mqttsuite-build/
  config/
  logs/
```

This mirrors the requested build style:

```text
build directory as sibling to the cloned source directory
```

## Start and prepare MariaDB

MariaDB stores historical measurement values.

Start and enable it:

```bash
sudo systemctl enable --now mariadb
sudo systemctl status mariadb
```

Create the course database and user:

```bash
sudo mariadb
```

Inside MariaDB:

```sql
CREATE DATABASE water_buoy CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'water_buoy'@'localhost' IDENTIFIED BY 'water-buoy-pass';
GRANT ALL PRIVILEGES ON water_buoy.* TO 'water_buoy'@'localhost';
FLUSH PRIVILEGES;
EXIT;
```

For the lab this simple password is acceptable if the Pi is on a trusted teaching network. For real deployments, use a stronger password and do not store it in public repositories.

Test login:

```bash
mariadb -u water_buoy -p water_buoy
```

Then exit:

```sql
EXIT;
```

## Create the measurement table

The course storage model is intentionally simple. It stores only numeric sensor values and the fPort that identifies the measurement type.

```bash
mariadb -u water_buoy -p water_buoy
```

```sql
CREATE TABLE IF NOT EXISTS measurements (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128) NOT NULL,
  device_id VARCHAR(128) NOT NULL,
  f_port INT NOT NULL,
  value DOUBLE NOT NULL
);

CREATE INDEX idx_measurements_device_time
ON measurements (device_id, received_at);

CREATE INDEX idx_measurements_fport_time
ON measurements (f_port, received_at);
```

The meaning of `value` is determined by `f_port`:

| fPort | Value meaning |
|---:|---|
| 2 | water temperature in °C |
| 3 | pH |
| 4 | TDS in ppm |
| 5 | turbidity in NTU |
| 6 | PH4502C board temperature in °C |

GPS fPort 1 contains multiple values and is not represented by the single-number table above. It can be handled later by a separate `gps_positions` table if needed.

Exit MariaDB:

```sql
EXIT;
```

## Network checks

Useful commands:

```bash
hostname
hostname -I
ip addr
```

For the lab backend, the important ports are:

| Port | Purpose |
|---:|---|
| 22 | SSH |
| 1883 | local MQTT broker |
| 8080 | mqttcli dashboard |

## Done when

```text
[ ] You can SSH into water@groupN.local.
[ ] The Pi has internet access.
[ ] Basic build tools are installed.
[ ] MariaDB is running.
[ ] Database water_buoy exists.
[ ] User water_buoy can log in.
[ ] Table measurements exists.
[ ] Directory ~/water-buoy exists.
```
