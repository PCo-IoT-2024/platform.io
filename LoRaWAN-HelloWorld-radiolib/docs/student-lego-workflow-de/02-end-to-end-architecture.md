# Ende-zu-Ende-Architektur der Boje

Dieses Kapitel zeigt die Gesamtarchitektur des Systems. Es erklärt, welche Komponenten beteiligt sind und wie ein Messwert von der realen Welt bis zum Dashboard gelangt.

Die Details einzelner Arbeitsschritte kommen später. Hier geht es zuerst um das große Bild.

## Die komplette Datenkette

Die wichtigste Idee ist:

```text
physikalische Messung
  -> digitale Messung
  -> Funknachricht
  -> Cloud-Dekodierung
  -> lokale MQTT-Nachricht
  -> Datenbankeintrag
  -> Dashboard-Anzeige
```

Als konkrete Architektur:

```text
Sensoren an der Boje
  -> ESP32-Firmware
  -> LoRaWAN-Uplink
  -> TTN / The Things Network
  -> TTN Payload Formatter
  -> TTN MQTT
  -> mqttbridge am Raspberry Pi
  -> lokaler mqttbroker
  -> mqttcli Storage/Dashboard
  -> MariaDB
  -> Browser-Dashboard
```

Wenn ein Wert im Dashboard erscheint, ist er bereits durch viele Systemschichten gegangen.

## Komponenten und Verantwortlichkeiten

| Komponente | Aufgabe |
|---|---|
| Sensoren | messen physikalische oder chemische Größen |
| ESP32 | liest Sensoren und sendet Uplinks |
| LoRa-Funkmodul | überträgt Daten per LoRaWAN |
| TTN | empfängt und verwaltet LoRaWAN-Daten |
| Payload Formatter | dekodiert binäre Payloads in lesbare Felder |
| TTN MQTT | stellt Uplinks als MQTT-Nachrichten bereit |
| mqttbridge | leitet TTN-MQTT lokal weiter |
| mqttbroker | lokaler MQTT-Broker am Raspberry Pi |
| mqttcli | speichert Daten und stellt Dashboard bereit |
| MariaDB | speichert Messwerte dauerhaft |
| Browser | zeigt Dashboard an |

## Wo laufen die Komponenten?

| Ort | Komponenten |
|---|---|
| Boje | ESP32, Sensoren, LoRa-Modul, Akku/Solar |
| LoRaWAN-Infrastruktur | Gateway und Netzwerkserver |
| TTN Cloud | Anwendung, Gerät, Formatter, MQTT-Zugang |
| Raspberry Pi | mqttbroker, mqttbridge, mqttcli, MariaDB, Dashboard |
| Laptop | Entwicklung, Browser, SSH, Dokumentation |

Das hilft beim Debugging. Wenn etwas nicht funktioniert, muss man wissen, wo der betreffende Teil läuft.

## Beispiel: Ein TDS-Wert

Ein TDS-Wert durchläuft diese Schritte:

```text
TDS-Sensor im Wasser
  -> analoges Signal am ESP32
  -> ADC-Rohwert
  -> Kalibrierung in der Firmware
  -> tds_ppm
  -> LoRaWAN-Uplink auf fPort 4
  -> TTN Payload Formatter
  -> decoded_payload.tds_ppm
  -> TTN MQTT
  -> mqttbridge
  -> lokaler mqttbroker
  -> mqttcli
  -> MariaDB measurements, f_port = 4, value = tds_ppm
  -> Dashboard
```

Wichtig ist: Der gleiche Wert wird in jeder Schicht etwas anders dargestellt.

## Beispiel: Eine GPS-Position

GPS ist anders als die meisten Sensorwerte, weil es mehrere Werte gleichzeitig enthält:

```text
latitude
longitude
altitude
hdop
```

Deshalb wird GPS in einer eigenen Tabelle gespeichert:

```text
LoRaWAN fPort 1
  -> decoded_payload.latitude
  -> decoded_payload.longitude
  -> decoded_payload.altitude
  -> decoded_payload.hdop
  -> MariaDB gps_positions
  -> Dashboard GPS-Anzeige
```

## Zentrale Schnittstellen

Ein integriertes System funktioniert nur, wenn die Schnittstellen stimmen.

### fPort-Schnittstelle

| fPort | Bedeutung |
|---:|---|
| 1 | GPS |
| 2 | Wassertemperatur |
| 3 | pH |
| 4 | TDS |
| 5 | Trübung |
| 6 | PH4502C-Boardtemperatur |

### Datenbank-Schnittstelle

```text
measurements
  -> f_port + value für skalare Sensoren

gps_positions
  -> latitude, longitude, altitude, hdop für GPS
```

### MQTT-Schnittstelle

TTN-Uplinks werden vom TTN-MQTT-Broker zum lokalen MQTT-Broker am Raspberry Pi weitergeleitet.

Die Bridge ist im Kurs bewusst einseitig gedacht:

```text
TTN -> lokaler MQTT-Broker
```

Nicht:

```text
lokaler MQTT-Broker -> TTN
```

## Warum diese Architektur sinnvoll ist

Die Architektur trennt Aufgaben klar:

- Die Boje misst und sendet.
- TTN übernimmt LoRaWAN und Dekodierung.
- Der Raspberry Pi übernimmt lokale Verarbeitung, Speicherung und Darstellung.
- Die Datenbank speichert Geschichte.
- Das Dashboard macht Werte sichtbar.

Diese Trennung macht das System verständlicher und testbarer.

## Was Studierende erklären können sollen

Nach diesem Kapitel sollten Studierende erklären können:

```text
[ ] wo ein Messwert entsteht
[ ] wo er dekodiert wird
[ ] warum TTN und lokaler MQTT-Broker verschiedene Rollen haben
[ ] warum MariaDB benötigt wird
[ ] warum GPS in einer eigenen Tabelle gespeichert wird
[ ] warum ein Dashboard nicht direkt mit dem ESP32 spricht
```

## Debugging-Denkweise

Wenn der Wert im Dashboard fehlt, geht man rückwärts:

```text
Dashboard
  -> MariaDB
  -> mqttcli
  -> lokaler MQTT-Broker
  -> mqttbridge
  -> TTN
  -> ESP32
  -> Sensor
```

Wenn schon in TTN nichts ankommt, geht man vorwärts:

```text
Sensor
  -> ESP32
  -> LoRaWAN Join
  -> Uplink
  -> TTN Live Data
```

Diese Denkweise spart Zeit, weil sie das Problem auf eine Schicht eingrenzt.
