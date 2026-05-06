# Abschlusspräsentation und Fazit

Die Abschlusspräsentation ist nicht nur eine Demonstration, dass das System einmal funktioniert hat. Sie ist der Moment, in dem die Gruppe das vollständige System, seinen Datenpfad, seine Grenzen und sinnvolle Verbesserungen für eine nächste Version erklärt.

Für Management-Studierende ist das Fazit besonders wichtig. Ein funktionierender Prototyp ist noch kein betriebsfähiges Produkt. Die Abschlussdiskussion soll technisches Verständnis und Projekturteil zeigen.

---

## Was die Gruppe präsentieren soll

Jede Gruppe sollte Folgendes präsentieren:

- Projektziel
- Ende-zu-Ende-Architektur
- Gruppenorganisation
- Sensorauswahl
- LoRaWAN-/TTN-Setup
- Raspberry-Pi-Backend
- Datenbankspeicherung
- Dashboard
- Ergebnisse der Feldmessung
- Probleme und Grenzen
- vorgeschlagene Verbesserungen

Die Präsentation soll klar machen, wie ein Messwert durch das gesamte System gewandert ist.

---

## Erforderliche Ende-zu-Ende-Erklärung

Die Gruppe sollte diese Kette erklären können:

```text
sensor
  -> ESP32 firmware
  -> LoRaWAN uplink
  -> TTN payload formatter
  -> TTN MQTT
  -> mqttbridge
  -> local mqttbroker
  -> mqttcli
  -> MariaDB
  -> SNode.C dashboard
  -> browser
```

Jede:r Studierende sollte den eigenen Teil erklären und zeigen, wie dieser Teil mit den anderen Teilen verbunden ist.

---

## Nachweise, die enthalten sein sollten

Gute Abschlussnachweise umfassen:

```text
[ ] photo of the buoy or lab setup
[ ] screenshot of generator settings
[ ] serial monitor output
[ ] TTN live data with decoded payload
[ ] local MQTT subscriber output
[ ] MariaDB SELECT query result
[ ] dashboard screenshot
[ ] field-test context log
[ ] calibration table
```

Die Gruppe sollte sich nicht nur auf einen Screenshot verlassen. Ziel ist, den vollständigen Pfad zu belegen.

---

## Messqualität diskutieren

Die Abschlusspräsentation sollte eine Diskussion der Messqualität enthalten.

Fragen:

- Welche Sensoren wurden kalibriert?
- Welche Referenzen wurden verwendet?
- Welche Werte sind absolute Messwerte?
- Welche Werte sind vor allem Trendindikatoren?
- Was könnte die Messwerte stören?
- Wie stabil waren die Messwerte über die Zeit?
- War die Wassermessung plausibel?

Die richtige Antwort ist nicht immer: „Unser Sensorwert ist exakt.“ Eine reife Antwort lautet oft:

```text
This value is useful as a trend, but not a laboratory-grade measurement.
```

---

## Systemzuverlässigkeit diskutieren

Die Gruppe sollte auch die Zuverlässigkeit diskutieren.

Fragen:

- Hat die LoRaWAN-Abdeckung zuverlässig funktioniert?
- Hat GPS einen Fix bekommen?
- Lief das Raspberry-Pi-Backend durchgehend?
- Hat MariaDB alle erwarteten Werte gespeichert?
- Hat sich das Dashboard korrekt aktualisiert?
- Was passiert nach einem Stromausfall?
- Was passiert, wenn WLAN oder Internet nicht verfügbar ist?

---

## Vorgeschlagene Struktur für den Abschlussbericht

```text
1. Introduction and project goal
2. Group organization and responsibilities
3. System architecture
4. Hardware and sensor setup
5. Firmware and generator configuration
6. TTN cloud setup
7. Raspberry Pi backend
8. Database and dashboard
9. Calibration
10. Field measurement
11. Results
12. Limitations
13. Improvements
14. Conclusion
```

Jede:r Studierende sollte zu dem Abschnitt beitragen, der zum eigenen Workstream gehört.

---

## Verbesserungsideen

Mögliche Verbesserungen für eine nächste Version:

- besser wasserdichtes Gehäuse
- geschaltete Sensorversorgungen
- ESP32-Board mit geringerem Stromverbrauch
- größeres Solarpanel oder besseres Batteriemanagement
- robusteres Dashboard
- besseres Datenbankschema
- Speicherung der Kalibrierung in der Datenbank
- mehrere Bojen
- Kartenvisualisierung
- CSV-Export
- Alarmierung bei Grenzwertüberschreitung
- bessere Sensorreferenzen für TDS und Trübung

---

## Abschließendes Fazit

Das Fazit sollte beantworten:

```text
What did we build?
What did we measure?
How reliable is it?
What did we learn?
What would we improve?
```

Ein gutes Fazit verbindet technische Arbeit mit Systemverständnis. Es listet nicht nur ausgeführte Kommandos auf.

---

## Erfolgskriterium des Kurses

Das Kursprojekt ist erfolgreich, wenn die Gruppe einen vollständigen Datenpfad vom physischen Seewasser bis zum Browser-Dashboard demonstrieren und erklären kann, inklusive der Grenzen der Messung und der Verantwortlichkeiten jeder Systemschicht.
