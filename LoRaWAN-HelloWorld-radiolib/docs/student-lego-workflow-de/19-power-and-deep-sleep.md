# Stromversorgung, Deep Sleep und Solarbetrieb

**Primäre Workstreams:** Student:in 1 — Firmware und Deep Sleep, Student:in 2 — Sensorversorgung, gesamte Gruppe — Feldvorbereitung

Eine Boje soll ohne USB-Kabel laufen. Deshalb ist der Energieverbrauch wichtig.

Die Firmware ist für einen Low-Power-Betrieb strukturiert:

```text
aufwachen -> einen Sensor messen -> einen Uplink senden -> Deep Sleep
```

Die Hardware sollte dieselbe Idee unterstützen, indem externe Sensorplatinen abgeschaltet werden, wenn sie nicht benötigt werden.

## Warum Deep Sleep wichtig ist

Ein ESP32, der vollständig wach bleibt, verbraucht deutlich mehr Strom als ein ESP32 im Deep Sleep. Ein Solar-/Batterieknoten kann nur gut funktionieren, wenn er die meiste Zeit schläft.

Vereinfachtes Energiebild:

```text
mittlerer Strom = aktiver Strom * aktiver Zeitanteil + Schlafstrom * Schlafzeitanteil
```

Der aktive Strom kann hoch sein, besonders während einer LoRa-Übertragung oder beim GPS-Fix. Wenn die aktive Zeit kurz und der Schlafstrom niedrig ist, kann der mittlere Strom trotzdem akzeptabel bleiben.

## Aktuelles Sleep-Modell der Firmware

Die Firmware macht normalerweise Folgendes:

1. Aufwachen durch Timer
2. Boot-Zähler erhöhen
3. Reset-/Kalibrierknöpfe prüfen
4. LoRaWAN-Zustand wiederherstellen oder bei Bedarf joinen
5. einen Sensor lesen
6. einen Uplink senden
7. das Funkmodul schlafen legen
8. ESP32 Deep Sleep starten

Das Timer-Wakeup-Intervall wird aus dem Feld `Uplink interval` im Generator erzeugt.

## Ein Sensor pro Wake-up

Alle Sensoren bei jedem Wake-up zu messen, würde die aktive Zeit erhöhen. Der aktuelle Scheduler misst pro Wake-up einen Sensor und rotiert durch die aktivierten Sensoren.

Das hat mehrere Vorteile:

- kürzere Wachzeit
- kleinere Payloads
- geringerer Sensorstrom
- einfacheres fPort-Decoding
- niedrigerer mittlerer Energieverbrauch

Der Nachteil ist, dass jeder einzelne Sensor seltener aktualisiert wird. Wenn sechs Messkanäle aktiviert sind und das Uplink-Intervall 60 Sekunden beträgt, wird jeder Kanal ungefähr alle sechs Minuten aktualisiert.

## Externe Sensorversorgung

Viele Sensorplatinen haben keinen brauchbaren Software-Schlafmodus. Das gilt besonders für günstige analoge Boards.

Die praktische Lösung ist Power Gating:

```text
ESP32 GPIO -> Load-Switch oder MOSFET -> Sensor-Versorgungsschiene
```

Empfohlene Versorgungsrails:

| Rail | Lasten |
|---|---|
| `3V3_MAIN` | ESP32 und LoRa-Funkmodul |
| geschaltetes `3V3_SENS` | GPS und DS18B20 |
| geschaltetes `5V_SENS` | PH4502C, TDS, Trübung |

## Warum analoge Boards geschaltet werden sollten

PH4502C-, TDS- und Trübungsboards können dauerhaft Strom verbrauchen, wenn sie eingeschaltet bleiben. Sie können sich außerdem erwärmen und dadurch driften.

Für eine echte Boje sollten sie nur während der Messung versorgt werden. Nach dem Einschalten der Sensorversorgung sollte die Firmware lange genug warten, bis sich der analoge Ausgang stabilisiert hat.

## Solar- und Batteriekonzept

Ein praktisches Outdoor-Konzept ist:

```text
Solarpanel -> LiFePO4-Solarlader -> LiFePO4-Akku -> Regler -> ESP32 und Sensoren
```

Empfohlene Größenordnung:

- 1S-LiFePO4-Akku, zum Beispiel 32700-Zelle mit ungefähr 6000 mAh
- 6-V-Solarpanel, 5 W als Basis, 10 W für mehr Reserve
- LiFePO4-kompatibler Solarlader
- 3,3-V-Buck-Boost-Regler mit niedrigem Ruhestrom
- geschalteter 5-V-Boost-Konverter für analoge Sensorplatinen

## Warum LiFePO4

LiFePO4 ist für Outdoor-Lehrhardware attraktiv, weil diese Chemie vergleichsweise robust und thermisch stabil ist.

Vorteile:

- sicherere Chemie als viele LiPo-Packs
- lange Zyklenlebensdauer
- gut geeignet für Embedded-Systeme
- geeignet für wiederholte Lade-/Entladezyklen

Der Lader muss wirklich die LiFePO4-Ladespannung unterstützen. Verwenden Sie keinen normalen 4,2-V-LiPo-Lader für eine LiFePO4-Zelle.

## Strom messen

Studierende sollen messen, nicht raten.

Sinnvolle Messungen:

| Messung | Warum sie wichtig ist |
|---|---|
| ESP32-Deep-Sleep-Strom | bestimmt die Basisentladung der Batterie |
| aktiver Strom während Sensorlesen | zeigt die Kosten der Messung |
| LoRa-TX-Strom | zeigt die Funk-Burst-Kosten |
| GPS-Akquisitionszeit | dominiert oft die Energie |
| Off-Strom des Boost-Konverters | zeigt, ob die Sensorversorgung wirklich aus ist |

## Praktische Regel

Low Power entsteht nicht nur durch den Aufruf einer Sleep-Funktion. Das vollständige System muss Schlaf unterstützen:

```text
Firmware-Sleep + Radio-Sleep + Sensor-Power-Switching + Low-IQ-Regler + keine verschwenderischen Dev-Board-Teile
```

Ein normales ESP32-Development-Board kann hohen Schlafstrom haben, weil USB-UART-Chips, Power-LEDs und ineffiziente Regler weiter Strom verbrauchen. Für finale Bojen-Hardware ist ein eigenes Low-Power-Board besser.

## Fertig, wenn

```text
[ ] Die Gruppe versteht den Unterschied zwischen aktivem Strom und Schlafstrom.
[ ] Die Gruppe kann erklären, warum ein Sensor pro Wake-up sinnvoll ist.
[ ] Die Gruppe kennt das Grundprinzip geschalteter Sensorversorgung.
[ ] Die Gruppe kann erklären, warum LiFePO4 für dieses Lehrprojekt sinnvoll ist.
[ ] Die Gruppe weiß, welche Strommessungen für eine spätere Boje wichtig wären.
```
