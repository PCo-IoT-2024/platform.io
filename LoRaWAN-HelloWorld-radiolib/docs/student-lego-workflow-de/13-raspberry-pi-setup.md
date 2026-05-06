# Raspberry-Pi-Setup

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

**Schnittstellen:** Student:in 3 liefert TTN-MQTT-Zugangsdaten; Student:in 5 verwendet Pi-Adresse sowie Datenbank-/Dashboard-Ergebnisse.

Der Raspberry Pi ist der lokale Backend-Server des Bojensystems. Er empfängt MQTT-Nachrichten von TTN über `mqttbridge`, betreibt den lokalen `mqttbroker`, speichert numerische Sensorwerte über `mqttcli` in MariaDB und stellt das Dashboard auf Port 8080 bereit.

## Zielhardware und Betriebssystem

Verwenden Sie diese Kurs-Basis:

| Element | Kurswert |
|---|---|
| Raspberry Pi | Raspberry Pi 4, 4 GB |
| OS | Raspberry Pi OS Lite, Bookworm |
| SSH | enabled |
| user | `water` |
| hostnames | `group1`, `group2`, `group3`, `group4` |
| local MQTT port | `1883` |
| local MQTT authentication | none for the lab setup |
| dashboard port | `8080` |

Jede Gruppe verwendet ihren eigenen Hostnamen. Beispiel für Gruppe 2:

```bash
ssh water@group2.local
```

Wenn `.local`-Namensauflösung nicht funktioniert, verwenden Sie die IP-Adresse, die der Router oder `hostname -I` anzeigt.

## Raspberry Pi OS installieren

Verwenden Sie Raspberry Pi Imager:

1. Raspberry Pi OS Lite, Bookworm, 64-bit wenn verfügbar, auswählen.
2. Benutzer auf `water` setzen.
3. Hostname auf den Gruppennamen setzen, zum Beispiel `group1`.
4. SSH aktivieren.
5. WLAN konfigurieren, falls Ethernet nicht verwendet wird.
6. SD-Karte schreiben und Pi booten.

Ethernet wird für das Backend empfohlen, wenn möglich. WLAN funktioniert ebenfalls, fügt aber eine weitere mögliche Instabilitätsquelle hinzu.

## Erster Login und Update

Per SSH verbinden:

```bash
ssh water@group1.local
```

System aktualisieren:

```bash
sudo apt update
sudo apt full-upgrade -y
sudo reboot
```

Nach dem Neustart erneut verbinden.

## Basispakete installieren

Installieren Sie die grundlegenden Pakete, die vor dem Bauen von SNode.C und MQTTSuite benötigt werden:

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

Das Kapitel zum Bauen von SNode.C und MQTTSuite ergänzt die projektspezifischen Pakete aus den Upstream-README-Installationsabschnitten.

## Verzeichnislayout

Verwenden Sie ein Arbeitsverzeichnis für die Kurssoftware:

```bash
mkdir -p ~/water-buoy
cd ~/water-buoy
```

Die Build-Kapitel verwenden parallele Build-Verzeichnisse neben den geklonten Source Trees:

```text
~/water-buoy/
  snode.c/
  snode.c-build/
  mqttsuite/
  mqttsuite-build/
  config/
  logs/
```

Das entspricht dem gewünschten Build-Stil:

```text
build directory as sibling to the cloned source directory
```

## MariaDB starten und vorbereiten

MariaDB speichert historische Messwerte.

Starten und aktivieren:

```bash
sudo systemctl enable --now mariadb
sudo systemctl status mariadb
```

Kursdatenbank und Benutzer erstellen:

```bash
sudo mariadb
```

Innerhalb von MariaDB:

```sql
CREATE DATABASE water_buoy CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'water_buoy'@'localhost' IDENTIFIED BY 'water-buoy-pass';
GRANT ALL PRIVILEGES ON water_buoy.* TO 'water_buoy'@'localhost';
FLUSH PRIVILEGES;
EXIT;
```

Für das Labor ist dieses einfache Passwort akzeptabel, wenn der Pi in einem vertrauenswürdigen Lehrnetzwerk steht. Für reale Deployments verwenden Sie ein stärkeres Passwort und speichern es nicht in öffentlichen Repositories.

Login testen:

```bash
mariadb -u water_buoy -p water_buoy
```

Dann beenden:

```sql
EXIT;
```

## Measurement-Tabelle erstellen

Das Kurs-Storage-Modell ist bewusst einfach. Es speichert nur numerische Sensorwerte und den fPort, der den Messwerttyp identifiziert.

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

Die Bedeutung von `value` wird durch `f_port` bestimmt:

| fPort | Bedeutung von value |
|---:|---|
| 2 | Wassertemperatur in °C |
| 3 | pH |
| 4 | TDS in ppm |
| 5 | Trübung in NTU |
| 6 | PH4502C-Boardtemperatur in °C |

GPS fPort 1 enthält mehrere Werte und wird nicht durch die obige Ein-Zahlen-Tabelle dargestellt. Er kann später durch eine separate Tabelle `gps_positions` behandelt werden, falls nötig.

MariaDB verlassen:

```sql
EXIT;
```

## Netzwerk prüfen

Nützliche Kommandos:

```bash
hostname
hostname -I
ip addr
```

Für das Labor-Backend sind die wichtigen Ports:

| Port | Zweck |
|---:|---|
| 22 | SSH |
| 1883 | lokaler MQTT-Broker |
| 8080 | mqttcli Dashboard |

## Fertig, wenn

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
