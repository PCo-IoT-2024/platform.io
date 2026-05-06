# 06 — Analoge Kalibrierung

Analoge Sensoren werden nicht direkt verwendet. Der ESP32 liest einen rohen ADC-Wert, und die Firmware wandelt diesen Rohwert in einen kalibrierten Messwert um.

In diesem Projekt sind alle analogen Kalibrierwerte im Generator rohe ESP32-ADC-Werte.

Sie sind keine Spannungen.

## Warum rohe ADC-Werte?

Der rohe ADC-Wert ist das, was die Firmware tatsächlich sieht. Er enthält bereits Effekte von:

- Versorgungsspannung des Sensorboards
- Ausgangsschaltung des Sensors
- ESP32-ADC-Verhalten
- gewähltem ADC-Pin
- Spannungsteilern
- RC-Filtern
- Kabellänge
- elektrischem Rauschen
- Toleranzen

Daher lautet der Kalibrierworkflow:

```text
bekannte Referenzbedingung -> raw_adc vom Serial Monitor ablesen -> raw_adc in den Generator eintragen
```

Damit muss nicht zuerst eine Spannung berechnet werden.

## Ausgabe im Kalibriermodus

Die Firmware verwendet für alle analogen Kalibriermodi dasselbe serielle Format:

```text
[CAL] sensor=<name>, raw_adc=<adc>, calibrated_<unit>=<value>
```

Beispiele:

```text
[CAL] sensor=ph, raw_adc=2080.00, calibrated_ph=7.000
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
[CAL] sensor=turbidity, raw_adc=1200.00, calibrated_ntu=600.000
[CAL] sensor=ph_board_temperature, raw_adc=302.00, calibrated_temperature_c=26.000
```

Die ersten Messwerte nach dem Einschalten können driften. Verwenden Sie stabile Werte.

## pH-Kalibrierung

pH verwendet drei Referenzpunkte:

| Feld | Referenz |
|---|---|
| pH 4 ADC | pH-4-Pufferlösung |
| pH 7 ADC | pH-7-Pufferlösung |
| pH 10 ADC | pH-10-Pufferlösung |

Die Firmware verwendet diese Punkte für einen quadratischen Fit.

Empfohlenes Vorgehen:

1. pH-4-, pH-7- und pH-10-Pufferlösungen vorbereiten.
2. Die Sonde vor jeder Flüssigkeit spülen.
3. Die Sonde zuerst in pH 7 geben.
4. Warten, bis sich der Wert stabilisiert.
5. Rohe ADC-Werte aufzeichnen.
6. Für pH 4 und pH 10 wiederholen.
7. Werte in den Generator eintragen.
8. Eine neue `platformio.ini` erzeugen und flashen.

Nicht aggressiv mit der Glassonde rühren. Die Sondenmembran nicht mit Kraft trockenwischen.

## PH4502C-Boardtemperatur-Kalibrierung

Der PH4502C-Boardtemperaturkanal verwendet zwei Punkte:

| Feld | Aktueller Default |
|---|---:|
| Low temp ADC | 0 |
| Low temp °C | 0 |
| High temp ADC | 302 |
| High temp °C | 26 |

Die Firmware verwendet einen linearen Fit:

```text
temperature_c = a * raw_adc + b
```

Dieser Wert soll als Board-/Onboard-Temperatur interpretiert werden. Er ist nicht die Wassertemperatur.

Wenn der ADC-Wert nach dem Einschalten langsam fällt oder steigt, kann das normal sein. Board und analoge Schaltung können sich erwärmen oder stabilisieren. Warten Sie, bis sich der Wert nur noch langsam ändert, bevor Kalibrierwerte aufgezeichnet werden.

## TDS-Kalibrierung

TDS verwendet zwei Punkte:

| Feld | Bedeutung |
|---|---|
| Low TDS ADC | roher ADC in der niedrigen/Referenzlösung |
| Low TDS ppm | bekannter ppm-Wert der niedrigen/Referenzlösung |
| High TDS ADC | roher ADC in der hohen/Referenzlösung |
| High TDS ppm | bekannter ppm-Wert der hohen/Referenzlösung |

Die Firmware verwendet einen linearen Fit:

```text
tds_ppm = a * raw_adc + b
```

Die DS18B20-Wassertemperatur kann intern für Temperaturkompensation verwendet werden. Wenn keine gültige Wassertemperatur vorhanden ist, wird die Fallback-Temperatur aus dem Generator verwendet.

Nur `tds_ppm` wird per LoRaWAN gesendet.

## Trübungskalibrierung

Trübung verwendet zwei Punkte:

| Feld | Bedeutung |
|---|---|
| Clear water ADC | roher ADC in klarem Wasser |
| Clear water NTU | zugewiesener/Referenz-NTU-Wert für klares Wasser |
| Turbid water ADC | roher ADC in trüber Referenzprobe |
| Turbid water NTU | zugewiesener/Referenz-NTU-Wert für trübe Probe |

Die Firmware verwendet einen linearen Fit:

```text
turbidity_ntu = a * raw_adc + b
```

Die ADC-Richtung hängt vom Modul ab. Manche Module erzeugen niedrigere ADC-Werte bei höherer Trübung. Andere können sich anders verhalten. Tragen Sie die Werte genau so ein, wie sie gemessen wurden.

## Allgemeine Kalibrierregeln

- Stabile Messwerte verwenden.
- Dieselbe Versorgungsspannung während Kalibrierung und Normalmessung verwenden.
- Nach der Kalibrierung keine Spannungsteiler ändern.
- Nach der Kalibrierung keine ADC-Pins ändern.
- Analogleitungen kurz halten.
- Analoge Verdrahtung von Funkmodul und Schaltreglern fernhalten.
- Nach Änderungen der Kalibrierwerte neu generieren und neu flashen.

## Warum Kalibrierung in den Generator gehört

Kalibrierwerte sind in diesem Projekt Compile-Time-Konfiguration. Die generierte `platformio.ini` enthält die Werte, die von der Firmware verwendet werden.

Das bedeutet, eine Kalibrieränderung erfordert:

```text
Generatorwerte aktualisieren -> platformio.ini neu erzeugen -> Firmware flashen
```

Das ist einfach und sichtbar für die Lehre. Eine spätere fortgeschrittene Version könnte Kalibrierwerte im Flash speichern oder per Downlink empfangen, aber das ist nicht das Ziel dieses anfängerfreundlichen Workflows.
