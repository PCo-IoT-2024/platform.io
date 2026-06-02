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
Sensor
  -> ESP32-Firmware
  -> LoRaWAN-Uplink
  -> TTN Payload Formatter
  -> TTN MQTT
  -> mqttbridge
  -> lokaler mqttbroker
  -> mqttcli
  -> MariaDB
  -> SNode.C-Dashboard
  -> Browser
```

Jede:r Studierende sollte den eigenen Teil erklären und zeigen, wie dieser Teil mit den anderen Teilen verbunden ist.

---

## Nachweise, die enthalten sein sollten

Gute Abschlussnachweise umfassen:

```text
[ ] Foto der Boje oder des Laboraufbaus
[ ] Screenshot der Generator-Einstellungen
[ ] Serial-Monitor-Ausgabe
[ ] TTN Live Data mit dekodierter Payload
[ ] Ausgabe des lokalen MQTT-Subscribers
[ ] Ergebnis einer MariaDB-SELECT-Abfrage
[ ] Dashboard-Screenshot
[ ] Feldtest-Kontextprotokoll
[ ] Kalibriertabelle
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
Dieser Wert ist als Trend nützlich, aber keine Messung in Laborqualität.
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
1. Einleitung und Projektziel
2. Gruppenorganisation und Verantwortlichkeiten
3. Systemarchitektur
4. Hardware- und Sensoraufbau
5. Firmware- und Generator-Konfiguration
6. TTN-Cloud-Setup
7. Raspberry-Pi-Backend
8. Datenbank und Dashboard
9. Kalibrierung
10. Feldmessung
11. Ergebnisse
12. Grenzen
13. Verbesserungen
14. Fazit
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
Was haben wir gebaut?
Was haben wir gemessen?
Wie zuverlässig ist es?
Was haben wir gelernt?
Was würden wir verbessern?
```

Ein gutes Fazit verbindet technische Arbeit mit Systemverständnis. Es listet nicht nur ausgeführte Kommandos auf.

---

## Erfolgskriterium des Kurses

Das Kursprojekt ist erfolgreich, wenn die Gruppe einen vollständigen Datenpfad vom physischen Seewasser bis zum Browser-Dashboard demonstrieren und erklären kann, inklusive der Grenzen der Messung und der Verantwortlichkeiten jeder Systemschicht.
