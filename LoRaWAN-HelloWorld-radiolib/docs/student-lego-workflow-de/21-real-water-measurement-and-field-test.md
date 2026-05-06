# Reale Wassermessung und Feldtest

Dieses Kapitel bereitet die Gruppe auf die reale Messsession am See vor.

Ein Feldtest ist anders als ein Labortest. Im Labor können Studierende Geräte zurücksetzen, Kabel neu verbinden, Serial Monitors öffnen und Einstellungen ändern. Am See muss das System als integrierte Messkette funktionieren.

Das Ziel ist nicht nur, Zahlen zu sammeln. Das Ziel ist, interpretierbare Wasserqualitätsdaten zusammen mit genügend Kontext zu erfassen, damit die Zahlen verstanden werden können.

---

## Ziel des Feldtests

Das minimale Feldtest-Ziel lautet:

```text
Ein realer Sensorwert wird im Seewasser gemessen, von der ESP32-Boje gesendet, in TTN dekodiert, zum Raspberry Pi gebridged, in MariaDB gespeichert und im Dashboard angezeigt.
```

Ein guter Feldtest dokumentiert zusätzlich den Umgebungskontext:

- Datum und Uhrzeit
- Wetter
- Sonne oder Schatten
- ungefähre Wasserbeschaffenheit
- Sensortiefe
- GPS-Position
- Batteriestand
- sichtbare Störungen
- Kalibrierzustand

Ohne Kontext sind Messwerte deutlich schwerer zu interpretieren.

---

## Vor dem Weg zum See

Beginnen Sie den Feldtest nicht mit einem ungetesteten System.

Die Gruppe sollte diese Checkliste zuerst im Innenraum abschließen:

```text
[ ] ESP32-Firmware baut und lässt sich hochladen.
[ ] TTN Live Data zeigt dekodierte Uplinks.
[ ] Payload Formatter funktioniert ohne Fehler.
[ ] Raspberry Pi ist per SSH erreichbar.
[ ] mqttbroker läuft.
[ ] mqttbridge leitet TTN-Uplinks weiter.
[ ] mqttcli speichert Zeilen in MariaDB.
[ ] Dashboard zeigt aktuelle Messwerte.
[ ] Batterie ist geladen.
[ ] Sensorverdrahtung ist mechanisch stabil.
[ ] Elektronik ist vor Wasser geschützt.
```

Der Feldtest soll das vollständige System bestätigen, nicht zum ersten Mal grundlegende Setup-Probleme aufdecken.

---

## Mechanische Vorbereitung

Bevor etwas in die Nähe des Sees gebracht wird, prüfen Sie:

```text
[ ] Elektronikgehäuse geschlossen
[ ] Kabeldurchführungen abgedichtet
[ ] Sensorkabel zugentlastet
[ ] Batterie mechanisch fixiert
[ ] Antenne sicher montiert
[ ] GPS hat nach Möglichkeit freie Sicht zum Himmel
[ ] Sensoren können das Wasser erreichen
[ ] Boje kann nicht unbeabsichtigt wegtreiben
[ ] Bergungsleine oder sicherer Zugang geplant
```

Platzieren Sie kein ungeschütztes Development-Board direkt über offenem Wasser.

---

## Sensorvorbereitung

Vor der Messung:

- pH-Sonde vorsichtig spülen
- pH-Kalibrierwerte prüfen
- TDS-Sonde auf Sauberkeit prüfen
- Fenster des Trübungssensors reinigen
- Zustand der wasserdichten DS18B20-Sonde prüfen
- analoge Sensorversorgung prüfen
- Sensorwerte nach Möglichkeit in sauberem oder Referenzwasser plausibilisieren

Für pH sollten vor der realen Messsession Kalibrierpuffer verwendet werden, wenn die Zeit es erlaubt.

Für TDS und Trübung sollten zumindest Plausibilitätsprüfungen in klarem Wasser und in einer bekannten trüberen oder leitfähigeren Probe durchgeführt werden.

---

## Messablauf am See

Vorgeschlagener Ablauf:

1. Raspberry-Pi-Backend starten.
2. mqttbroker, mqttbridge, Storage-Prozess und Dashboard starten.
3. TTN Live Data auf Laptop oder Tablet öffnen.
4. ESP32-Boje einschalten.
5. Auf LoRaWAN Join oder Session Restore warten.
6. Auf den vollständigen fPort-Zyklus warten.
7. Sensoren stabil ins Wasser bringen.
8. Uhrzeit und Kontext dokumentieren.
9. TTN Live Data beobachten.
10. Lokales Dashboard beobachten.
11. System mehrere Zyklen laufen lassen.
12. Datenbank-/Dashboard-Nachweise exportieren oder dokumentieren.

Ein einzelner Uplink reicht nicht aus, um Stabilität zu beurteilen. Lassen Sie das System lange genug laufen, um wiederholte Werte zu sehen.

---

## Vorlage für das Kontextprotokoll

Jede Gruppe sollte ein kurzes Kontextprotokoll erstellen.

Beispiel:

```text
Gruppe:
Datum:
Ort:
Startzeit:
Endzeit:
Wetter:
Sonne/Schatten:
Erscheinungsbild des Wassers:
ungefähre Sensortiefe:
Boje-Position:
Batteriezustand vor dem Test:
Batteriezustand nach dem Test:
Kalibrierung durchgeführt um:
Beobachtete Probleme:
```

Dieses Protokoll gehört in den Abschlussbericht.

---

## Worauf während der Messung zu achten ist

Technische Beobachtungen:

- Läuft die fPort-Sequenz weiter?
- Empfängt TTN die Uplinks zuverlässig?
- Bekommt GPS einen Fix?
- Aktualisiert sich das Dashboard?
- Nehmen die Datenbankzeilen zu?
- Sind die Werte plausibel?

Physikalische Beobachtungen:

- Ist der Sensor vollständig im Wasser?
- Bewegt sich die Boje?
- Erwärmt Sonnenlicht das Gehäuse?
- Beeinflussen Luftblasen oder Schmutz die Sensoren?
- Ist das Wasser sichtbar klar, trüb, grün oder aufgewühlt?

---

## Sicherheit am Wasser

Grundlegende Sicherheitsregeln:

- nicht allein in Wassernähe arbeiten
- keine unsicheren Bereiche betreten
- Elektronik geschützt halten
- Batterien geschützt halten
- gesamte Ausrüstung wieder bergen
- keine Sonden, Kabel oder Kunststoffteile zurücklassen

Der Feldtest ist eine technische Übung und kein Grund, persönliche Risiken einzugehen.

---

## Fertig, wenn ...

Der Feldtest ist abgeschlossen, wenn:

```text
[ ] mindestens ein vollständiger fPort-Zyklus beobachtet wurde
[ ] TTN reale Seemesswerte dekodiert hat
[ ] Raspberry Pi die Werte in MariaDB gespeichert hat
[ ] Dashboard die Werte angezeigt hat
[ ] Feldkontext dokumentiert wurde
[ ] mindestens ein Problem oder eine Grenze dokumentiert wurde
[ ] Ausrüstung sicher geborgen wurde
```

Eine gute Gruppe kann nicht nur erklären, welche Werte gemessen wurden, sondern auch, wie zuverlässig diese Werte sind und wodurch sie beeinflusst worden sein könnten.
