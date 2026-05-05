# Studierendenhandbuch — LoRaWAN-Wasserqualitätsboje

Dieses Verzeichnis enthält die deutsche Parallelfassung des englischen Studierendenhandbuchs für den Workshop zur konfigurierbaren LoRaWAN-Wasserqualitätsboje.

Die englische Version bleibt unverändert unter:

```text
LoRaWAN-HelloWorld-radiolib/docs/student-lego-workflow/
```

Diese deutsche Version liegt parallel unter:

```text
LoRaWAN-HelloWorld-radiolib/docs/student-lego-workflow-de/
```

Die Übersetzung ist für Studierende in einem managementorientierten Studienprogramm gedacht, die nicht notwendigerweise viel Vorwissen in Elektronik, Embedded Systems oder Netzwerktechnik mitbringen. Kommandos, Dateinamen, JSON-Schlüssel, SQL-Spaltennamen, fPorts, TTN-Feldnamen und Codeblöcke bleiben bewusst auf Englisch bzw. technisch unverändert.

## Inhaltsverzeichnis

1. [Kurskontext und Lernziele](00-course-context.md)
2. [Technologie-Grundlagen für Manager:innen](01-technology-primer-for-managers.md)
3. [Ende-zu-Ende-Architektur der Boje](02-end-to-end-architecture.md)
4. [Organisation der Gruppenarbeit](03-group-work-organization.md)
5. [Installation von Qt Creator, PlatformIO und CP2102-Treibern](04-installation-windows-macos.md)
6. [Systemüberblick](05-system-overview.md)
7. [LoRaWAN und TTN](06-lorawan-and-ttn.md)
8. [TTN-Cloud-Setup](07-ttn-cloud-setup.md)
9. [Generator-Workflow](08-generator-workflow.md)
10. [Firmware-Architektur](09-firmware-architecture.md)
11. [Sensoren und Messtheorie](10-sensors-and-measurements.md)
12. [Analoge Kalibrierung](11-analog-calibration.md)
13. [Bauen, Flashen, Testen und Fehlersuche](12-build-test-troubleshoot.md)
14. [Raspberry-Pi-Setup](13-raspberry-pi-setup.md)
15. [MQTT-Grundlagen für das Bojen-Backend](14-mqtt-theory.md)
16. [SNode.C und MQTTSuite am Raspberry Pi bauen](15-building-snodec-and-mqttsuite.md)
17. [Lokalen MQTT-Broker und TTN-Bridge betreiben](16-local-mqttbroker-and-bridge.md)
18. [MQTT-Daten mit mqttcli in MariaDB speichern](17-mariadb-storage-with-mqttcli.md)
19. [Dashboard mit mqttcli und SNode.C](18-dashboard-with-snodec.md)
20. [Stromversorgung, Deep Sleep und Solarbetrieb](19-power-and-deep-sleep.md)
21. [Prozesse starten und finaler Ende-zu-Ende-Test](20-running-services-and-final-test.md)
22. [Reale Wassermessung und Feldtest](21-real-water-measurement-and-field-test.md)
23. [Abschlusspräsentation und Fazit](22-final-presentation-and-conclusion.md)
24. [Aufgaben und Deliverables](23-student-tasks.md)

## Technische Konstanten

Die praktische Firmware-Arbeit basiert auf zwei zusammengehörigen Dateien:

```text
platformio.ini
payload-formatter.js
```

Das Raspberry-Pi-Backend verwendet pro Gruppe einen Pi:

```text
user: water
hostnames: group1, group2, group3, group4
local MQTT port: 1883
dashboard port: 8080
```

Die MariaDB-Datenbank verwendet zwei Tabellen:

```text
measurements     -> skalare Sensorwerte, über f_port identifiziert
gps_positions   -> GPS-Datensätze mit latitude, longitude, altitude, hdop
```

## Aktuelle fPort-Zuordnung

| fPort | Messung | Dekodiertes TTN-Feld | MariaDB-Tabelle |
|---:|---|---|---|
| 1 | GPS-Position | `latitude`, `longitude`, `altitude`, `hdop` | `gps_positions` |
| 2 | DS18B20-Wassertemperatur | `temperature_c` | `measurements` |
| 3 | PH4502C pH | `ph_level` | `measurements` |
| 4 | Gravity TDS | `tds_ppm` | `measurements` |
| 5 | Trübung | `turbidity_ntu` | `measurements` |
| 6 | PH4502C-Boardtemperatur | `ph_board_temperature_c` | `measurements` |

## Schneller Gesamtworkflow

```text
1. Ziel und Ende-zu-Ende-Architektur verstehen.
2. Gruppe in fünf Workstreams aufteilen.
3. Qt Creator, PlatformIO, Git/Python und USB-Treiber installieren.
4. TTN-Anwendung, Endgerät, Payload Formatter und MQTT-Zugang einrichten.
5. platformio.ini und payload-formatter.js erzeugen.
6. ESP32-Firmware bauen und flashen.
7. Sensoren kalibrieren und dekodierte TTN-Uplinks prüfen.
8. Raspberry-Pi-Backend als water@groupN.local vorbereiten.
9. SNode.C master und MQTTSuite mqttcli-mariadb bauen und installieren.
10. mqttbroker, mqttbridge, mqttcli Storage/Dashboard und MariaDB starten.
11. Messwerte in measurements und GPS-Positionen in gps_positions speichern.
12. Dashboard mit mqttcli/SNode.C auf Port 8080 bereitstellen.
13. Ende-zu-Ende-Test durchführen.
14. Reales Wasser am See messen.
15. Ergebnisse, Grenzen und Verbesserungen präsentieren.
```
