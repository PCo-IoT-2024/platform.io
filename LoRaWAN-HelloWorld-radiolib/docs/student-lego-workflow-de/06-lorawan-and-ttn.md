# LoRaWAN und TTN

Dieses Kapitel erklärt die Rolle von LoRaWAN und The Things Network im Bojenprojekt.

LoRaWAN ist die Funk- und Netzwerkebene zwischen der Boje und der Cloud. TTN ist der Netzwerkdienst, der die LoRaWAN-Nachrichten empfängt, Geräte verwaltet und die Uplinks für weitere Systeme bereitstellt.

## Warum LoRaWAN?

Die Boje soll kleine Sensordaten über größere Entfernungen senden können und dabei möglichst wenig Energie verbrauchen.

LoRaWAN ist dafür gut geeignet, weil es für folgende Situationen entwickelt wurde:

- kleine Datenmengen
- geringe Datenrate
- große Reichweite
- Batteriebetrieb
- seltene Uplinks
- Sensornetzwerke

LoRaWAN ist nicht geeignet für:

- Video
- Audio
- große Dateien
- dauerhafte Datenströme
- sehr niedrige Latenz

## Grundbegriffe

| Begriff | Bedeutung |
|---|---|
| End Device | das Gerät, hier der ESP32 mit LoRa-Modul |
| Gateway | empfängt LoRa-Funksignale und leitet sie ins Internet weiter |
| Network Server | verwaltet LoRaWAN-Kommunikation |
| Application Server | stellt Anwendungsdaten bereit |
| Uplink | Nachricht vom Gerät zur Cloud |
| Downlink | Nachricht von der Cloud zum Gerät |
| Join | Aktivierung des Geräts im Netzwerk |

## TTN im Projekt

TTN übernimmt im Kurs:

```text
LoRaWAN-Geräteverwaltung
Join-Prozess
Empfang von Uplinks
Payload Formatter
MQTT-Integration
Live-Data-Ansicht
```

Die Studierenden arbeiten in TTN vor allem mit:

- Application
- End Device
- Live Data
- Payload Formatter
- API Key für MQTT

## LoRaWAN-Versionen

Im Generator werden für den Kurs die Begriffe verwendet:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

Die Version beeinflusst unter anderem das Schlüsselmodell. Wichtig ist, dass die Auswahl im Generator und in TTN zusammenpasst.

Wenn Firmware und TTN unterschiedliche Annahmen haben, schlägt der Join-Prozess fehl oder Uplinks können nicht korrekt verarbeitet werden.

## Region und Frequenzplan

In Europa wird üblicherweise EU868 verwendet.

Die Region ist nicht nur eine Anzeige. Sie beeinflusst:

- erlaubte Frequenzen
- Datenraten
- Sendeleistung
- Duty Cycle
- regionale Parameter

Für den Kurs muss der Generator zur TTN-Frequency-Plan-Auswahl passen.

## Uplink und fPort

Jeder Uplink enthält einen fPort. Im Kurs wird der fPort als Bedeutungsschlüssel verwendet.

| fPort | Bedeutung |
|---:|---|
| 1 | GPS |
| 2 | Wassertemperatur |
| 3 | pH |
| 4 | TDS |
| 5 | Trübung |
| 6 | PH4502C-Boardtemperatur |

Der Payload Formatter in TTN muss anhand des fPorts wissen, wie der Payload zu dekodieren ist.

## Payload Formatter

Der Payload Formatter ist JavaScript-Code in TTN. Er wandelt den binären Payload in lesbare Felder um.

Beispiel:

```text
fPort 4 + binärer Payload -> decoded_payload.tds_ppm
```

Wenn Firmware und Payload Formatter nicht zusammenpassen, sieht man in TTN entweder falsche Werte oder Decode-Fehler.

## TTN MQTT

TTN stellt Uplinks auch über MQTT bereit. Für den Kurs verwenden wir den europäischen TTN-Sandbox-Host:

```text
eu1.cloud.thethings.network
```

Für TLS-MQTT wird Port 8883 verwendet.

Typische Werte:

```text
host: eu1.cloud.thethings.network
port: 8883
username: <application-id>@ttn
topic: v3/<application-id>@ttn/devices/+/up
```

Das Passwort ist ein TTN API Key mit passenden Rechten.

## Was in TTN geprüft werden muss

Vor der Raspberry-Pi-Integration muss TTN funktionieren.

Prüfliste:

```text
[ ] Application existiert.
[ ] End Device existiert.
[ ] LoRaWAN-Version stimmt.
[ ] Region/Frequency Plan stimmt.
[ ] Schlüssel stimmen.
[ ] Gerät joint erfolgreich.
[ ] Live Data zeigt Uplinks.
[ ] Payload Formatter dekodiert fPorts 1..6.
[ ] MQTT API Key ist vorhanden.
```

## Häufige Fehler

| Symptom | Mögliche Ursache |
|---|---|
| Gerät joint nicht | falsche Keys, falsche Version, falsche Region |
| Uplinks kommen nicht | Funkproblem, Gateway-Abdeckung, falsche Firmware |
| Decode-Fehler | Payload Formatter passt nicht zur Firmware |
| MQTT verbindet nicht | falscher Host, falscher Username, falscher API Key |
| Werte sind leer | falscher fPort oder falsches Feld im Formatter |

## Fertig, wenn

```text
[ ] ESP32 joint TTN.
[ ] TTN Live Data zeigt Uplinks.
[ ] fPorts 1..6 werden korrekt dekodiert.
[ ] TTN MQTT-Zugangsdaten sind dokumentiert.
[ ] Die Gruppe kann erklären, wofür TTN in der Architektur zuständig ist.
```
