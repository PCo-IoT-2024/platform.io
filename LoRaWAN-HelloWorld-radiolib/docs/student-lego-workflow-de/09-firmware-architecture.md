# Firmware-Architektur

**Primärer Workstream:** Student:in 1 — ESP32-Firmware und LoRaWAN-Gerät

Dieses Kapitel erklärt, wie die Firmware grundsätzlich aufgebaut ist und warum sie in einzelnen, wiederholbaren Schritten arbeitet.

Die Firmware ist nicht nur ein einzelnes Programm, das Sensoren liest. Sie muss Energie sparen, LoRaWAN-Sessions verwalten, Kalibriermodi erkennen, Messwerte auswählen, Payloads erzeugen und den ESP32 wieder schlafen legen.

## Grundidee

Die Firmware arbeitet zyklisch:

```text
Start oder Wake-up
  -> Hardware initialisieren
  -> LoRaWAN Session wiederherstellen oder joinen
  -> Wartungs-/Kalibrierpins prüfen
  -> genau einen Messkanal auswählen
  -> Messwert erfassen
  -> Payload bauen
  -> Uplink senden
  -> Status speichern
  -> schlafen gehen
```

Diese Struktur ist für Batteriebetrieb wichtig. Der ESP32 soll nicht dauerhaft aktiv sein.

## Warum nur ein Sensor pro Wake-up?

LoRaWAN ist für kleine Datenpakete gedacht. Außerdem braucht jeder Sensor Zeit und Energie.

Deshalb sendet die Firmware pro Wake-up nur einen Messkanal. Die Kanäle rotieren über mehrere Wake-ups.

Vorteile:

- kleinere Payloads
- kürzere aktive Zeit
- einfachere fPort-Zuordnung
- klarere TTN-Dekodierung
- weniger Energieverbrauch

## fPort-Rotation

Die Firmware verwendet die fPorts als Messkanal-Kennungen.

| fPort | Kanal |
|---:|---|
| 1 | GPS |
| 2 | Wassertemperatur |
| 3 | pH |
| 4 | TDS |
| 5 | Trübung |
| 6 | PH4502C-Boardtemperatur |

Beispielzyklus:

```text
Wake 1 -> fPort 1
Wake 2 -> fPort 2
Wake 3 -> fPort 3
Wake 4 -> fPort 4
Wake 5 -> fPort 5
Wake 6 -> fPort 6
Wake 7 -> fPort 1
```

## LoRaWAN Session

Ein LoRaWAN-Gerät muss im Netzwerk aktiviert sein. Das kann durch einen neuen Join oder durch Wiederherstellen einer gespeicherten Session passieren.

Die Firmware versucht, unnötige Joins zu vermeiden. Das spart Zeit und Energie.

Typische Zustände:

```text
keine Session vorhanden -> joinen
Session vorhanden -> wiederherstellen
Join fehlgeschlagen -> später erneut versuchen
```

## Kalibriermodus

Beim Start prüft die Firmware bestimmte Pins. Wenn ein Kalibrierpin aktiv ist, startet die Firmware nicht den normalen Mess-/Sendezyklus, sondern einen Kalibriermodus.

Im Kalibriermodus:

```text
Sensor wiederholt lesen
rohen ADC-Wert ausgeben
berechneten Sensorwert ausgeben
auf serielle Konsole schreiben
bei Loslassen des Buttons neu starten
```

Das ist wichtig, weil die Studierenden die rohen ADC-Werte für den Generator brauchen.

## Wartungspins

Neben Kalibrierpins gibt es Wartungspins, zum Beispiel:

```text
APP_FACTORY_RESET_PIN
APP_DANGEROUS_NONCE_RESET_PIN
```

Diese Pins sind gefährlicher als Kalibrierpins, weil sie gespeicherte Zustände zurücksetzen können. Sie sollten bewusst und dokumentiert verwendet werden.

## Deep Sleep

Deep Sleep ist ein Energiesparzustand des ESP32. Während Deep Sleep läuft die normale Firmware nicht. Nach dem Aufwachen startet der ESP32 weitgehend neu, kann aber bestimmte Daten aus RTC- oder persistentem Speicher wieder verwenden.

Für die Boje bedeutet das:

```text
kurz aktiv sein
messen
senden
schlafen
```

Nicht:

```text
dauerhaft wach bleiben
```

## Serielle Ausgabe

Die serielle Ausgabe ist während Entwicklung und Fehlersuche entscheidend.

Typische Informationen:

- Wake-up-Grund
- Boot-Zähler
- LoRaWAN Join oder Restore
- ausgewählter fPort
- Sensorstatus
- Uplink-Ergebnis
- Kalibrierdaten

Studierende sollen Logs sammeln, weil sie im Abschlussbericht helfen, den Systempfad nachzuweisen.

## Fertig, wenn

```text
[ ] Die Gruppe kann den Firmware-Zyklus erklären.
[ ] Die Gruppe versteht die fPort-Rotation.
[ ] Die Gruppe versteht den Unterschied zwischen normalem Modus und Kalibriermodus.
[ ] Die Gruppe kann erklären, warum Deep Sleep verwendet wird.
[ ] Die serielle Ausgabe wird zur Fehlersuche genutzt.
```
