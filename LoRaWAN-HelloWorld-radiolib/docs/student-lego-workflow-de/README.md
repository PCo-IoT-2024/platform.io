# Studierendenhandbuch — LoRaWAN-Wasserüberwachungsboje

Dieses Verzeichnis enthält die Teaching-Book-Version des Student-Lego-Workflows für die konfigurierbare LoRaWAN-Wasserüberwachungsbojen-Firmware und das lokale Raspberry-Pi-Backend.

Das Handbuch ist für Studierende in einem managementorientierten Studienprogramm geschrieben, die möglicherweise keine starke Vorbildung in Elektronik oder Embedded Systems haben. Es folgt dem Workshop-Ablauf: Orientierung, Gruppenorganisation, Installation, Geräte-Setup, TTN-Cloud-Setup, lokales Raspberry-Pi-Backend, Dashboard, reale Wassermessung und finale Reflexion.

Verwenden Sie dieses Handbuch zusammen mit dem aktuellen Branch:

```text
course/lego-configurable
```

Der praktische Firmware-Workflow ist generatorbasiert. Der Webgenerator erzeugt zwei Dateien, die zusammenpassen müssen:

```text
platformio.ini
payload-formatter.js
```

Der praktische Backend-Workflow verwendet einen Raspberry Pi pro Gruppe:

```text
user: water
hostnames: group1, group2, group3, group4
local MQTT port: 1883
dashboard port: 8080
```

Das MariaDB-Backend verwendet zwei Tabellen:

```text
measurements     -> skalare Sensorwerte, identifiziert über f_port
gps_positions   -> GPS-Datensätze mit latitude/longitude/altitude/HDOP
```

## Inhaltsverzeichnis

1. [Kurskontext und Lernziele](00-course-context.md)
2. [Technologie-Grundlagen für Manager:innen](01-technology-primer-for-managers.md)
3. [Ende-zu-Ende-Architektur der Boje](02-end-to-end-architecture.md)
4. [Organisation der Gruppenarbeit](03-group-work-organization.md)
5. [Installation von Qt Creator, PlatformIO und CP2102-Treibern unter Windows und macOS](04-installation-windows-macos.md)
6. [Systemüberblick](05-system-overview.md)
7. [LoRaWAN und TTN-Theorie](06-lorawan-and-ttn.md)
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

## Wie dieses Handbuch gelesen werden soll

Studierende mit wenig technischem Vorwissen sollten zuerst die Orientierungskapitel lesen, bevor sie Werkzeuge verwenden. Das Kapitel zur Gruppenarbeit sollte vor der ersten praktischen Workshop-Einheit gelesen werden, weil es die fünf individuellen Workstreams definiert.

Die praktische Build-Arbeit beginnt nach der Installation. Die Geräteseite-Kapitel führen zu einem funktionierenden ESP32-zu-TTN-Pfad. Die Backend-Kapitel erweitern das System dann von TTN in den Raspberry Pi, MariaDB und das mqttcli/SNode.C-Dashboard. Die finalen Kapitel bereiten die Feldmessung am See und den Abschluss vor.

## Wichtige externe Ressourcen

- [Git downloads](https://git-scm.com/downloads)
- [Python downloads](https://www.python.org/downloads/)
- [PlatformIO Core installation](https://docs.platformio.org/en/latest/core/installation/index.html)
- [Qt Creator installation documentation](https://doc.qt.io/qtcreator/creator-how-to-install.html)
- [Qt downloads](https://www.qt.io/download/)
- [Silicon Labs CP210x USB to UART Bridge VCP drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers)
- [Homebrew](https://brew.sh/)
- [The Things Network / The Things Stack documentation](https://www.thethingsindustries.com/docs/)
- [PlatformIO documentation](https://docs.platformio.org/)
- [RadioLib documentation](https://jgromes.github.io/RadioLib/)
- [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/)
- [MariaDB documentation](https://mariadb.com/kb/en/documentation/)
- [SNode.C repository](https://github.com/SNodeC/snode.c)
- [MQTTSuite repository](https://github.com/SNodeC/mqttsuite)

## Aktuelle fPort-Zuordnung

| fPort | Messung | Dekodiertes TTN-Feld | MariaDB-Tabelle |
|---:|---|---|---|
| 1 | GPS-Position | `latitude`, `longitude`, `altitude`, `hdop` | `gps_positions` |
| 2 | DS18B20-Wassertemperatur | `temperature_c` | `measurements` |
| 3 | PH4502C pH | `ph_level` | `measurements` |
| 4 | Gravity TDS | `tds_ppm` | `measurements` |
| 5 | Trübung | `turbidity_ntu` | `measurements` |
| 6 | PH4502C-Boardtemperatur | `ph_board_temperature_c` | `measurements` |

Diagnose-fPorts:

| fPort | Bedeutung |
|---:|---|
| 220 | weitere Downlinks anfordern |
| 221 | Info / Diagnose |
| 222 | Warnung |
| 223 | Fehler |

Wichtiges aktuelles Verhalten:

- Der PH4502C-Boardtemperaturkanal ist verpflichtend und verwendet fPort 6.
- Dekodierte Measurement-Payloads enthalten kein `payload_raw`.
- TDS sendet nur `tds_ppm`, nicht die Kompensationstemperatur.
- pH sendet nur `ph_level`, nicht den rohen ADC-Wert und nicht die Boardtemperatur.
- Alle analogen Kalibrierwerte sind rohe ESP32-ADC-Werte, keine Spannungen.
- MariaDB speichert skalare Werte in `measurements` mit `f_port` plus `value`.
- MariaDB speichert GPS-Werte in `gps_positions` mit Latitude-, Longitude-, Altitude- und HDOP-Spalten.

## Schneller Gesamtworkflow

```text
1. Ziel und Ende-zu-Ende-Architektur verstehen.
2. Gruppe in fünf Workstreams aufteilen.
3. Qt Creator, PlatformIO, Git/Python und USB-Bridge-Treiber installieren.
4. TTN-Anwendung, Gerät, Payload Formatter und MQTT-Zugang einrichten.
5. platformio.ini und payload-formatter.js erzeugen.
6. ESP32-Firmware bauen und flashen.
7. Sensoren kalibrieren und dekodierte TTN-Uplinks prüfen.
8. Raspberry-Pi-Backend als water@groupN.local vorbereiten.
9. SNode.C master und MQTTSuite mqttcli-mariadb bauen.
10. mqttbroker, mqttbridge, mqttcli Storage/Dashboard und MariaDB starten.
11. Skalare Messwerte in measurements und GPS-Positionen in gps_positions speichern.
12. Dashboard mit mqttcli/SNode.C auf Port 8080 bereitstellen.
13. Ende-zu-Ende-Test durchführen.
14. Reales Wasser am See messen.
15. Ergebnisse, Grenzen und Verbesserungen präsentieren.
```
