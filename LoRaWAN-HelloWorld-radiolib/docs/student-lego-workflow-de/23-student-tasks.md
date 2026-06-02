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
Screenshot oder Terminal-Log mit Repository, Branch und Arbeitsverzeichnis
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
Gruppentabelle mit Namen, Workstreams, Schnittstellen und Verantwortlichkeiten
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
TTN-Live-Data-Screenshot mit dekodierter Payload und getrennte, sichere Dokumentation der MQTT-Zugangsdaten
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
platformio.ini, Payload Formatter, erfolgreicher Build-/Upload-Log, Serial-Monitor-Ausgabe
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
Rohe ESP32-ADC-Werte für analoge Kalibrierung verwenden, keine Spannungen.
```

Deliverable:

```text
Verdrahtungstabelle, Kalibriertabelle, rohe ADC-Werte, Notizen zu Referenzen und Plausibilität
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
Tabelle mit Wake-Nummer, fPort, Sensor, dekodiertem TTN-Feld und Beispielwert
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
SSH-Nachweis, Paketinstallationsnachweis, MariaDB-DESCRIBE-Ausgabe für measurements und gps_positions
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
Build-Verzeichnis als Geschwisterverzeichnis neben jedem geklonten Source-Verzeichnis
```

Deliverable:

```text
erfolgreiche Build-Logs und funktionierende mqttbroker-, mqttbridge- und mqttcli-Binaries
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
Terminal-Ausgabe, die zeigt, dass ein TTN-Uplink in lokalem MQTT ankommt, und bridge-config.json ohne lokale '#'-Subscription
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
SELECT-Ausgabe mit skalaren Sensorzeilen, SELECT-Ausgabe mit GPS-Zeilen und Dashboard-Screenshot von Port 8080
```

## Aufgabe 11 — Finaler Ende-zu-Ende-Test

Owner: alle Studierenden, koordiniert durch Student:in 5.

Verfolgen Sie einen skalaren Wert durch die vollständige Kette:

```text
Sensor
  -> ESP32-Serial-Ausgabe
  -> TTN Live Data
  -> lokales MQTT am Raspberry Pi
  -> MariaDB-Zeile in measurements
  -> Dashboard-Anzeige
```

Verfolgen Sie außerdem eine GPS-Position:

```text
GPS
  -> ESP32-Serial-Ausgabe
  -> TTN Live Data
  -> lokales MQTT am Raspberry Pi
  -> MariaDB-Zeile in gps_positions
  -> Dashboard-Anzeige oder GPS-Tabelle
```

Deliverable:

```text
ein dokumentierter Skalarwert-Trace und ein dokumentierter GPS-Trace durch alle Schichten
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
Feldkontextprotokoll, Dashboard-Screenshot, Datenbankzeilen aus measurements und gps_positions sowie Messwertdiskussion
```

## Aufgabe 13 — Abschlussbericht und Präsentation

Owner: Student:in 5 koordiniert, alle Studierenden tragen bei.

Der Abschlussbericht sollte enthalten:

```text
1. Einleitung und Projektziel
2. Gruppenorganisation und Verantwortlichkeiten
3. Ende-zu-Ende-Architektur
4. Hardware und Sensoren
5. Firmware- und Generator-Konfiguration
6. TTN-Setup
7. Raspberry-Pi-Backend
8. MQTT-Bridge und MariaDB-Speicherung
9. Dashboard
10. Kalibrierung
11. Feldmessung
12. Ergebnisse
13. Grenzen
14. Verbesserungen
15. Fazit
```

Jede:r Studierende trägt den Abschnitt bei, der zum eigenen Workstream gehört.

## Minimales Erfolgskriterium

Die Gruppe ist erfolgreich, wenn sie Folgendes demonstrieren und erklären kann:

```text
Ein realer Sensorwert und eine GPS-Position werden von der ESP32-Boje gemessen, in TTN dekodiert, zum Raspberry Pi gebridged, in MariaDB gespeichert und im Dashboard angezeigt.
```

Die Gruppe sollte außerdem die Grenzen der Messung und die Zuverlässigkeitsrisiken des Systems erklären.
