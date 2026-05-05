# MQTT-Grundlagen für das Bojen-Backend

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

MQTT ist das Nachrichtensystem, das im Backend des Projekts verwendet wird. Dieses Kapitel erklärt die Grundbegriffe und die konkrete Rolle von MQTT im Bojensystem.

## Was ist MQTT?

MQTT ist ein leichtgewichtiges Publish/Subscribe-Protokoll.

Es besteht aus:

| Begriff | Bedeutung |
|---|---|
| Broker | zentrale Nachrichtenstelle |
| Publisher | sendet Nachrichten an ein Topic |
| Subscriber | abonniert Topics und empfängt Nachrichten |
| Topic | hierarchischer Name einer Nachricht |

Ein Publisher sendet nicht direkt an einen Subscriber. Beide sprechen mit dem Broker.

## Publish/Subscribe

Klassisches Client/Server-Denken:

```text
Client fragt Server direkt
```

MQTT-Denken:

```text
Publisher -> Broker -> Subscriber
```

Der Publisher muss nicht wissen, wer die Nachricht empfängt. Der Subscriber muss nicht wissen, wer die Nachricht erzeugt hat. Beide müssen nur das Topic kennen.

## Topics

Topics sind hierarchische Namen.

Beispiele:

```text
ttn/water-buoy-group1/buoy-01/up
sensors/buoy-01/tds
status/backend/mqttbridge
```

Das Wildcard-Zeichen `#` bedeutet „alles darunter“.

Beispiel:

```text
ttn/#
```

abonniert alle Topics, die mit `ttn/` beginnen.

## Zwei MQTT-Welten im Projekt

Im Projekt gibt es zwei MQTT-Bereiche:

```text
TTN MQTT in der Cloud
lokaler MQTT-Broker am Raspberry Pi
```

TTN MQTT stellt LoRaWAN-Uplinks bereit. Der lokale Broker sammelt die Nachrichten am Raspberry Pi.

Die Bridge verbindet beide:

```text
TTN MQTT -> mqttbridge -> local mqttbroker
```

## Warum nicht direkt aus TTN speichern?

Man könnte theoretisch direkt aus TTN in eine Datenbank schreiben. Für den Kurs ist die lokale MQTT-Schicht aber didaktisch wertvoll.

Sie zeigt:

- wie Nachrichten entkoppelt werden
- wie lokale Subscriber arbeiten
- wie ein lokaler Broker als Integrationspunkt dient
- wie Debugging mit `mqttcli` funktioniert
- wie mehrere lokale Prozesse dieselben Nachrichten sehen können

## Lokaler Broker

Der lokale Broker läuft am Raspberry Pi auf Port 1883.

```text
host: groupN.local oder 127.0.0.1
port: 1883
authentication: none im Kursnetz
```

Er empfängt Nachrichten von `mqttbridge` und stellt sie lokalen Clients bereit.

## mqttbridge

`mqttbridge` verbindet TTN MQTT mit dem lokalen Broker. Im Kurs wird dafür eine JSON-Konfigurationsdatei verwendet:

```text
bridge-config.json
```

Die Bridge soll im Kurs einseitig arbeiten:

```text
TTN -> local MQTT
```

Die lokale Seite darf nicht `#` abonnieren, damit lokale Nachrichten nicht zurück Richtung TTN gebridged werden.

## mqttcli

`mqttcli` wird im Kurs für zwei Dinge verwendet:

1. zum Prüfen lokaler MQTT-Nachrichten
2. als bereitgestellter Storage/Dashboard-Prozess im Branch `mqttcli-mariadb`

Zum Prüfen kann `mqttcli` ein Topic abonnieren:

```bash
mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

Damit sieht man, ob TTN-Uplinks am lokalen Broker ankommen.

## Nachrichtenfluss

Der Nachrichtenfluss im Kurs:

```text
ESP32
  -> LoRaWAN
  -> TTN
  -> TTN MQTT topic
  -> mqttbridge
  -> local mqttbroker
  -> mqttcli
  -> MariaDB + Dashboard
```

## Häufige MQTT-Probleme

| Problem | Mögliche Ursache |
|---|---|
| keine lokale Nachricht | Bridge läuft nicht oder falsches Topic |
| Verbindung zu TTN schlägt fehl | falscher API Key oder falscher Username |
| lokaler Broker nicht erreichbar | mqttbroker läuft nicht oder falscher Port |
| Subscriber sieht nichts | falsches Topic oder falsches Prefix |
| Loop-Gefahr | lokale Seite subscribed fälschlich auf `#` |

## Fertig, wenn

```text
[ ] Die Gruppe kann Broker, Publisher, Subscriber und Topic erklären.
[ ] Die Gruppe versteht TTN MQTT und lokalen MQTT-Broker als getrennte Bereiche.
[ ] Die Gruppe kann mit mqttcli lokale Nachrichten abonnieren.
[ ] Die Gruppe versteht, warum die Bridge im Kurs einseitig sein soll.
```
