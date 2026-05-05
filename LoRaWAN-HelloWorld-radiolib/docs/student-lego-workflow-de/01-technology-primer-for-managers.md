# Technologie-Grundlagen für Manager:innen

Dieses Kapitel erklärt die wichtigsten technischen Begriffe des Projekts ohne vorauszusetzen, dass Studierende bereits Erfahrung mit Embedded Systems, Funknetzen, Datenbanken oder Web-Dashboards haben.

Das Ziel ist nicht, jedes Detail vollständig zu beherrschen. Das Ziel ist, die Rolle jeder Komponente im Gesamtsystem zu verstehen.

## Ein IoT-System ist eine Kette

Ein IoT-System besteht selten aus einem einzigen Gerät. Es ist meistens eine Kette aus vielen Komponenten.

In diesem Kurs sieht die Kette so aus:

```text
Sensor
  -> Mikrocontroller
  -> Funknetz
  -> Cloud-Dienst
  -> MQTT
  -> lokaler Server
  -> Datenbank
  -> Dashboard
```

Wenn ein Wert im Dashboard falsch ist, kann der Fehler an fast jeder Stelle dieser Kette entstanden sein.

Beispiele:

- Sensor falsch verkabelt
- Sensor nicht kalibriert
- Firmware liest den falschen Pin
- LoRaWAN-Schlüssel sind falsch
- Payload Formatter passt nicht zum Firmware-Payload
- MQTT-Bridge ist falsch konfiguriert
- Datenbank speichert den Wert falsch
- Dashboard interpretiert den fPort falsch

Deshalb ist schrittweises Testen wichtig.

## Sensor

Ein Sensor wandelt eine physikalische oder chemische Größe in ein elektrisches Signal oder digitale Daten um.

Beispiele in diesem Projekt:

| Sensor | Messgröße |
|---|---|
| DS18B20 | Temperatur |
| pH-Sonde mit PH4502C | pH-Wert |
| TDS-Sensor | gelöste Stoffe im Wasser |
| Trübungssensor | Trübung des Wassers |
| GPS-Modul | Position |

Ein Sensor liefert nicht automatisch Wahrheit. Ein Sensor liefert ein Signal. Dieses Signal muss richtig gelesen, kalibriert und interpretiert werden.

## Mikrocontroller

Ein Mikrocontroller ist ein kleiner Computer auf einem Chip. In diesem Projekt verwenden wir einen ESP32.

Der ESP32:

- liest Sensoren
- verarbeitet Messwerte
- steuert das LoRa-Funkmodul
- sendet Daten
- geht in Deep Sleep, um Energie zu sparen

Ein Mikrocontroller ist kein normaler Laptop. Er hat begrenzten Speicher, begrenzte Rechenleistung und wird oft mit Batterie betrieben.

## Firmware

Firmware ist Software, die direkt auf dem Mikrocontroller läuft.

In diesem Projekt macht die Firmware unter anderem:

```text
aufwachen
Sensor auswählen
Sensorwert lesen
Kalibrierung anwenden
LoRaWAN-Uplink senden
wieder schlafen gehen
```

Die Firmware wird mit PlatformIO gebaut und auf den ESP32 geflasht.

## LoRaWAN

LoRaWAN ist ein Funknetz für kleine Datenmengen über größere Distanzen.

Es ist nicht gedacht für:

- Bilder
- Audio
- Video
- große Datenpakete
- sehr häufige Nachrichten

Es ist gut geeignet für:

- Sensordaten
- Batteriebetrieb
- kurze Nachrichten
- große Reichweite

In diesem Projekt sendet die Boje kleine Messwerte über LoRaWAN.

## TTN / The Things Network

The Things Network, kurz TTN, ist ein LoRaWAN-Netzwerkserver.

TTN empfängt LoRaWAN-Nachrichten von Gateways und stellt sie der Anwendung zur Verfügung.

TTN übernimmt unter anderem:

- Geräteverwaltung
- Join-Prozess
- Sicherheitsprüfung
- Frame Counter
- Weiterleitung von Uplinks
- Payload Formatter
- MQTT-Integration

Die Studierenden sehen in TTN, ob ein Gerät erfolgreich sendet und ob der Payload korrekt dekodiert wird.

## Payload

Der Payload ist der eigentliche Inhalt einer Nachricht.

Ein LoRaWAN-Uplink besteht nicht nur aus dem Messwert. Er enthält technische Informationen und einen binären Payload. TTN kann diesen Payload mit einem Payload Formatter in lesbare Felder umwandeln.

Beispiel:

```text
binärer Payload -> Payload Formatter -> { "tds_ppm": 350 }
```

Firmware und Payload Formatter müssen exakt zusammenpassen.

## fPort

Der fPort ist eine kleine Zahl in einer LoRaWAN-Nachricht. In diesem Projekt verwenden wir ihn, um die Art der Messung zu unterscheiden.

Beispiel:

| fPort | Bedeutung |
|---:|---|
| 1 | GPS |
| 2 | Wassertemperatur |
| 3 | pH |
| 4 | TDS |
| 5 | Trübung |
| 6 | PH4502C-Boardtemperatur |

Der fPort ist damit ein wichtiger Vertrag zwischen Firmware, TTN, Datenbank und Dashboard.

## MQTT

MQTT ist ein leichtgewichtiges Nachrichtensystem.

Es arbeitet mit drei Rollen:

| Rolle | Bedeutung |
|---|---|
| Broker | zentrale Nachrichtenstelle |
| Publisher | sendet Nachrichten |
| Subscriber | empfängt Nachrichten |

In diesem Projekt gibt es zwei MQTT-Welten:

```text
TTN MQTT in der Cloud
lokaler MQTT-Broker am Raspberry Pi
```

Die MQTT-Bridge verbindet diese beiden Welten.

## Raspberry Pi

Der Raspberry Pi ist der lokale Server des Projekts.

Er ist zuständig für:

- lokalen MQTT-Broker
- MQTT-Bridge von TTN zum lokalen Broker
- Speicherung in MariaDB
- Dashboard auf Port 8080

Der Pi ist nicht die Boje. Die Boje ist der ESP32 mit Sensoren. Der Pi ist das lokale Backend.

## Datenbank

MQTT zeigt aktuelle Nachrichten. Eine Datenbank speichert Geschichte.

Ohne Datenbank kann man schwer beantworten:

- Wie hat sich der pH-Wert über die Zeit verändert?
- Wann wurde ein bestimmter Wert gemessen?
- Welche Werte kamen von welchem Gerät?
- Welche Messung gehört zu welchem fPort?

In diesem Projekt verwenden wir MariaDB mit zwei Tabellen:

```text
measurements
  -> skalare Sensorwerte

gps_positions
  -> GPS-Positionen
```

## Dashboard

Ein Dashboard ist eine Benutzeroberfläche im Browser.

Es soll Studierenden und Betreuer:innen helfen, die Messdaten zu sehen, ohne SQL-Befehle eintippen zu müssen.

Ein einfaches Dashboard zeigt:

- letzte Messwerte
- Zeitstempel
- Gerät
- GPS-Position
- Tabellen oder einfache Verlaufsgrafiken

## Kalibrierung

Kalibrierung bedeutet, einen Sensor mit bekannten Referenzwerten abzugleichen.

Beispiel pH:

```text
pH-Referenzlösung 4
pH-Referenzlösung 7
pH-Referenzlösung 10
```

Für analoge Sensoren verwendet das Projekt rohe ADC-Werte des ESP32. Diese Werte werden in den Generator eingetragen und daraus wird die Kalibrierung der Firmware erzeugt.

## Wichtigste Management-Erkenntnis

Ein IoT-Prototyp ist nicht nur Hardware.

Er besteht aus:

```text
Geräten
Software
Cloud-Konfiguration
Schnittstellen
Datenmodell
Betrieb
Dokumentation
Teamarbeit
```

Ein technisches Projekt scheitert oft nicht an einer einzelnen Komponente, sondern an schlecht definierten Schnittstellen zwischen Komponenten.

Deshalb sind die Verträge in diesem Kurs so wichtig:

- fPort-Vertrag
- Pin-Mapping
- TTN-Feldnamen
- MQTT-Themen
- Datenbankschema
- Dashboard-Interpretation
