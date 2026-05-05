# Kurskontext und Lernziele

Dieses Handbuch begleitet einen kompakten Workshop, in dem Studierendengruppen eine LoRaWAN-basierte Wasserqualitätsboje aufbauen. Das Ziel ist nicht nur, einzelne Sensoren an einen ESP32 anzuschließen. Das Ziel ist, ein vollständiges kleines IoT-System zu verstehen: von der physikalischen Messung im Wasser bis zur Darstellung der Messwerte in einem Dashboard.

Der Kurs richtet sich an Studierende, die nicht notwendigerweise eine tiefe technische Vorbildung in Elektronik, Embedded Systems oder Netzwerktechnik haben. Deshalb erklärt dieses Handbuch nicht nur Arbeitsschritte, sondern auch die wichtigsten technischen Hintergründe.

## Was wird gebaut?

Die Gruppe baut ein System mit dieser Grundidee:

```text
Wasser / Umgebung
  -> Sensoren
  -> ESP32
  -> LoRaWAN
  -> The Things Network
  -> MQTT
  -> Raspberry Pi
  -> MariaDB
  -> Dashboard
```

Die Boje misst unter anderem:

- GPS-Position
- Wassertemperatur
- pH-Wert
- TDS-Wert
- Trübung
- PH4502C-Boardtemperatur

Diese Werte werden über LoRaWAN an TTN gesendet, dort dekodiert, über MQTT an einen Raspberry Pi weitergeleitet, in MariaDB gespeichert und in einem Dashboard angezeigt.

## Warum ist das ein gutes Kursprojekt?

Das Projekt verbindet mehrere Ebenen eines realen IoT-Systems:

| Ebene | Inhalt |
|---|---|
| Physik und Chemie | Was bedeuten pH, TDS, Trübung und Temperatur? |
| Elektronik | Wie werden Sensoren mit einem ESP32 verbunden? |
| Embedded Software | Wie liest Firmware Sensoren und sendet Daten? |
| Funknetz | Wie funktioniert LoRaWAN grundsätzlich? |
| Cloud | Welche Rolle spielt TTN? |
| Integration | Wie werden MQTT-Nachrichten weitergeleitet? |
| Datenhaltung | Warum braucht man eine Datenbank? |
| Visualisierung | Wie werden Messwerte verständlich dargestellt? |
| Projektarbeit | Wie arbeitet eine Gruppe an einem integrierten System? |

Für managementorientierte Studierende ist besonders wichtig: Das System zeigt, dass ein IoT-Projekt nicht nur aus Hardware besteht. Es ist ein Zusammenspiel aus Technik, Organisation, Schnittstellen, Datenqualität, Betrieb und Interpretation.

## Lernziele

Nach dem Workshop sollen Studierende erklären können:

```text
[ ] wie ein Messwert vom Sensor bis zum Dashboard gelangt
[ ] welche Rolle ESP32, TTN, MQTT, Raspberry Pi und MariaDB spielen
[ ] warum Kalibrierung für analoge Sensoren notwendig ist
[ ] warum fPorts und Payload Formatter exakt zusammenpassen müssen
[ ] wie Gruppenarbeit über Schnittstellen organisiert wird
[ ] welche Grenzen ein Prototyp im Vergleich zu einem Produkt hat
```

## Was ist ein gutes Ergebnis?

Ein gutes Ergebnis ist kein perfekt industriell gefertigtes Gerät. Ein gutes Ergebnis ist ein nachvollziehbarer, erklärbarer Prototyp.

Die Mindestanforderung lautet:

```text
Ein realer Sensorwert und eine GPS-Position werden vom ESP32 gemessen, in TTN dekodiert, zum Raspberry Pi gebridged, in MariaDB gespeichert und im Dashboard angezeigt.
```

Zusätzlich soll die Gruppe erklären können, wie zuverlässig die Messwerte sind, welche Fehlerquellen existieren und was in einer nächsten Version verbessert werden müsste.

## Was bleibt bewusst einfach?

Einige Dinge werden im Kurs bewusst vereinfacht:

- Die Datenbankstruktur ist einfach gehalten.
- Das Dashboard muss zuerst funktionieren, nicht perfekt aussehen.
- GPS wird in einer eigenen Tabelle gespeichert.
- Die Stromversorgung wird als Feldvorbereitung behandelt, nicht als vollständiges Produktdesign.
- Der Fokus liegt auf Verstehen, Integration und Nachvollziehbarkeit.

## Erwartete Haltung

Die wichtigste Arbeitsweise im Kurs ist schrittweises Testen.

Nicht so:

```text
alles zusammenbauen und hoffen, dass es funktioniert
```

Sondern so:

```text
einen Schritt bauen
prüfen
verstehen
belegen
erst dann den nächsten Schritt
```

Das ist die zentrale Methode für robuste technische Systeme.
