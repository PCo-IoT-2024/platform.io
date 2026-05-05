# Sensoren und Messtheorie

**Primärer Workstream:** Student:in 2 — Sensoren und Kalibrierung

Dieses Kapitel erklärt, was die verwendeten Sensoren messen und wie ihre Werte zu interpretieren sind. Ein Sensorwert ist nicht automatisch eine objektive Wahrheit. Er ist das Ergebnis eines Messsystems mit Stromversorgung, Sensor, Anschluss, Firmware, Kalibrierung und Umgebungseinflüssen.

## Verwendete Sensoren

| Sensor | Messgröße | fPort | Datenbank |
|---|---|---:|---|
| GPS | Position | 1 | `gps_positions` |
| DS18B20 | Wassertemperatur | 2 | `measurements` |
| PH4502C pH | pH-Wert | 3 | `measurements` |
| Gravity TDS | TDS ppm | 4 | `measurements` |
| Trübungssensor | Trübung NTU | 5 | `measurements` |
| PH4502C-Boardtemperatur | Board-/Lufttemperatur | 6 | `measurements` |

## GPS

GPS liefert Positionen als Koordinaten. Im Projekt werden verwendet:

```text
latitude
longitude
altitude
hdop
```

`latitude` und `longitude` beschreiben die Position. `altitude` ist die Höhe. `hdop` beschreibt die horizontale Genauigkeit beziehungsweise geometrische Qualität der Satellitenpositionierung. Je kleiner der HDOP-Wert, desto besser ist normalerweise die Positionsgeometrie.

GPS kann einige Zeit brauchen, bis ein gültiger Fix verfügbar ist. Im Gebäude oder nahe an Fenstern kann GPS unzuverlässig sein.

## DS18B20-Wassertemperatur

Der DS18B20 ist ein digitaler Temperatursensor. Er ist im Vergleich zu analogen Sensoren einfacher auszulesen, weil er bereits digitale Messwerte liefert.

Wichtig:

- Sensor muss wasserdicht ausgeführt sein.
- Messung braucht etwas Zeit.
- Sensor muss tatsächlich im Wasser sein.
- Temperatur kann sich langsam anpassen.

## pH-Wert

Der pH-Wert beschreibt, wie sauer oder basisch eine Lösung ist.

Typische Orientierung:

| pH | Bedeutung |
|---:|---|
| 7 | neutral |
| kleiner als 7 | sauer |
| größer als 7 | basisch |

pH-Messungen sind empfindlich. Die Sonde muss sauber sein und kalibriert werden. Dafür werden pH-Referenzlösungen verwendet.

Im Projekt wird der pH-Kanal analog gelesen und durch Kalibrierwerte korrigiert.

## TDS

TDS steht für Total Dissolved Solids. Der Wert ist ein Schätzwert für gelöste Stoffe im Wasser, typischerweise in ppm.

Wichtig: Ein einfacher TDS-Sensor misst nicht direkt alle Stoffe chemisch. Er nutzt elektrische Leitfähigkeit als Grundlage und rechnet daraus einen TDS-Schätzwert ab.

TDS ist nützlich als Trendwert, aber nicht dasselbe wie eine vollständige Laboranalyse.

## Trübung

Trübung beschreibt, wie stark Wasser Licht streut oder absorbiert. In vielen Systemen wird sie in NTU angegeben.

Ein einfacher Trübungssensor ist stark abhängig von:

- Lichtweg
- Verschmutzung des Sensors
- Luftblasen
- Ausrichtung
- Umgebungslicht
- Kalibrierung

Der Wert ist für einen Kursprototyp gut geeignet, um Unterschiede und Trends sichtbar zu machen.

## PH4502C-Boardtemperatur

Das PH4502C-Modul besitzt zusätzlich einen Temperaturkanal. Im Kurs wird dieser als eigener Messkanal auf fPort 6 gesendet.

Dieser Wert ist nicht die Wassertemperatur. Er beschreibt die Temperatur am Modul beziehungsweise in der Nähe des Boards.

Wassertemperatur kommt vom DS18B20 auf fPort 2.

## Analoge und digitale Sensoren

| Art | Beispiel | Konsequenz |
|---|---|---|
| digital | DS18B20, GPS | Sensor liefert bereits digitale Daten |
| analog | pH, TDS, Trübung, Boardtemperatur | ESP32 ADC liest Rohwert, Kalibrierung nötig |

Bei analogen Sensoren ist Kalibrierung besonders wichtig.

## Plausibilität statt blindem Vertrauen

Studierende sollen jeden Wert kritisch prüfen:

```text
Ist der Sensor angeschlossen?
Ist der Wert stabil?
Passt der Wert grob zur Umgebung?
Wurde der Sensor kalibriert?
Gibt es offensichtliche Störungen?
```

Ein plausibler Wert ist nicht automatisch exakt, aber ein unplausibler Wert ist ein Hinweis auf ein Problem.

## Fertig, wenn

```text
[ ] Die Gruppe kennt alle Sensoren und fPorts.
[ ] Die Gruppe kann erklären, welche Sensoren analog sind.
[ ] Die Gruppe kann erklären, warum Kalibrierung nötig ist.
[ ] Die Gruppe unterscheidet Wassertemperatur und Boardtemperatur.
[ ] Die Gruppe kann Grenzen von TDS und Trübung erklären.
```
