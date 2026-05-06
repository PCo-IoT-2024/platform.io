# Aufgaben und Deliverables

Dieses Kapitel fasst die individuellen und gemeinsamen Deliverables zusammen. Es soll während der Arbeit mit dem Handbuch als Checkliste verwendet werden.

Das Projekt wird von Gruppen mit fünf Studierenden durchgeführt. Jede:r Studierende verantwortet einen Workstream, aber das Endergebnis muss als ein integriertes Bojensystem funktionieren.

## Deliverables der Workstreams

| Student:in | Workstream | Haupt-Deliverable |
|---:|---|---|
| 1 | ESP32-Firmware und LoRaWAN-Gerät | Firmware baut, wird geflasht, joint TTN und sendet erwartete fPorts |
| 2 | Sensoren und Kalibrierung | Verdrahtung, rohe ADC-Kalibrierwerte, plausible Sensorwerte |
| 3 | TTN-Cloud und Payload Formatter | TTN Application/Device, Formatter, MQTT API Key |
| 4 | Raspberry-Pi-Backend, MQTT, MariaDB | Broker, Bridge, bereitgestelltes mqttcli Storage/Dashboard, Datenbankzeilen |
| 5 | Dashboard, Dokumentation, finaler Test | Dashboard auf Port 8080, GPS-/Skalarwertnachweise, Abschlussbericht |

## Aufgabe 1 — Repository und Branch einrichten

Ziel: einen reproduzierbaren Arbeitsstand herstellen.

Schritte:

1. Repository wie vorgegeben forken oder klonen.
2. Den Kurs-Branch verwenden:

```text
course/lego-configurable
```

3. Das lokale Arbeitsverzeichnis dokumentieren.
4. Keine Secrets committen, also keine AppKeys, NwkKeys, TTN API Keys oder Datenbankpasswörter.

Deliverable:

```text
screenshot or terminal log showing repository, branch, and working directory
```

## Aufgabe 2 — Gruppenarbeitsvertrag

Ziel: Verantwortlichkeiten vor Beginn der technischen Arbeit festlegen.

Schritte:

1. Die fünf Workstreams zuordnen.
2. Gruppen-Hostname vereinbaren: `group1`, `group2`, `group3` oder `group4`.
3. TTN Application ID und Device ID vereinbaren.
4. Lokale MQTT-Topic-Konvention vereinbaren.
5. Festlegen, wer die finalen Nachweise sammelt.

Deliverable:

```text
group table with names, workstreams, interfaces, and responsibilities
```

## Aufgabe 3 — TTN-Cloud-Setup

Owner: Student:in 3.

Ziel: die Cloud-Seite vorbereiten.

Schritte:

1. TTN Application erstellen.
2. TTN End Device erstellen.
3. Richtige Region und LoRaWAN-Version auswählen.
4. DevEUI, JoinEUI, AppKey und bei Bedarf NwkKey erstellen oder dokumentieren.
5. Generierten Payload Formatter installieren.
6. TTN MQTT API Key erstellen.
7. TTN MQTT Host, Username und Uplink-Topic dokumentieren.

Kurswerte:

```text
TTN MQTT host: eu1.cloud.thethings.network
username format: <application-id>@ttn
uplink topic: v3/<application-id>@ttn/devices/+/up
```

Deliverable:

```text
TTN live-data screenshot with decoded payload and separate secure record of MQTT credentials
```

## Aufgabe 4 — Generator, Firmware und ESP32 flashen

Owner: Student:in 1, mit Inputs von Studierenden 2 und 3.

Ziel: eine funktionierende ESP32-Firmware-Konfiguration erstellen.

Schritte:

1. Generator öffnen.
2. TTN-Zugangsdaten eintragen.
3. Funkmodul, LoRaWAN-Version und Region auswählen.
4. Sensor-Pins eintragen.
5. Kalibrierwerte von Student:in 2 eintragen.
6. `platformio.ini` generieren.
7. `payload-formatter.js` generieren.
8. Bauen und flashen:

```bash
pio run
pio run -t upload
pio device monitor
```

Deliverable:

```text
platformio.ini, payload formatter, successful build/upload log, serial monitor output
```

## Aufgabe 5 — Sensorverdrahtung und Kalibrierung

Owner: Student:in 2.

Ziel: Messwerte plausibel machen.

Schritte:

1. Alle Sensoren verdrahten.
2. ESP32-Pin-Mapping dokumentieren.
3. Kalibriermodus für analoge Sensoren starten.
4. Rohe ADC-Werte dokumentieren.
5. Rohe ADC-Kalibrierwerte in den Generator eintragen.
6. Normale Messwerte nach dem erneuten Flashen prüfen.

Wichtige Regel:

```text
Use raw ESP32 ADC values for analog calibration, not voltages.
```

Deliverable:

```text
wiring table, calibration table, raw ADC values, notes about references and plausibility
```

## Aufgabe 6 — fPort-Rotation und dekodierte Payloads prüfen

Owner: Studierende 1 und 3.

Ziel: beweisen, dass Firmware und TTN Formatter zusammenpassen.

Erwartete fPort-Zuordnung:

| fPort | Bedeutung | Feld |
|---:|---|---|
| 1 | GPS | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | Wassertemperatur | `temperature_c` |
| 3 | pH | `ph_level` |
| 4 | TDS | `tds_ppm` |
| 5 | Trübung | `turbidity_ntu` |
| 6 | PH4502C-Boardtemperatur | `ph_board_temperature_c` |

Deliverable:

```text
table with wake number, fPort, sensor, TTN decoded field, and example value
```

## Aufgabe 7 — Raspberry Pi vorbereiten

Owner: Student:in 4.

Ziel: den Backend-Computer vorbereiten.

Schritte:

1. Raspberry Pi OS Lite Bookworm installieren.
2. Benutzer auf `water` setzen.
3. Hostname auf Gruppennummer setzen, zum Beispiel `group1`.
4. SSH aktivieren.
5. Basispakete installieren.
6. `~/water-buoy` erstellen.
7. MariaDB installieren und starten.
8. Datenbank `water_buoy`, Benutzer `water_buoy`, Tabelle `measurements` und Tabelle `gps_positions` erstellen.

Deliverable:

```text
SSH proof, package installation proof, MariaDB DESCRIBE output for measurements and gps_positions
```

## Aufgabe 8 — SNode.C und MQTTSuite bauen

Owner: Student:in 4.

Ziel: die lokalen Backend-Werkzeuge bauen.

Repositories:

```text
SNode.C:    https://github.com/SNodeC/snode.c      branch master
MQTTSuite:  https://github.com/SNodeC/mqttsuite    branch mqttcli-mariadb
```

Build-Stil:

```text
sibling build directory beside each cloned source directory
```

Deliverable:

```text
successful build logs and working mqttbroker, mqttbridge, mqttcli binaries
```

## Aufgabe 9 — Lokaler MQTT-Broker und TTN-Bridge

Owner: Student:in 4, mit Zugangsdaten von Student:in 3.

Ziel: TTN-Uplinks zu lokalem MQTT weiterleiten.

Schritte:

1. Lokalen `mqttbroker` auf Port 1883 starten.
2. Lokales Publish/Subscribe mit `mqttcli` testen.
3. `bridge-config.json` erstellen.
4. Sicherstellen, dass der TTN-Broker-Eintrag das TTN-Uplink-Topic abonniert.
5. Sicherstellen, dass der lokale Broker-Eintrag nicht `#` abonniert.
6. `mqttbridge` mit `bridge --definition` starten.
7. Lokale `ttn/#`-Topics abonnieren.
8. Auf ESP32-Uplink warten.

Deliverable:

```text
terminal output showing TTN uplink arriving on local MQTT and bridge-config.json without local '#' subscription
```

## Aufgabe 10 — Messwerte, GPS und Dashboard mit mqttcli bereitstellen

Owner: Student:in 4 startet den Prozess; Student:in 5 prüft die Browseransicht.

Ziel: das bereitgestellte `mqttcli` aus `mqttcli-mariadb` als einzigen Storage-/Dashboard-Prozess verwenden.

Schritte:

1. Bereitgestellten `mqttcli`-Storage-/Dashboard-Prozess starten.
2. Er abonniert lokale `ttn/#`-Nachrichten.
3. Er fügt skalare numerische Werte in `measurements` ein.
4. Er fügt GPS-Positionen in `gps_positions` ein.
5. Er stellt das Dashboard auf Port 8080 bereit.
6. Beide Datenbanktabellen abfragen.
7. Dashboard öffnen.

Erwartete Spalten der Tabelle `measurements`:

```text
id, received_at, application_id, device_id, f_port, value
```

Erwartete Spalten der Tabelle `gps_positions`:

```text
id, received_at, application_id, device_id, latitude, longitude, altitude, hdop
```

Dashboard-URL:

```text
http://groupN.local:8080/
```

Deliverable:

```text
SELECT output showing scalar sensor rows, SELECT output showing GPS rows, plus dashboard screenshot from port 8080
```

## Aufgabe 11 — Finaler Ende-zu-Ende-Test

Owner: alle Studierenden, koordiniert durch Student:in 5.

Verfolgen Sie einen skalaren Wert durch die vollständige Kette:

```text
sensor
  -> ESP32 serial output
  -> TTN live data
  -> local MQTT on Raspberry Pi
  -> MariaDB measurements row
  -> dashboard display
```

Verfolgen Sie außerdem eine GPS-Position:

```text
GPS
  -> ESP32 serial output
  -> TTN live data
  -> local MQTT on Raspberry Pi
  -> MariaDB gps_positions row
  -> dashboard display or GPS table
```

Deliverable:

```text
one documented scalar trace and one documented GPS trace through all layers
```

## Aufgabe 12 — Reale Wassermessung am See

Owner: alle Studierenden.

Ziel: eine reale Messung mit Kontext durchführen.

Schritte:

1. Batterie laden.
2. Wasserschutz und Verdrahtung prüfen.
3. Pi-Backend starten.
4. ESP32-Boje starten.
5. Auf vollständigen fPort-Zyklus warten.
6. Sensoren ins Seewasser bringen.
7. Umgebungskontext dokumentieren.
8. Dashboard-Aktualisierung für skalare Messwerte und GPS prüfen.

Deliverable:

```text
field context log, dashboard screenshot, database rows from measurements and gps_positions, and measurement discussion
```

## Aufgabe 13 — Abschlussbericht und Präsentation

Owner: Student:in 5 koordiniert, alle Studierenden tragen bei.

Der Abschlussbericht sollte enthalten:

```text
1. Introduction and project goal
2. Group organization and responsibilities
3. End-to-end architecture
4. Hardware and sensors
5. Firmware and generator configuration
6. TTN setup
7. Raspberry Pi backend
8. MQTT bridge and MariaDB storage
9. Dashboard
10. Calibration
11. Field measurement
12. Results
13. Limitations
14. Improvements
15. Conclusion
```

Jede:r Studierende trägt den Abschnitt bei, der zum eigenen Workstream gehört.

## Minimales Erfolgskriterium

Die Gruppe ist erfolgreich, wenn sie Folgendes demonstrieren und erklären kann:

```text
A real sensor value and a GPS position are measured by the ESP32 buoy, decoded in TTN, bridged to the Raspberry Pi, stored in MariaDB, and shown in the dashboard.
```

Die Gruppe sollte außerdem die Grenzen der Messung und die Zuverlässigkeitsrisiken des Systems erklären.
