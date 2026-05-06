# Prozesse starten und finaler Ende-zu-Ende-Test

**Primäre Workstreams:** Student:in 4 — Backend-Prozesse; Student:in 5 — Integrationstest und Nachweise

Dieses Kapitel erklärt, wie das finale Kurs-Backend manuell gestartet und der vollständige Datenpfad geprüft wird.

Das Kurs-Setup benötigt keine systemd-Services. Die Studierenden starten die erforderlichen Prozesse in Terminals, damit sie Logs sehen und das System verstehen können.

## Manuelle Startreihenfolge

Starten Sie in dieser Reihenfolge:

1. MariaDB
2. lokaler `mqttbroker`
3. `mqttbridge` mit `bridge-config.json`
4. lokale MQTT-Inspektion mit `mqttcli`
5. bereitgestellter `mqttcli`-Storage-/Dashboard-Prozess

Das bereitgestellte `mqttcli` aus dem Branch `mqttcli-mariadb` ist der Kursprozess, der Werte in MariaDB speichert und das Dashboard auf Port 8080 bereitstellt. Skalare Sensorwerte werden in `measurements` gespeichert; GPS-Positionen werden in `gps_positions` gespeichert. Der Dashboard-Teil wird vor Workshop-Beginn in der Kursversion von `mqttcli` implementiert.

Verwenden Sie keine erfundenen Kommandozeilenformen. Für MQTTBridge verwendet der Kurs-Workflow eine Bridge-Definitionsdatei. Für Storage/Dashboard verwenden Sie das finale `mqttcli`-Kommando aus dem implementierten Kurs-Binary.

## MariaDB prüfen

```bash
sudo systemctl status mariadb
```

Bei Bedarf starten:

```bash
sudo systemctl start mariadb
```

## Terminal 1: mqttbroker starten

```bash
mqttbroker \
  in-mqtt \
    local --host 0.0.0.0 \
          --port 1883
```

Der lokale Broker lauscht auf Port 1883.

## Terminal 2: mqttbridge starten

Starten Sie die Bridge mit der JSON-Definitionsdatei aus dem MQTTBridge-Kapitel:

```bash
mqttbridge \
  bridge --definition ~/water-buoy/config/bridge-config.json
```

Die Datei enthält TTN-Broker, lokalen Broker, Zugangsdaten, Topics und Präfixe.

## Terminal 3: lokales MQTT inspizieren

Verwenden Sie `mqttcli` mit der dokumentierten MQTT-Publish-/Subscribe-Syntax:

```bash
mqttcli \
  in-mqtt \
    remote --host 127.0.0.1 \
           --port 1883 \
    sub --topic 'ttn/#'
```

Diese Prüfung ist erfolgreich, wenn TTN-Uplinks am lokalen Broker erscheinen.

## Terminal 4: bereitgestelltes mqttcli Storage/Dashboard starten

Starten Sie den bereitgestellten `mqttcli`-Storage-/Dashboard-Prozess aus dem Branch `mqttcli-mariadb`.

Der Prozess muss Folgendes tun:

```text
lokales MQTT-Topic ttn/#
  -> dekodiertes TTN-JSON parsen
  -> skalare Werte in die MariaDB-Tabelle measurements einfügen
  -> GPS-Positionen in die MariaDB-Tabelle gps_positions einfügen
  -> Dashboard auf http://groupN.local:8080/ bereitstellen
```

Das exakte Kommando muss vor Kursbeginn aus der final implementierten Ausgabe von `mqttcli --help` übernommen werden:

```bash
mqttcli <course-storage-dashboard-options>
```

Der Dashboard-Endpunkt lautet:

```text
http://groupN.local:8080/
```

## Finaler Ende-zu-Ende-Test

Das vollständige System funktioniert, wenn ein echter ESP32-Uplink durch die ganze Kette läuft.

Verfolgen Sie einen skalaren Wert:

```text
ESP32 Serial Monitor
  -> TTN Live Data
  -> lokales MQTT-Topic am Raspberry Pi
  -> MariaDB-Zeile in measurements
  -> Dashboard-Anzeige
```

Beispiel-Trace für TDS:

1. ESP32 Serial Monitor zeigt einen fPort-4-Uplink.
2. TTN Live Data dekodiert `tds_ppm`.
3. Lokaler MQTT-Subscriber sieht einen weitergeleiteten Uplink unter `ttn/#`.
4. MariaDB enthält eine Zeile mit `f_port = 4` und dem numerischen TDS-Wert.
5. Dashboard zeigt den neuesten TDS-Wert.

Verfolgen Sie eine GPS-Position:

```text
ESP32 Serial Monitor
  -> TTN Live Data
  -> lokales MQTT-Topic am Raspberry Pi
  -> MariaDB-Zeile in gps_positions
  -> Dashboard-Anzeige oder GPS-Tabelle
```

Beispiel-Trace für GPS:

1. ESP32 Serial Monitor zeigt einen fPort-1-Uplink.
2. TTN Live Data dekodiert `latitude`, `longitude`, `altitude` und `hdop`.
3. Lokaler MQTT-Subscriber sieht den weitergeleiteten GPS-Uplink unter `ttn/#`.
4. MariaDB enthält eine Zeile in `gps_positions`.
5. Dashboard zeigt die neueste GPS-Position.

## SQL-Verifikation

```bash
mariadb -u water_buoy -p water_buoy
```

Skalare Messwerte:

```sql
SELECT id, received_at, application_id, device_id, f_port, value
FROM measurements
ORDER BY id DESC
LIMIT 20;
```

GPS-Positionen:

```sql
SELECT id, received_at, application_id, device_id, latitude, longitude, altitude, hdop
FROM gps_positions
ORDER BY id DESC
LIMIT 20;
```

## Finale Checkliste

```text
[ ] ESP32-Firmware geflasht.
[ ] TTN-Gerät joint erfolgreich.
[ ] TTN Payload Formatter dekodiert fPorts 1..6.
[ ] TTN MQTT API Key existiert.
[ ] Raspberry Pi ist per SSH als water@groupN.local erreichbar.
[ ] MariaDB läuft.
[ ] mqttbroker läuft lokal auf Port 1883.
[ ] mqttbridge läuft mit bridge-config.json.
[ ] TTN-Uplinks erscheinen auf lokalen ttn/#-Topics.
[ ] Bereitgestellter mqttcli-Storage-/Dashboard-Prozess fügt skalare Messwerte in MariaDB ein.
[ ] Bereitgestellter mqttcli-Storage-/Dashboard-Prozess fügt GPS-Positionen in MariaDB ein.
[ ] Bereitgestellter mqttcli-Storage-/Dashboard-Prozess stellt Port 8080 bereit.
[ ] Browser zeigt aktuelle Messwerte und neueste GPS-Position unter http://groupN.local:8080/.
[ ] Studierende können den vollständigen Datenpfad erklären.
```

Wenn ein Wert im Dashboard fehlt, debuggen Sie rückwärts:

```text
Dashboard -> MariaDB -> mqttcli Storage/Dashboard -> lokales MQTT -> mqttbridge -> TTN -> ESP32
```

Wenn ein Wert TTN nie erreicht, debuggen Sie vorwärts:

```text
ESP32 -> LoRaWAN Join -> TTN Live Data -> Formatter
```
