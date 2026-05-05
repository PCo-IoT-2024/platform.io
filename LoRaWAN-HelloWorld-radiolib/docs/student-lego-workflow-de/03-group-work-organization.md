# Organisation der Gruppenarbeit

Das Bojenprojekt ist für Gruppen mit fünf Studierenden ausgelegt.

Jede Person übernimmt einen klar definierten Workstream. Die Workstreams sind so unabhängig, dass jede Person sichtbar beitragen kann. Gleichzeitig sind sie nicht isoliert: Das Endergebnis funktioniert nur, wenn alle fünf Teile integriert werden.

Das ist bewusst so gewählt. Ein reales IoT-System wird nicht von einer einzelnen isolierten Spezialistin oder einem einzelnen isolierten Spezialisten gebaut. Es ist ein Integrationsprojekt aus Hardware, Firmware, Cloud-Diensten, Backend-Infrastruktur, Datenbank, Dashboard, Test und Dokumentation.

## Empfohlene Gruppenstruktur

| Student:in | Workstream | Hauptergebnis |
|---:|---|---|
| 1 | ESP32-Firmware und LoRaWAN-Gerät | Firmware baut, flasht, joint TTN und sendet korrekte fPorts |
| 2 | Sensoren und Kalibrierung | Sensoren sind verdrahtet, kalibriert, dokumentiert und liefern plausible Werte |
| 3 | TTN-Cloud-Setup und Payload Formatter | TTN-Anwendung/Gerät funktioniert, Formatter dekodiert Uplinks, MQTT-Zugang ist vorbereitet |
| 4 | Raspberry-Pi-Backend, MQTT und MariaDB | Pi betreibt Broker, Bridge, Storage/Dashboard-Prozess und Datenbankzeilen |
| 5 | Dashboard, Dokumentation und Integrationstest | Dashboard zeigt Daten, finaler Testpfad ist dokumentiert, Bericht ist zusammengestellt |

Diese Tabelle darf nicht als fünf voneinander getrennte Mini-Projekte verstanden werden. Am wichtigsten sind die Schnittstellen zwischen den Workstreams.

## Workstream 1 — ESP32-Firmware und LoRaWAN-Gerät

Diese Person ist für den Embedded-Firmware-Workflow verantwortlich.

Hauptaufgaben:

- `platformio.ini` erzeugen
- Firmware mit PlatformIO bauen
- ESP32 flashen
- serielle Ausgabe prüfen
- LoRa-Radioinitialisierung prüfen
- OTAA Join oder Session Restore prüfen
- fPort-Rotation prüfen
- Firmware-Einstellungen mit TTN- und Sensor-Workstream abstimmen

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
[ ] erzeugte platformio.ini
[ ] erfolgreicher Build-Log
[ ] erfolgreicher Upload
[ ] Screenshot oder Log des seriellen Monitors
[ ] Dokumentation der fPort-Sequenz
```

Fertig, wenn:

```text
Der ESP32 Uplinks auf den erwarteten fPorts sendet und TTN sie empfängt.
```

Schnittstellen zu anderen Personen:

| Schnittstelle | Partner:in |
|---|---|
| Sensorpins und aktivierte Sensoren | Student:in 2 |
| DevEUI, AppKey, NwkKey, LoRaWAN-Version | Student:in 3 |
| fPort-Zuordnung und Payload-Felder | Studierende 3, 4, 5 |

## Workstream 2 — Sensoren und Kalibrierung

Diese Person ist für die Messseite verantwortlich.

Hauptaufgaben:

- Sensoren korrekt verdrahten
- Sensorversorgung prüfen
- ESP32-Pin-Mapping dokumentieren
- Kalibriermodus für analoge Sensoren verwenden
- rohe ADC-Kalibrierwerte erfassen
- Kalibrierwerte in den Generator eintragen
- Grenzen der Messung erklären

Sensoren:

| Sensor | Hauptwert |
|---|---|
| GPS | Position |
| DS18B20 | Wassertemperatur |
| PH4502C pH | pH-Wert |
| PH4502C-Boardtemperatur | Board-/Onboard-Temperatur |
| Gravity TDS | TDS ppm |
| Trübungssensor | turbidity NTU |

Wichtige Regel:

```text
Alle analogen Kalibrierwerte sind rohe ESP32-ADC-Werte, keine Spannungen.
```

Deliverables:

```text
[ ] Verdrahtungstabelle
[ ] Kalibriertabelle
[ ] rohe ADC-Werte
[ ] Notizen zu Referenzlösungen oder Referenzbedingungen
[ ] kurze Diskussion der Sensorzuverlässigkeit
```

Fertig, wenn:

```text
Die Firmware plausible kalibrierte Werte für alle angeschlossenen Sensoren erzeugt.
```

## Workstream 3 — TTN-Cloud-Setup und Payload Formatter

Diese Person ist für die LoRaWAN-Cloud-Seite verantwortlich.

Hauptaufgaben:

- TTN-Anwendung erstellen
- TTN-Endgerät erstellen
- LoRaWAN-Version und Region konfigurieren
- DevEUI, AppKey und gegebenenfalls NwkKey bereitstellen
- generierten Payload Formatter installieren
- TTN Live Data prüfen
- MQTT API Key für Backend-Zugriff erstellen
- TTN-MQTT-Verbindungsdaten dokumentieren

Wichtige TTN-Werte:

```text
application ID
device ID
DevEUI
JoinEUI
AppKey
NwkKey, falls LoRaWAN 1.1.0
region / frequency plan
MQTT server
MQTT username
MQTT API key
uplink topic pattern
```

Echte Schlüssel dürfen nicht öffentlich committed werden.

Fertig, wenn:

```text
TTN Live Data dekodierte Felder für Bojen-Uplinks zeigt und das Backend-Team die MQTT-Zugangsdaten hat.
```

## Workstream 4 — Raspberry-Pi-Backend, MQTT und MariaDB

Diese Person ist für die lokale Backend-Infrastruktur verantwortlich.

Hauptaufgaben:

- Raspberry Pi OS vorbereiten
- Build-Werkzeuge und MariaDB installieren
- SNode.C bauen und installieren
- MQTTSuite branch `mqttcli-mariadb` bauen und installieren
- lokalen `mqttbroker` betreiben
- `mqttbridge` konfigurieren und starten
- lokale MQTT-Nachrichten prüfen
- Messwerte mit `mqttcli` in MariaDB speichern
- Dashboardprozess betreiben

Zielpfad:

```text
TTN MQTT -> mqttbridge -> local mqttbroker -> mqttcli -> MariaDB + Dashboard
```

Fertig, wenn:

```text
Ein echter TTN-Uplink eine Zeile in MariaDB erzeugt und über das Dashboard sichtbar wird.
```

## Workstream 5 — Dashboard, Dokumentation und Integrationstest

Diese Person ist für das sichtbare Ergebnis und die Integrationsgeschichte verantwortlich.

Hauptaufgaben:

- definieren, was das Dashboard zeigen soll
- Dashboard unter `http://groupN.local:8080/` prüfen
- MariaDB-Ergebnisse mit Dashboard vergleichen
- Screenshots und Logs sammeln
- finalen Ende-zu-Ende-Test koordinieren
- Abschlussbericht zusammenstellen

Zielpfad:

```text
Browser -> mqttcli/SNode.C Dashboard -> MariaDB -> gespeicherte Messwerte
```

Fertig, wenn:

```text
Ein von der Boje gemessener Wert im Browser-Dashboard erscheint und über MariaDB, MQTT, TTN und ESP32-Serienausgabe zurückverfolgt werden kann.
```

## Erforderliche Schnittstellenverträge

Jede Gruppe muss diese Verträge früh festlegen.

### fPort-Vertrag

| fPort | Bedeutung | Feldname |
|---:|---|---|
| 1 | GPS | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | Wassertemperatur | `temperature_c` |
| 3 | pH | `ph_level` |
| 4 | TDS | `tds_ppm` |
| 5 | Trübung | `turbidity_ntu` |
| 6 | PH4502C-Boardtemperatur | `ph_board_temperature_c` |

### Pin-Mapping-Vertrag

Die Gruppe führt eine Tabelle mit:

```text
sensor
ESP32 pin
sensor-side label
notes
```

### MQTT-Topic-Vertrag

Die lokale Topic-Konvention muss dokumentiert werden, z. B.:

```text
ttn/#
```

### Datenbankvertrag

Die Gruppe muss wissen:

```text
measurements für skalare Werte
gps_positions für GPS
f_port als Bedeutungsschlüssel für measurements
```

## Empfohlener Gruppenablauf

### Phase 1 — Paralleles Setup

| Student:in | Aufgabe |
|---:|---|
| 1 | PlatformIO-Firmware-Build vorbereiten |
| 2 | Sensoren verdrahten und prüfen |
| 3 | TTN-Anwendung und Gerät erstellen |
| 4 | Raspberry Pi und MariaDB vorbereiten |
| 5 | Dashboard-/Berichtsstruktur vorbereiten |

### Phase 2 — Erste Integration

```text
Student:in 1 + 3: ESP32 joint TTN
Student:in 1 + 2: Sensorwerte sind plausibel
Student:in 3 + 4: TTN-MQTT-Nachrichten kommen am Pi an
Student:in 4 + 5: Datenbankfelder passen zum Dashboard
```

### Phase 3 — Ende-zu-Ende-Test

Alle arbeiten mit.

```text
sensor -> ESP32 serial output -> TTN live data -> local MQTT -> MariaDB -> dashboard
```

### Phase 4 — Abschlussbericht

Der Bericht wird nicht von einer Person allein geschrieben. Jede Person liefert ihren Workstream-Teil, Student:in 5 koordiniert das Gesamtdokument.

## Finales Erfolgskriterium

Die Gruppe ist erfolgreich, wenn alle Studierenden das Gesamtsystem auf hohem Niveau erklären können, auch wenn jede Person nur einen Workstream im Detail umgesetzt hat.

Mindestdemonstration:

```text
Ein realer Sensorwert und eine GPS-Position werden vom ESP32 gemessen, in TTN dekodiert, zum Raspberry Pi gebridged, in MariaDB gespeichert und im Dashboard angezeigt.
```
