# Lokalen MQTT-Broker und TTN-Bridge betreiben

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

**Schnittstellen:** Student:in 3 stellt den TTN-MQTT-Zugang bereit; Student:in 5 prüft, ob weitergeleitete Nachrichten im Dashboard-Pfad sichtbar werden.

Dieses Kapitel verbindet TTN-MQTT mit dem Raspberry Pi der Gruppe.

Der Zielpfad ist:

```text
TTN MQTT on eu1.cloud.thethings.network
  -> mqttbridge using bridge-config.json
  -> local mqttbroker on groupN.local:1883
  -> local mqttcli subscriber / later storage workflow
```

Die wichtige Korrektur lautet: **MQTTBridge wird über eine Bridge-JSON-Datei konfiguriert.** Für den Kurs-Workflow sollen Studierende keine lange `mqttbridge`-Kommandozeile zusammenbauen. Sie sollen eine Bridge-Konfigurationsdatei erstellen und die Bridge mit dieser Datei starten.

## Lokalen mqttbroker starten

Das MQTTSuite-README zeigt die Kommando-Struktur von MQTTBroker mit Verbindungsinstanzen. Für einen einfachen MQTT-Listener über TCP/IPv4 verwenden Sie die Instanz `in-mqtt` mit einem lokalen Endpoint.

Am Raspberry Pi:

```bash
~/water-buoy/bin/mqttbroker \
  in-mqtt \
    local --host 0.0.0.0 \
          --port 1883
```

Für den ersten Labortest bleibt dieser Prozess im Vordergrund, damit die Logs sichtbar sind.

Der lokale Kurs-Broker verwendet:

```text
host: group Raspberry Pi
port: 1883
authentication: none
TLS: no
```

Der MQTTSuite-Broker kann weitere Instanzen und ein Webinterface bereitstellen. Für den Kurs-Bridge-Pfad reicht aber einfaches lokales MQTT auf Port 1883.

## Lokalen Broker mit mqttcli testen

Öffnen Sie zwei SSH-Terminals zum Raspberry Pi.

Terminal 1 abonniert alle lokalen Topics:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic '#'
```

Terminal 2 veröffentlicht eine lokale Testnachricht:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    pub --topic 'test/hello' \
        --message 'Hello MQTT'
```

Der lokale Broker-Test ist fertig, wenn Terminal 1 die Nachricht empfängt.

## TTN-MQTT-Werte

Für The Things Stack Sandbox in Europa verwenden Sie:

```text
host: eu1.cloud.thethings.network
port: 8883
TLS: yes
username format: <application-id>@ttn
password: TTN API key
topic: v3/<application-id>@ttn/devices/+/up
```

Application ID und Device ID werden von den Studierenden in TTN gewählt.

## Bridge-Konfigurationsdatei erstellen

Erstellen Sie ein Konfigurationsverzeichnis:

```bash
mkdir -p ~/water-buoy/config
```

Erstellen Sie die Bridge-Konfiguration:

```bash
nano ~/water-buoy/config/bridge-config.json
```

Vorlage für Gruppe 1, Application `water-buoy-group1`:

```json
{
  "bridges": [
    {
      "name": "ttn-to-local",
      "prefix": "",
      "brokers": [
        {
          "network": {
            "instance_name": "ttn-mqtts",
            "protocol": "in",
            "in": {
              "host": "eu1.cloud.thethings.network",
              "port": 8883
            },
            "encryption": "tls",
            "transport": "stream"
          },
          "prefix": "ttn/",
          "mqtt": {
            "client_id": "group1-ttn-bridge",
            "clean_session": true,
            "username": "water-buoy-group1@ttn",
            "password": "NNSXS.YOUR_TTN_API_KEY_HERE",
            "loop_prevention": true
          },
          "topics": [
            {
              "topic": "v3/water-buoy-group1@ttn/devices/+/up",
              "qos": 0
            }
          ]
        },
        {
          "network": {
            "instance_name": "local-mqtt",
            "protocol": "in",
            "in": {
              "host": "127.0.0.1",
              "port": 1883
            },
            "encryption": "legacy",
            "transport": "stream"
          },
          "prefix": "local/",
          "mqtt": {
            "client_id": "group1-local-bridge",
            "clean_session": true,
            "loop_prevention": true
          },
          "topics": []
        }
      ]
    }
  ]
}
```

Wichtige Hinweise:

- Ersetzen Sie `water-buoy-group1` durch die echte TTN Application ID.
- Ersetzen Sie den API-Key-Platzhalter durch den echten TTN API Key.
- Committen Sie diese Datei nicht in ein öffentliches Repository, wenn sie einen echten TTN API Key enthält.
- Der TTN-Broker-Eintrag abonniert das TTN-Uplink-Topic.
- Der lokale Broker-Eintrag hat absichtlich eine leere `topics`-Liste. Dadurch ist die Bridge im Kurs einseitig: TTN-Uplinks werden zum lokalen Broker weitergeleitet, aber lokale Nachrichten werden nicht abonniert und nicht zurück in Richtung TTN weitergeleitet.
- MQTTBridge leitet Nachrichten zwischen Brokern derselben Bridge gemäß den konfigurierten Subscriptions und Präfixen weiter.

Mit der Vorlage oben erscheinen von TTN stammende Nachrichten, die zum lokalen Broker weitergeleitet wurden, mit dem Präfix `ttn/`.

## mqttbridge starten

Starten Sie die Bridge mit der JSON-Definitionsdatei:

```bash
~/water-buoy/bin/mqttbridge \
  bridge --definition ~/water-buoy/config/bridge-config.json
```

Das ist der Kurs-Workflow. Verwenden Sie keine lange Liste von Bridge-Verbindungsoptionen auf der Kommandozeile.

## Bridge-Betrieb prüfen

Wenn `mqttbroker` und `mqttbridge` laufen, warten Sie, bis der ESP32 einen Uplink sendet.

Abonnieren Sie in einem weiteren Terminal den lokalen Broker:

```bash
~/water-buoy/bin/mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

Sie sind fertig, wenn ein TTN-Uplink als lokale MQTT-Nachricht am Raspberry Pi erscheint.

## Debug-Reihenfolge

Wenn keine lokale Nachricht erscheint, debuggen Sie in dieser Reihenfolge:

```text
1. TTN Live Data zeigt dekodierte Uplinks.
2. Der TTN API Key hat Leserechte für Application Traffic.
3. bridge-config.json enthält den korrekten TTN-Host, Port, Username, Password und Topic.
4. local mqttbroker läuft auf Port 1883.
5. mqttbridge läuft mit der Bridge-Definitionsdatei.
6. mqttcli abonniert das korrekte lokale Topic mit Präfix.
```

## Fertig, wenn

```text
[ ] mqttbroker läuft lokal auf Port 1883.
[ ] mqttcli kann lokal publishen und subscriben.
[ ] bridge-config.json existiert und enthält die TTN- und lokalen Broker-Definitionen.
[ ] Der lokale Broker-Eintrag in bridge-config.json abonniert nicht '#'.
[ ] mqttbridge startet mit bridge --definition.
[ ] TTN-Uplinks erscheinen unter lokalen ttn/#-Topics.
[ ] Die Gruppe hat ihre Application ID und Device ID dokumentiert.
```
