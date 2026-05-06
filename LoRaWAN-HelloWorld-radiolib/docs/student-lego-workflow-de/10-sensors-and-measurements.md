# 05 — Sensoren und Messtheorie

Dieses Kapitel erklärt, was die Sensoren messen und wie ihre Werte interpretiert werden sollten.

Die wichtige Lektion ist, dass Sensoren nicht direkt Wahrheit erzeugen. Sie erzeugen elektrische Signale. Firmware wandelt diese Signale in Zahlen um, und diese Zahlen müssen sorgfältig interpretiert werden.

## GPS

GPS liefert Positionsdaten.

Die Firmware sendet GPS auf fPort 1:

```text
latitude, longitude, altitude, hdop
```

Dekodierte TTN-Felder:

```text
latitude
longitude
altitude
hdop
```

HDOP bedeutet horizontal dilution of precision. Niedrigere HDOP-Werte zeigen normalerweise eine bessere horizontale Positionsschätzung an. Ein hoher HDOP-Wert bedeutet, dass die Position mit Vorsicht behandelt werden sollte.

GPS-Probleme sind in Innenräumen häufig. Wenn das GPS-Modul keinen Fix hat, sendet die Firmware eine Info-Nachricht auf fPort 221.

## DS18B20 water temperature

Der DS18B20 ist ein digitaler Temperatursensor. Er wird als Wassertemperatursensor verwendet.

Die Firmware sendet ihn auf fPort 2 als:

```text
temperature_c
```

Der DS18B20 ist normalerweise einfacher zu handhaben als analoge Sensoren, weil die Messung bereits digital ist. Trotzdem ist die Verdrahtung wichtig. Die OneWire-Datenleitung benötigt einen Pull-up-Widerstand, typischerweise 4,7 kΩ.

## pH

pH beschreibt, wie sauer oder alkalisch eine Flüssigkeit ist.

Vereinfachte Interpretation:

| pH | Bedeutung |
|---:|---|
| unter 7 | sauer |
| 7 | neutral |
| über 7 | alkalisch / basisch |

Das PH4502C-Board wandelt das Signal der pH-Sonde in eine analoge Spannung um. Der ESP32 liest diese Spannung als rohen ADC-Wert. Die Firmware wandelt den rohen ADC-Wert dann mithilfe von Kalibrierwerten in pH um.

Die Firmware sendet pH auf fPort 3 als:

```text
ph_level
```

pH-Sonden benötigen Kalibrierflüssigkeiten und sorgfältige Behandlung. Eine trockene, alte, verschmutzte oder schlecht gelagerte Sonde kann unzuverlässige Ergebnisse liefern.

## PH4502C board temperature

Das PH4502C-Board stellt außerdem einen temperaturbezogenen analogen Ausgang auf T1 bereit. In diesem Projekt wird er als PH4502C-Board-/Onboard-Temperatur verwendet.

Er wird auf fPort 6 gesendet als:

```text
ph_board_temperature_c
```

Das ist nicht die Wassertemperatur. Es ist ein Diagnosewert für die Elektronik rund um das PH4502C-Board. Er kann durch Sonnenlicht, Gehäuseerwärmung, Selbsterwärmung, Luftbewegung und Boardplatzierung beeinflusst werden.

Verwenden Sie den DS18B20 für die Wassertemperatur.

## TDS

TDS bedeutet Total Dissolved Solids. Ein TDS-Sensor schätzt gelöste Ionen und Feststoffe indirekt aus der elektrischen Leitfähigkeit.

Die Firmware sendet TDS auf fPort 4 als:

```text
tds_ppm
```

TDS wird oft in ppm angegeben. Der Wert ist nützlich für Trendbeobachtung, aber keine vollständige chemische Analyse. Zwei unterschiedliche Flüssigkeiten können denselben TDS-Wert haben, aber eine sehr unterschiedliche chemische Zusammensetzung.

TDS hängt von der Temperatur ab. Die Firmware kann intern die letzte gültige DS18B20-Wassertemperatur für die Kompensation verwenden. Die Kompensationstemperatur wird nicht gemeinsam mit dem TDS-Uplink gesendet.

## Turbidity

Turbidity beschreibt, wie trüb das Wasser ist. Günstige Trübungsmodule schätzen das optisch. Schwebstoffe beeinflussen, wie viel Licht den Detektor erreicht.

Die Firmware sendet Trübung auf fPort 5 als:

```text
turbidity_ntu
```

NTU bedeutet Nephelometric Turbidity Unit. In diesem Kurs-Setup sind der günstige Sensor und die einfache lineare Kalibrierung für Trends und Experimente geeignet, nicht für Labor-zertifizierte Trübungsmessung.

## Measurement limitations

Die folgenden Punkte sollten in Studierendenberichten diskutiert werden:

- Kalibrierzustand des Sensors
- Alterung und Verschmutzung der Sonde
- Temperatureffekte
- analoges Rauschen
- Stabilität der Versorgungsspannung
- Qualität der Wasserdichtung
- mechanische Platzierung im Wasser
- ob der Wert absolut oder hauptsächlich als Trend nützlich ist

## Warum mehrere Sensoren nützlich sind

Ein einzelner Sensorwert allein erklärt ein Wassersystem selten. Die Kombination ist nützlicher:

| Kombination | Interpretationsbeispiel |
|---|---|
| GPS + turbidity | wo trübes Wasser beobachtet wurde |
| temperature + TDS | leitfähigkeitsbezogene Änderungen mit Temperaturkontext |
| pH + temperature | chemischer Zustand mit Umweltkontext |
| turbidity + GPS | mögliche Algen, Schlamm, Störung oder Schwebstoffe |

Das Dashboard oder die spätere Datenbank sollte Zeitstempel, Ort und fPort-spezifische Werte zusammenhalten.
