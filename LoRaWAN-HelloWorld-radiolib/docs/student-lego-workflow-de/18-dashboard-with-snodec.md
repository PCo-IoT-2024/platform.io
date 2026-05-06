# Dashboard mit mqttcli und SNode.C

Primärer Workstream: Student:in 5 — Dashboard, Dokumentation und Integrationstest

Schnittstellen: Student:in 4 stellt das laufende Raspberry-Pi-Backend und die MariaDB-Tabellen bereit; die Studierenden 1–3 liefern den fPort- und Decoded-Field-Vertrag.

Das Dashboard wird durch die Kursversion von `mqttcli` aus dem Branch `mqttcli-mariadb` bereitgestellt. Es ist derselbe Prozess, der den lokalen MQTT-Stream abonniert und Werte in MariaDB speichert.

Der Dashboard-Teil ist derzeit Teil der Kursvorbereitung und wird vor Workshop-Beginn implementiert. Die Studierenden starten das bereitgestellte `mqttcli`-Binary; sie benötigen kein separates Dashboard-Repository.

Für den Kurs lautet der Browser-Endpunkt:

```text
http://groupN.local:8080/
```

Dabei ist `groupN` `group1`, `group2`, `group3` oder `group4`.

## Was das Dashboard anzeigen soll

Ein nützliches erstes Dashboard sollte Folgendes anzeigen:

- neuesten Wert pro skalarem Sensor
- Zeitpunkt der letzten Aktualisierung
- Device ID
- fPort für skalare Messwerte
- neueste GPS-Position
- GPS-Verlaufstabelle oder Karten-Link
- pH-Verlauf
- TDS-Verlauf
- Trübungsverlauf
- Wassertemperaturverlauf
- PH4502C-Boardtemperaturverlauf
- einfache Statusinformationen

Das erste Dashboard muss optisch nicht perfekt sein. Es soll den vollständigen Datenpfad beweisen.

## Eingangsdaten des Dashboards

Das Dashboard liest aus zwei MariaDB-Tabellen:

| Tabelle | Verwendung im Dashboard |
|---|---|
| `measurements` | skalare Sensorwerte |
| `gps_positions` | GPS-Positionswerte |

Die Tabelle `measurements` enthält:

```text
id
received_at
application_id
device_id
f_port
value
```

Das Dashboard muss die numerische Spalte `value` über die Spalte `f_port` interpretieren.

| fPort | Dashboard-Bezeichnung |
|---:|---|
| 2 | Wassertemperatur °C |
| 3 | pH |
| 4 | TDS ppm |
| 5 | Trübung NTU |
| 6 | PH4502C-Boardtemperatur °C |

Die Tabelle `gps_positions` enthält:

```text
id
received_at
application_id
device_id
latitude
longitude
altitude
hdop
```

GPS fPort 1 wird aus der Tabelle `gps_positions` angezeigt, nicht aus der skalaren Tabelle `measurements`.

## mqttcli Storage/Dashboard starten

Verwenden Sie das bereitgestellte `mqttcli`-Binary aus dem Branch `mqttcli-mariadb`. Die genaue Kommandozeilen-Syntax muss zur finalen Kursimplementierung passen.

Dokumentierte Kursabsicht:

```text
mqttcli subscribes to ttn/# on localhost:1883
mqttcli inserts scalar values into water_buoy.measurements
mqttcli inserts GPS values into water_buoy.gps_positions
mqttcli serves the dashboard on port 8080 at /
```

Ersetzen Sie diesen Platzhalter vor Kursbeginn durch das exakte Kommando aus der finalen Ausgabe von `mqttcli --help`:

```bash
~/water-buoy/bin/mqttcli COURSE_STORAGE_DASHBOARD_OPTIONS
```

Ersetzen Sie das nicht durch geratene Syntax. Das Kommando muss aus der implementierten Version `mqttcli-mariadb` stammen.

## Dashboard öffnen

Von einem Laptop im selben Netzwerk:

```text
http://group1.local:8080/
http://group2.local:8080/
http://group3.local:8080/
http://group4.local:8080/
```

Falls `.local` nicht aufgelöst wird, verwenden Sie die IP-Adresse des Raspberry Pi:

```text
http://RASPBERRY_PI_IP:8080/
```

## Beispielabfragen hinter dem Dashboard

Neueste skalare Werte:

```sql
SELECT device_id, f_port, value, received_at
FROM measurements
ORDER BY received_at DESC
LIMIT 20;
```

TDS-Verlauf:

```sql
SELECT received_at, value
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

Neueste GPS-Positionen:

```sql
SELECT device_id, latitude, longitude, altitude, hdop, received_at
FROM gps_positions
ORDER BY received_at DESC
LIMIT 20;
```

Ein einfaches Dashboard kann Latitude und Longitude als Zahlen anzeigen. Eine bessere Version kann einen OpenStreetMap-Link für die neueste Position ergänzen.

## Minimal brauchbares Dashboard

Ein minimales Dashboard reicht aus, wenn es die vollständige Pipeline beweist.

```text
[ ] Browser kann http://groupN.local:8080/ öffnen.
[ ] Bereitgestellter mqttcli-Prozess kann sich mit MariaDB verbinden.
[ ] Neueste skalare Messwerte werden angezeigt.
[ ] Neueste GPS-Position wird angezeigt.
[ ] fPort ist sichtbar oder wird korrekt in skalare Sensorbezeichnungen übersetzt.
[ ] Mindestens eine Verlaufstabelle oder ein Diagramm ist sichtbar.
```

## Nachweise für den Abschlussbericht

Student:in 5 sollte Folgendes sammeln:

```text
[ ] Screenshot der Dashboard-Startseite
[ ] Screenshot mit mindestens einem realen skalaren Sensorwert
[ ] Screenshot oder Tabelle mit einer GPS-Position
[ ] passende MariaDB-SELECT-Ausgabe aus measurements
[ ] passende MariaDB-SELECT-Ausgabe aus gps_positions
[ ] passende TTN-Live-Data-Ausgabe
[ ] kurze Erklärung der fPort-zu-Label-Zuordnung und der getrennten GPS-Tabelle
```

## Fertig, wenn

```text
[ ] Bereitgestellter mqttcli-Storage-/Dashboard-Prozess startet am Raspberry Pi.
[ ] Browser öffnet http://groupN.local:8080/.
[ ] Dashboard zeigt skalare Werte aus measurements.
[ ] Dashboard zeigt GPS-Werte aus gps_positions.
[ ] Ein angezeigter skalarer Wert kann auf fPort, MariaDB-Zeile, lokale MQTT-Nachricht, TTN-Uplink und ESP32-Serial-Output zurückgeführt werden.
[ ] Eine angezeigte GPS-Position kann auf gps_positions, lokale MQTT-Nachricht, TTN-Uplink und ESP32-Serial-Output zurückgeführt werden.
```
