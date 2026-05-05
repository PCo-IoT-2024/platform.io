# Bauen, Flashen, Testen und Fehlersuche

**Primärer Workstream:** Student:in 1 — ESP32-Firmware und LoRaWAN-Gerät

Dieses Kapitel beschreibt den praktischen Arbeitsablauf zum Bauen, Flashen und Testen der ESP32-Firmware.

## Grundprinzip

Nicht alles auf einmal testen.

Besser ist:

```text
bauen
flashen
seriellen Monitor öffnen
einen erwarteten Schritt prüfen
Fehler eingrenzen
nächsten Schritt testen
```

## Firmware bauen

Im Projektverzeichnis:

```bash
pio run
```

Ein erfolgreicher Build endet ohne Fehler. Warnungen sollen gelesen werden, aber nicht jede Warnung verhindert den Build.

Wenn der Build fehlschlägt, zuerst prüfen:

```text
[ ] richtige Branch?
[ ] platformio.ini vorhanden?
[ ] richtiger Environment-Name?
[ ] Bibliotheken verfügbar?
[ ] Syntaxfehler in Code oder Build-Flags?
```

## Firmware flashen

ESP32 über USB anschließen und flashen:

```bash
pio run -t upload
```

Falls der falsche Port verwendet wird, zuerst Geräte auflisten:

```bash
pio device list
```

Typische Probleme:

| Symptom | Mögliche Ursache |
|---|---|
| Upload startet nicht | falscher Port oder USB-Kabel ohne Datenleitungen |
| ESP32 wird nicht erkannt | CP2102/USB-Treiber fehlt |
| Upload hängt | Boot-Taste nötig oder serieller Monitor blockiert Port |
| Permission denied | Linux-Benutzer nicht in passender Gruppe |

## Seriellen Monitor öffnen

Nach dem Flashen:

```bash
pio device monitor
```

Der serielle Monitor ist das wichtigste Werkzeug für die erste Fehlersuche.

Er zeigt zum Beispiel:

```text
Wake from sleep
Initialize radio
Restored session
Sending fPort = ...
No downlink received
Go to sleep
```

## Was zuerst funktionieren muss

Die Reihenfolge ist wichtig.

```text
1. Build funktioniert.
2. Upload funktioniert.
3. Serieller Monitor funktioniert.
4. Firmware startet.
5. Radio initialisiert.
6. LoRaWAN Join oder Session Restore funktioniert.
7. TTN Live Data sieht Uplinks.
8. Payload Formatter dekodiert korrekt.
9. Sensorwerte sind plausibel.
```

Wenn Schritt 4 nicht funktioniert, ist TTN noch irrelevant. Wenn TTN nichts empfängt, ist MariaDB noch irrelevant.

## TTN prüfen

In TTN Live Data prüfen:

```text
[ ] Join Accept sichtbar?
[ ] Uplinks sichtbar?
[ ] richtiger fPort?
[ ] decoded_payload vorhanden?
[ ] keine JavaScript-Fehler im Formatter?
```

## Payload Formatter prüfen

Wenn TTN einen Decode-Fehler zeigt, liegt das Problem meist im Formatter oder in der Erwartung an den Payload.

Typische Ursachen:

- falscher Formatter in TTN
- alte Version des Formatters
- Syntaxfehler in JavaScript
- Firmware sendet anderes Format als der Formatter erwartet
- falscher fPort

## Sensorwerte prüfen

Ein technisch erfolgreicher Uplink bedeutet noch nicht, dass der Messwert sinnvoll ist.

Für jeden Sensor prüfen:

```text
[ ] Wert ändert sich bei geänderter Umgebung?
[ ] Wert liegt grob im erwarteten Bereich?
[ ] Kalibrierwerte wurden korrekt eingetragen?
[ ] Sensor ist richtig angeschlossen?
[ ] Sensor wird korrekt versorgt?
```

## Debugging rückwärts und vorwärts

Wenn im Dashboard nichts erscheint, rückwärts prüfen:

```text
dashboard -> MariaDB -> mqttcli -> local MQTT -> mqttbridge -> TTN -> ESP32
```

Wenn schon TTN nichts empfängt, vorwärts prüfen:

```text
ESP32 -> LoRaWAN -> TTN Live Data
```

## Logs sammeln

Für den Abschlussbericht sind Logs wichtig.

Sammeln:

```text
serieller Monitor
TTN Live Data Screenshot
mqttcli Subscribe-Ausgabe
MariaDB SELECT-Ausgabe
Dashboard Screenshot
```

## Fertig, wenn

```text
[ ] Firmware baut.
[ ] Firmware flasht.
[ ] serieller Monitor zeigt Start und Uplink-Versuche.
[ ] TTN empfängt Uplinks.
[ ] Payload Formatter dekodiert fPorts 1..6.
[ ] mindestens ein Sensorwert ist plausibel.
[ ] Logs und Screenshots sind dokumentiert.
```
