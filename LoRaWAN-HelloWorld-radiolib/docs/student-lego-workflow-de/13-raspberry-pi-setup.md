# Raspberry-Pi-Setup

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

**Schnittstellen:** Student:in 3 liefert TTN-MQTT-Zugangsdaten; Student:in 5 verwendet Pi-Adresse, Datenbank- und Dashboard-Ergebnisse.

Der Raspberry Pi ist der lokale Backend-Server des Bojensystems. Er empfängt MQTT-Nachrichten aus TTN über `mqttbridge`, betreibt den lokalen `mqttbroker`, speichert Messwerte über `mqttcli` in MariaDB und stellt das Dashboard auf Port 8080 bereit.

## Zielhardware und Betriebssystem

Kurs-Basis:

| Element | Kurswert |
|---|---|
| Raspberry Pi | Raspberry Pi 4, 4 GB |
| OS | Raspberry Pi OS Lite, Bookworm |
| SSH | aktiviert |
| Benutzer | `water` |
| Hostnames | `group1`, `group2`, `group3`, `group4` |
| lokaler MQTT-Port | `1883` |
| lokale MQTT-Authentifizierung | keine im Laborsetup |
| Dashboard-Port | `8080` |

Beispiel für Gruppe 2:

```bash
ssh water@group2.local
```

Falls `.local` nicht funktioniert, die IP-Adresse verwenden.

## Raspberry Pi OS installieren

Mit Raspberry Pi Imager:

1. Raspberry Pi OS Lite Bookworm auswählen.
2. Benutzer `water` setzen.
3. Hostname auf Gruppennamen setzen, z. B. `group1`.
4. SSH aktivieren.
5. WLAN konfigurieren, falls kein Ethernet verwendet wird.
6. SD-Karte schreiben und Pi starten.

Ethernet ist für das Backend empfehlenswert, wenn verfügbar.

## Erster Login und Update

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

Projektabhängige Pakete werden im Kapitel zu SNode.C und MQTTSuite ergänzt.

## Verzeichnisstruktur

```bash
mkdir -p ~/water-buoy
cd ~/water-buoy
```

Verwendete Struktur:

```text
~/water-buoy/
  snode.c/
  snode.c-build/
  mqttsuite/
  mqttsuite-build/
  config/
  logs/
```

## MariaDB starten und vorbereiten

```bash
sudo systemctl enable --now mariadb
sudo systemctl status mariadb
```

Datenbank und Benutzer erstellen:

```bash
sudo mariadb
```

In MariaDB:

```sql
CREATE DATABASE water_buoy CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'water_buoy'@'localhost' IDENTIFIED BY 'water-buoy-pass';
GRANT ALL PRIVILEGES ON water_buoy.* TO 'water_buoy'@'localhost';
FLUSH PRIVILEGES;
EXIT;
```

Login testen:

```bash
mariadb -u water_buoy -p water_buoy
```

Dann:

```sql
EXIT;
```

## Tabellen erstellen

Das Kursdatenmodell verwendet zwei Tabellen.

```bash
mariadb -u water_buoy -p water_buoy
```

Skalare Messwerte:

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

GPS-Positionen:

```sql
CREATE TABLE IF NOT EXISTS gps_positions (
  id BIGINT AUTO_INCREMENT PRIMARY KEY,
  received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  application_id VARCHAR(128) NOT NULL,
  device_id VARCHAR(128) NOT NULL,
  latitude DOUBLE NOT NULL,
  longitude DOUBLE NOT NULL,
  altitude DOUBLE,
  hdop DOUBLE
);

CREATE INDEX idx_gps_positions_device_time
ON gps_positions (device_id, received_at);
```

Prüfen:

```sql
DESCRIBE measurements;
DESCRIBE gps_positions;
EXIT;
```

## Netzwerk prüfen

Nützliche Befehle:

```bash
hostname
hostname -I
ip addr
```

Wichtige Ports:

| Port | Zweck |
|---:|---|
| 22 | SSH |
| 1883 | lokaler MQTT-Broker |
| 8080 | mqttcli Dashboard |

## Fertig, wenn

```text
[ ] SSH zu water@groupN.local funktioniert.
[ ] Pi hat Internetzugang.
[ ] Build-Werkzeuge sind installiert.
[ ] MariaDB läuft.
[ ] Datenbank water_buoy existiert.
[ ] Benutzer water_buoy kann sich anmelden.
[ ] Tabellen measurements und gps_positions existieren.
[ ] Verzeichnis ~/water-buoy existiert.
```
