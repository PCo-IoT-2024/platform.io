# MQTT-Daten mit mqttcli in MariaDB speichern

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

**Schnittstellen:** Student:in 3 stellt dekodierte TTN-Payloads bereit; Student:in 5 verwendet denselben `mqttcli`-Prozess für das Web-Dashboard und für Nachweise im Abschlussbericht.

Dieses Kapitel beschreibt, wie das bereitgestellte `mqttcli` aus dem Branch `mqttcli-mariadb` Messwerte in MariaDB speichert.

Das Kursmodell ist:

```text
lokales MQTT-Topic ttn/#
  -> bereitgestelltes mqttcli aus mqttcli-mariadb
  -> MariaDB-Tabellen measurements und gps_positions
  -> mqttcli-Dashboard auf Port 8080
```

Wichtig: `mqttcli` ist im Kurs der Prozess sowohl für die Datenbankspeicherung als auch für das Web-Dashboard. Der Dashboard-Teil ist Bestandteil der Kursversion von `mqttcli` und wird vor Kursbeginn verfügbar sein.

## Datenbankmodell

Die Datenbank verwendet zwei Tabellen:

| Tabelle | Zweck |
|---|---|
| `measurements` | skalare numerische Sensorwerte mit einem Wert pro Zeile |
| `gps_positions` | GPS-Positionsdatensätze mit Latitude, Longitude, Altitude und HDOP |

Dadurch bleibt das Schema einfach. Die meisten Sensoren erzeugen pro Uplink genau eine Zahl. GPS erzeugt mehrere Werte und bekommt daher eine eigene Tabelle.

## Tabelle für skalare Sensorwerte: measurements

Die Tabelle `measurements` speichert einen numerischen Wert pro Zeile und enthält zusätzlich `f_port`, damit die Bedeutung des Werts eindeutig bleibt.

```text
device_id + f_port + value + received_at
```

Die Bedeutung von `value` ergibt sich aus `f_port`.

| fPort | Gespeicherter numerischer Wert |
|---:|---|
| 2 | Wassertemperatur in °C |
| 3 | pH |
| 4 | TDS in ppm |
| 5 | Trübung in NTU |
| 6 | PH4502C-Boardtemperatur in °C |

Erstellen Sie die Tabelle:

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

## GPS-Tabelle: gps_positions

GPS fPort 1 enthält mehrere Werte. Er wird in einer eigenen Tabelle gespeichert.

| Spalte | Bedeutung |
|---|---|
| `latitude` | GPS-Breitengrad in Dezimalgrad |
| `longitude` | GPS-Längengrad in Dezimalgrad |
| `altitude` | GPS-Höhe, falls verfügbar |
| `hdop` | horizontal dilution of precision |

Erstellen Sie die Tabelle:

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

Prüfen Sie beide Tabellen:

```sql
DESCRIBE measurements;
DESCRIBE gps_positions;
EXIT;
```

## Erforderliches mqttcli-Verhalten

Der bereitgestellte `mqttcli`-Storage-/Dashboard-Prozess muss für jeden über die Bridge ankommenden TTN-Uplink Folgendes tun:

```text
1. lokales MQTT-Topic ttn/# abonnieren
2. TTN-Uplink-JSON parsen
3. Application ID lesen
4. Device ID lesen
5. fPort lesen
6. für fPort 1 eine Zeile in gps_positions einfügen
7. für fPorts 2..6 eine Zeile in measurements einfügen
8. Dashboard-Ansicht auf Port 8080 bereitstellen
```

Die gespeicherten Werte müssen dekodierte Messwerte sein, nicht ESP32-Rohwerte des ADC und keine vollständigen JSON-Blobs.

## mqttcli Storage/Dashboard starten

Verwenden Sie das bereitgestellte `mqttcli`-Binary aus dem Branch `mqttcli-mariadb`. Die genaue Kommandozeilen-Syntax muss zur Kursimplementierung passen, die vor dem Workshop verfügbar ist.

Dokumentierte Kursabsicht:

```text
mqttcli abonniert ttn/# auf localhost:1883
mqttcli fügt GPS-Werte in water_buoy.gps_positions ein
mqttcli fügt skalare Sensorwerte in water_buoy.measurements ein
mqttcli stellt das Dashboard auf Port 8080 unter / bereit
```

Ersetzen Sie diesen Platzhalter vor Kursbeginn durch das exakte Kommando aus der finalen Ausgabe von `mqttcli --help`:

```bash
mqttcli <course-storage-dashboard-options>
```

Ersetzen Sie das nicht durch geratene Syntax. Das Kommando muss aus der implementierten Version `mqttcli-mariadb` stammen.

## Lokalen MQTT-Input prüfen

Bevor Sie die Datenbankeinfügung testen, weisen Sie zuerst nach, dass der lokale Broker TTN-Nachrichten empfängt:

```bash
mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

Wenn hier keine JSON-Nachrichten ankommen, kann die MariaDB-Speicherung noch nicht funktionieren. Gehen Sie zurück zum MQTTBridge-Kapitel.

## Gespeicherte skalare Messwerte prüfen

Nachdem der bereitgestellte `mqttcli`-Storage-/Dashboard-Prozess läuft und ein skalarer Uplink ankommt, prüfen Sie:

```bash
mariadb -u water_buoy -p water_buoy
```

```sql
SELECT id, received_at, application_id, device_id, f_port, value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

## Gespeicherte GPS-Positionen prüfen

Nachdem ein GPS-Uplink ankommt, prüfen Sie:

```sql
SELECT id, received_at, application_id, device_id, latitude, longitude, altitude, hdop
FROM gps_positions
ORDER BY id DESC
LIMIT 20;
```

## Nach Sensortyp abfragen

TDS-Verlauf:

```sql
SELECT received_at, value AS tds_ppm
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

GPS-Verlauf:

```sql
SELECT received_at, latitude, longitude, altitude, hdop
FROM gps_positions
WHERE device_id = 'buoy-01'
ORDER BY received_at DESC
LIMIT 100;
```

## Fertig, wenn

```text
[ ] mqttcli zeigt über die Bridge kommende TTN-Nachrichten auf dem lokalen Topic ttn/#.
[ ] Der bereitgestellte mqttcli-Storage-/Dashboard-Prozess läuft.
[ ] Die Tabelle measurements erhält numerische skalare Sensorwerte.
[ ] Die Tabelle gps_positions erhält GPS-Zeilen für fPort 1.
[ ] SELECT-Abfragen zeigen aktuelle Sensorwerte und GPS-Positionen.
[ ] Die Studierenden können erklären, warum GPS getrennt von skalaren Messwerten gespeichert wird.
```
