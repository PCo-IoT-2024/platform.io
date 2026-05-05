# TTN-Cloud-Setup

**Primärer Workstream:** Student:in 3 — TTN-Cloud-Setup und Payload Formatter

**Schnittstellen:** Student:in 1 benötigt die LoRaWAN-Zugangsdaten; Student:in 4 benötigt MQTT-Zugangsdaten; Student:in 5 benötigt Screenshots und Nachweise.

Dieses Kapitel beschreibt, wie die TTN-Anwendung, das Endgerät, der Payload Formatter und der MQTT-Zugang vorbereitet werden.

## Ziel

Am Ende dieses Kapitels soll gelten:

```text
[ ] TTN Application existiert.
[ ] TTN End Device existiert.
[ ] ESP32 kann joinen.
[ ] TTN Live Data zeigt Uplinks.
[ ] Payload Formatter dekodiert fPorts 1..6.
[ ] MQTT API Key ist vorhanden.
```

## TTN-Anwendung erstellen

In TTN eine neue Application anlegen. Der Name kann von der Gruppe gewählt werden, sollte aber eindeutig sein.

Beispiel:

```text
water-buoy-group1
water-buoy-group2
```

Die Application ID ist später Teil des MQTT-Benutzernamens und der Topic-Namen.

## Endgerät erstellen

Für jede Gruppe wird mindestens ein End Device benötigt.

Wichtige Felder:

```text
DevEUI
JoinEUI
AppKey
NwkKey, falls LoRaWAN 1.1.0 verwendet wird
LoRaWAN-Version
Frequency Plan
```

Die Werte müssen exakt mit der Firmware-Konfiguration im Generator übereinstimmen.

## Region und Frequency Plan

Für Europa wird im Kurs EU868 verwendet.

Der Frequency Plan in TTN muss zur Firmware passen. Eine falsche Region kann dazu führen, dass das Gerät nicht joined oder keine Uplinks sichtbar sind.

## Payload Formatter installieren

Der Generator erzeugt:

```text
payload-formatter.js
```

Dieser Code wird in TTN als Uplink Payload Formatter eingetragen.

Nach dem Einfügen prüfen:

```text
[ ] JavaScript ohne Syntaxfehler
[ ] fPort 1 dekodiert GPS
[ ] fPort 2 dekodiert Wassertemperatur
[ ] fPort 3 dekodiert pH
[ ] fPort 4 dekodiert TDS
[ ] fPort 5 dekodiert Trübung
[ ] fPort 6 dekodiert PH4502C-Boardtemperatur
```

## TTN Live Data prüfen

Nach dem Flashen des ESP32 sollten in TTN Live Data Uplinks sichtbar sein.

Wichtig ist nicht nur, dass irgendein Uplink kommt. Wichtig ist, dass der Payload korrekt dekodiert wird.

Beispiele erwarteter Felder:

```text
latitude
longitude
altitude
hdop
temperature_c
ph_level
tds_ppm
turbidity_ntu
ph_board_temperature_c
```

## MQTT API Key erstellen

Für die Verbindung vom Raspberry Pi zu TTN MQTT wird ein API Key benötigt.

Der API Key muss mindestens das Lesen von Application Traffic erlauben.

Der Key wird später im `bridge-config.json` verwendet.

Wichtig:

```text
API Keys nicht in öffentliche Repositories committen.
```

## TTN MQTT-Werte dokumentieren

Für die Gruppe dokumentieren:

```text
TTN_MQTT_HOST=eu1.cloud.thethings.network
TTN_MQTT_PORT=8883
TTN_MQTT_USERNAME=<application-id>@ttn
TTN_UPLINK_TOPIC=v3/<application-id>@ttn/devices/+/up
```

Das Passwort ist der API Key.

## Übergabe an Student:in 1

Student:in 1 benötigt für die Firmware:

```text
DevEUI
JoinEUI
AppKey
NwkKey, falls nötig
LoRaWAN-Version
Region/Frequency Plan
```

## Übergabe an Student:in 4

Student:in 4 benötigt für die Bridge:

```text
TTN MQTT host
TTN MQTT port
TTN MQTT username
TTN MQTT API key
TTN uplink topic
```

## Fertig, wenn

```text
[ ] TTN Application und End Device existieren.
[ ] Firmware-Schlüssel wurden korrekt an Student:in 1 übergeben.
[ ] Payload Formatter ist installiert.
[ ] TTN Live Data zeigt dekodierte Uplinks.
[ ] MQTT API Key wurde erzeugt.
[ ] MQTT-Zugangsdaten wurden sicher an Student:in 4 übergeben.
```
