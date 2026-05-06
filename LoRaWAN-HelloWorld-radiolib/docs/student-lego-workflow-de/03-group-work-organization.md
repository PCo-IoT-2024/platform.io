# Organisation der Gruppenarbeit

Das Bojenprojekt ist für Gruppen mit fünf Studierenden ausgelegt.

Jede Person soll einen klar definierten Workstream verantworten. Die Workstreams sind unabhängig genug, dass jede Person sichtbaren Fortschritt erzielen kann, aber sie sind nicht isoliert. Das Endergebnis funktioniert nur, wenn alle fünf Teile integriert werden.

Das ist bewusst so gewählt. Ein reales IoT-System wird nicht von einer einzelnen isolierten Spezialistin oder einem einzelnen isolierten Spezialisten gebaut. Es ist ein Systemintegrationsprojekt aus Hardware, Firmware, Cloud-Diensten, Backend-Infrastruktur, Datenbanken, Dashboards, Tests und Dokumentation.

---

## Empfohlene Gruppenstruktur

| Student:in | Workstream | Hauptergebnis |
|---:|---|---|
| 1 | ESP32-Firmware und LoRaWAN-Gerät | Firmware baut, wird geflasht, joint TTN und sendet korrekte fPorts |
| 2 | Sensoren und Kalibrierung | Sensoren verdrahtet, kalibriert, dokumentiert, plausible Werte gemessen |
| 3 | TTN-Cloud-Setup und Payload Formatter | TTN-Anwendung/Gerät funktioniert, Formatter dekodiert Uplinks, MQTT-Zugang vorbereitet |
| 4 | Raspberry-Pi-Backend, MQTT und MariaDB | Pi betreibt Broker, Bridge, Storage-Prozess, Datenbank erhält Zeilen |
| 5 | Dashboard, Dokumentation und Integrationstest | Dashboard zeigt Daten, finaler Testpfad ist dokumentiert, Bericht ist zusammengestellt |

Die Gruppe soll diese Tabelle nicht als fünf isolierte Mini-Projekte verstehen. Die Schnittstellen zwischen den Workstreams sind der wichtigste Teil.

---

## Workstream 1 — ESP32-Firmware und LoRaWAN-Gerät

Diese Person verantwortet den Embedded-Firmware-Workflow.

Hauptaufgaben:

- `platformio.ini` erzeugen
- Firmware mit PlatformIO bauen
- ESP32 flashen
- serielle Ausgabe prüfen
- LoRa-Radioinitialisierung prüfen
- OTAA Join oder Session Restore prüfen
- fPort-Rotation prüfen
- Firmware-Einstellungen mit den TTN- und Sensor-Studierenden abstimmen

Wichtige Dateien und Werkzeuge:

```text
tools/platformio-ini-generator/www/index.html
platformio.ini
pio run
pio run -t upload
pio device monitor
```

Deliverables:

```text
[ ] generated platformio.ini
[ ] successful build log
[ ] successful upload
[ ] serial monitor screenshot or log
[ ] fPort sequence documentation
```

Fertig, wenn:

```text
The ESP32 sends uplinks on the expected fPorts and TTN receives them.
```

Schnittstellen zu anderen Studierenden:

| Schnittstelle | Partner:in |
|---|---|
| Sensorpins und aktivierte Sensoren | Student:in 2 |
| DevEUI, AppKey, NwkKey, LoRaWAN-Version | Student:in 3 |
| fPort-Zuordnung und Payload-Felder | Studierende 3, 4, 5 |

---

## Workstream 2 — Sensoren und Kalibrierung

Diese Person verantwortet die Messseite.

Hauptaufgaben:

- Sensoren korrekt verdrahten
- Sensorversorgungsspannungen prüfen
- ESP32-Pin-Mapping dokumentieren
- Kalibriermodus für analoge Sensoren starten
- rohe ADC-Kalibrierwerte aufzeichnen
- Kalibrierwerte in den Generator eintragen
- Grenzen der Messung erklären

Sensoren:

| Sensor | Hauptwert |
|---|---|
| GPS | Position |
| DS18B20 | Wassertemperatur |
| PH4502C pH | pH-Wert |
| PH4502C board temperature | Board-/Onboard-Temperatur |
| Gravity TDS | TDS ppm |
| turbidity sensor | Trübung NTU |

Wichtige Regel:

```text
All analog calibration values are raw ESP32 ADC values, not voltages.
```

Deliverables:

```text
[ ] wiring table
[ ] calibration table
[ ] raw ADC values
[ ] notes about reference liquids or reference conditions
[ ] short discussion of sensor reliability
```

Fertig, wenn:

```text
The firmware produces plausible calibrated values for all connected sensors.
```

Schnittstellen zu anderen Studierenden:

| Schnittstelle | Partner:in |
|---|---|
| Pin-Mapping | Student:in 1 |
| Kalibrierwerte im Generator | Student:in 1 |
| Interpretation dekodierter Felder | Studierende 3 und 5 |
| Datenbank-Feldnamen | Student:in 4 |

---

## Workstream 3 — TTN-Cloud-Setup und Payload Formatter

Diese Person verantwortet die LoRaWAN-Cloud-Seite.

Hauptaufgaben:

- TTN-Anwendung erstellen
- TTN-Endgerät erstellen
- LoRaWAN-Version und Region konfigurieren
- DevEUI, AppKey und bei Bedarf NwkKey bereitstellen
- generierten Payload Formatter installieren
- TTN Live Data prüfen
- MQTT API Key für Backend-Zugriff erstellen
- TTN-MQTT-Verbindungsinformationen dokumentieren

Wichtige TTN-Werte:

```text
application ID
device ID
DevEUI
JoinEUI
AppKey
NwkKey, if LoRaWAN 1.1.0
region / frequency plan
MQTT server
MQTT username
MQTT API key
uplink topic pattern
```

Veröffentlichen Sie keine echten Schlüssel.

Deliverables:

```text
[ ] TTN application exists
[ ] TTN end device exists
[ ] payload formatter installed
[ ] live data screenshot with decoded payload
[ ] MQTT API key created and stored securely
[ ] TTN MQTT topic information documented
```

Fertig, wenn:

```text
TTN live data shows decoded fields for the buoy uplinks and the backend team has the MQTT access information.
```

Schnittstellen zu anderen Studierenden:

| Schnittstelle | Partner:in |
|---|---|
| LoRaWAN-Zugangsdaten | Student:in 1 |
| Payload-Felder | Studierende 2, 4, 5 |
| MQTT API Key und Topic | Student:in 4 |
| Screenshots/Ergebnisse | Student:in 5 |

---

## Workstream 4 — Raspberry-Pi-Backend, MQTT und MariaDB

Diese Person verantwortet die lokale Backend-Infrastruktur.

Hauptaufgaben:

- Raspberry Pi OS installieren und vorbereiten
- Build-Werkzeuge und MariaDB installieren
- SNode.C bauen
- MQTTSuite-Branch `mqttcli-mariadb` bauen
- lokalen `mqttbroker` starten
- `mqttbridge` konfigurieren und starten
- lokale MQTT-Nachrichten abonnieren
- Messwerte mit `mqttcli` in MariaDB speichern
- Kommandos und Konfiguration dokumentieren

Zieldatenpfad:

```text
TTN MQTT -> mqttbridge -> local mqttbroker -> mqttcli -> MariaDB
```

Deliverables:

```text
[ ] Raspberry Pi reachable by SSH
[ ] MariaDB running
[ ] SNode.C builds
[ ] MQTTSuite builds on branch mqttcli-mariadb
[ ] mqttbroker runs locally
[ ] mqttbridge forwards TTN messages
[ ] mqttcli stores measurements in MariaDB
[ ] SELECT query shows recent rows
```

Fertig, wenn:

```text
A real TTN uplink creates a measurement row in MariaDB on the Raspberry Pi.
```

Schnittstellen zu anderen Studierenden:

| Schnittstelle | Partner:in |
|---|---|
| TTN-MQTT-Zugangsdaten | Student:in 3 |
| Feldnamen und fPorts | Studierende 1, 2, 3 |
| Datenbankschema/API-Bedarf | Student:in 5 |
| finaler Integrationstest | alle Studierenden |

---

## Workstream 5 — Dashboard, Dokumentation und Integrationstest

Diese Person verantwortet das finale sichtbare Ergebnis und die Integrationsgeschichte.

Hauptaufgaben:

- definieren, was das Dashboard zeigen soll
- SNode.C-Dashboard-Backend starten oder anpassen
- Dashboard/Backend mit MariaDB verbinden
- neueste Messwerte anzeigen
- mindestens eine Verlaufsansicht anzeigen
- Screenshots und Logs sammeln
- finalen Ende-zu-Ende-Test koordinieren
- Abschlussbericht zusammenstellen

Zielpfad für Benutzer:innen:

```text
browser -> SNode.C dashboard -> MariaDB -> stored measurements
```

Deliverables:

```text
[ ] dashboard opens in browser
[ ] dashboard shows latest values
[ ] dashboard shows historical values or table
[ ] dashboard can be traced back to database rows
[ ] final integration checklist completed
[ ] final report assembled
```

Fertig, wenn:

```text
A value measured by the buoy appears in the browser dashboard and can be traced back through MariaDB, MQTT, TTN, and the ESP32 serial output.
```

Schnittstellen zu anderen Studierenden:

| Schnittstelle | Partner:in |
|---|---|
| Datenbankschema und Abfragen | Student:in 4 |
| Bedeutung dekodierter Felder | Studierende 2 und 3 |
| fPort-Zuordnung | Student:in 1 |
| Screenshots und Testnachweise | alle Studierenden |

---

## Erforderliche Schnittstellenverträge

Jede Gruppe muss diese Verträge früh vereinbaren.

### fPort-Vertrag

| fPort | Bedeutung | Feldname |
|---:|---|---|
| 1 | GPS | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | Wassertemperatur | `temperature_c` |
| 3 | pH | `ph_level` |
| 4 | TDS | `tds_ppm` |
| 5 | Trübung | `turbidity_ntu` |
| 6 | PH4502C board temperature | `ph_board_temperature_c` |

### Pin-Mapping-Vertrag

Die Gruppe muss eine Tabelle führen, die Folgendes enthält:

```text
sensor
ESP32 pin
sensor-side label
notes
```

### MQTT-Topic-Vertrag

Die Gruppe muss die lokale Topic-Konvention definieren, zum Beispiel:

```text
ttn/<application-id>/<device-id>/up
```

### Datenbankvertrag

Die Gruppe muss vereinbaren:

```text
database name
table names
field names
timestamp format
device identifier
which decoded fields are stored
```

### Dashboard-Vertrag

Die Gruppe muss vereinbaren:

```text
which values are shown
which time range is shown
which device is selected
which API endpoints are needed
```

---

## Empfohlener Gruppenablauf

### Phase 1 — Paralleles Setup

| Student:in | Aufgabe |
|---:|---|
| 1 | PlatformIO-Firmware-Build vorbereiten |
| 2 | Sensoren verdrahten und prüfen |
| 3 | TTN-Anwendung/Gerät erstellen |
| 4 | Raspberry Pi und MariaDB vorbereiten |
| 5 | Dashboard und Berichtsstruktur skizzieren |

### Phase 2 — Erste Integration

```text
Student 1 + Student 3: ESP32 joins TTN
Student 1 + Student 2: sensor readings are plausible
Student 3 + Student 4: TTN MQTT messages arrive on Pi
Student 4 + Student 5: database fields match dashboard needs
```

### Phase 3 — Ende-zu-Ende-Test

Alle Studierenden nehmen teil.

Verfolgen Sie einen Wert:

```text
sensor -> ESP32 serial output -> TTN live data -> local MQTT -> MariaDB -> dashboard
```

### Phase 4 — Abschlussbericht

Der Abschlussbericht soll nicht von einer Person allein geschrieben werden. Jede:r Studierende trägt den eigenen Abschnitt bei, und Student:in 5 koordiniert das finale Dokument.

---

## Koordinationsmeetings

Ein guter leichter Rhythmus ist:

| Zeitpunkt | Zweck |
|---|---|
| Beginn der Einheit | Ziele und Schnittstellen vereinbaren |
| Mitte der Einheit | Blocker prüfen |
| Ende der Einheit | dokumentieren, was funktioniert und was fehlt |

Verwenden Sie eine gemeinsame Checkliste. Das finale System hat viele kleine Teile, und man verliert leicht den Überblick.

---

## Finales Gruppenerfolgskriterium

Die Gruppe ist erfolgreich, wenn alle Studierenden das vollständige System auf hoher Ebene erklären können, auch wenn jede Person nur einen Workstream vertieft umgesetzt hat.

Minimale Abschlussdemonstration:

```text
A real sensor value is measured by the ESP32 buoy, decoded in TTN, bridged to the Raspberry Pi, stored in MariaDB, and shown in the dashboard.
```
