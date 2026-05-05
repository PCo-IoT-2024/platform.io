# Analoge Kalibrierung

**Primärer Workstream:** Student:in 2 — Sensoren und Kalibrierung

Dieses Kapitel erklärt die Kalibrierung der analogen Sensoren. Im Projekt werden für analoge Kalibrierpunkte ausschließlich rohe ESP32-ADC-Werte verwendet.

Wichtig:

```text
Keine Spannungen.
Keine Millivolt.
Keine theoretischen ADC-Maximalwerte.
Nur rohe ADC-Werte aus der Firmware-Kalibrierausgabe.
```

## Warum Kalibrierung nötig ist

Analoge Sensoren liefern eine Spannung beziehungsweise ein elektrisches Signal. Der ESP32 wandelt dieses Signal mit seinem ADC in einen Rohwert um.

Dieser Rohwert hängt ab von:

- Sensor
- Versorgungsspannung
- Modulschaltung
- ESP32-ADC-Eigenschaften
- Verkabelung
- Temperatur
- Kalibrierzustand

Deshalb kann man nicht einfach annehmen, dass ein ADC-Rohwert automatisch einen korrekten pH-, TDS- oder Trübungswert bedeutet.

## Kalibriermodus

Die Firmware besitzt Kalibriermodi für analoge Sensoren.

Wenn beim Start der passende Kalibrierpin aktiv ist, startet die Firmware den Kalibriermodus für diesen Sensor.

Währenddessen gibt die Firmware fortlaufend Werte aus:

```text
[CAL] sensor=<name> raw=<adc> value=<calibrated-value>
```

Der wichtigste Wert für den Generator ist `raw`.

## pH-Kalibrierung

Für pH werden Referenzlösungen verwendet.

Typische Referenzen:

```text
pH 4
pH 7
pH 10
```

Vorgehen:

1. Sonde reinigen.
2. Sonde in Referenzlösung geben.
3. Stabilisierung abwarten.
4. Kalibriermodus starten.
5. rohen ADC-Wert notieren.
6. Referenz-pH-Wert notieren.
7. Für weitere Referenzlösungen wiederholen.
8. Werte in den Generator eintragen.

Die pH-Kalibrierung kann mehrpunktig sein. Wichtig ist, dass die eingetragenen ADC-Werte wirklich aus derselben Hardwarekonfiguration stammen.

## TDS-Kalibrierung

TDS kann mit Referenzflüssigkeiten kalibriert werden. Falls keine Referenz verfügbar ist, kann man zumindest Plausibilitätsmessungen durchführen.

Im Projekt wird eine lineare Kalibrierung verwendet:

```text
raw ADC -> tds_ppm
```

Dazu werden mindestens zwei Punkte benötigt:

```text
niedriger ADC-Wert + bekannter TDS-Wert
hoher ADC-Wert + bekannter TDS-Wert
```

Für genaue Messungen sind echte TDS-Referenzlösungen sinnvoll. Ohne Referenzlösung ist TDS eher als Trendwert zu verstehen.

## Trübungskalibrierung

Auch die Trübung wird im Projekt linear aus ADC-Werten berechnet.

```text
raw ADC -> turbidity_ntu
```

Auch hier sind mindestens zwei Referenzpunkte sinnvoll.

Ohne NTU-Referenzlösung kann man nur eine einfache Plausibilitätskalibrierung machen, zum Beispiel mit klarem Wasser und einer sichtbar getrübten Probe. Das ist für Trends brauchbar, aber nicht laborpräzise.

## PH4502C-Boardtemperatur

Der Boardtemperaturkanal des PH4502C wird ebenfalls analog gelesen und kalibriert.

Standardwerte im Generator:

```text
Low temp ADC: 0
Low temp °C: 0
High temp ADC: 302
High temp °C: 26
```

Diese Werte bilden eine einfache lineare Zuordnung. Wenn die Gruppe bessere Referenzpunkte misst, können diese im Generator ersetzt werden.

## Kalibriertabelle

Jede Gruppe soll eine Kalibriertabelle führen.

Beispiel:

| Sensor | Referenz | roher ADC-Wert | berechneter Wert | Notiz |
|---|---:|---:|---:|---|
| pH | 7.00 | 1234 | 7.01 | stabil nach 60 s |
| TDS | 342 ppm | 1850 | 340 | Referenzlösung |
| Trübung | 0 NTU | 2900 | 0 | klares Wasser |
| Boardtemp | 26 °C | 302 | 26 | Raumtemperatur |

## Häufige Fehler

| Fehler | Auswirkung |
|---|---|
| Spannung statt ADC-Wert eingetragen | Kalibrierung völlig falsch |
| Sensor nicht stabilisiert | schwankende Kalibrierpunkte |
| falscher Pin | keine sinnvollen Werte |
| Referenzlösung verschmutzt | falsche Referenz |
| pH-Sonde nicht gespült | Verschleppung zwischen Lösungen |

## Fertig, wenn

```text
[ ] Alle analogen Sensoren können in den Kalibriermodus gebracht werden.
[ ] Rohwerte werden seriell angezeigt.
[ ] Kalibrierwerte sind dokumentiert.
[ ] Generator verwendet rohe ADC-Werte.
[ ] Normale Messwerte nach dem Flashen sind plausibel.
```
