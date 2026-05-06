# 08 — Bauen, Flashen, Testen und Fehlersuche

Dieses Kapitel gibt den praktischen Kommandozeilen-Workflow und typische Debugging-Schritte.

Alle Kommandos werden ausgeführt aus:

```text
LoRaWAN-HelloWorld-radiolib/
```

## Build

Nachdem `platformio.ini` erzeugt und in das Projektverzeichnis kopiert wurde, bauen Sie mit:

```bash
pio run
```

Die generierte Datei setzt `default_envs`, daher ist normalerweise kein `-e` erforderlich.

Wenn Sie das Environment explizit angeben wollen:

```bash
pio run -e <environment-name>
```

## Upload

Upload auf den ESP32:

```bash
pio run -t upload
```

Explizite Environment-Variante:

```bash
pio run -e <environment-name> -t upload
```

## Serial monitor

Monitor öffnen:

```bash
pio device monitor
```

Nützliche Dinge zum Prüfen:

- Boot-Grund
- Boot Count
- ausgewählte Sensorkonfiguration
- ausgewählter Sensorindex
- ausgewählter Sensorname
- LoRaWAN Join oder Session Restore
- fPort
- Diagnosemeldungen
- Kalibrierausgabe

## Expected startup information

Typische serielle Ausgabe enthält einen Sensor-Konfigurationsblock:

```text
[APP] Sensor configuration:
[APP]   GPS: 1
[APP]   temperature: 1
[APP]   pH: 1
[APP]   PH4502C board temperature: 1
[APP]   TDS: 1
[APP]   turbidity: 1
```

Die genauen Werte hängen von der generierten Konfiguration ab.

## TTN live data check

In TTN Live Data prüfen:

- Join accepted
- Uplinks kommen an
- richtiger fPort
- decoded payload vorhanden
- kein JavaScript-Formatter-Fehler
- keine unerwarteten dekodierten Felder

Erwartete dekodierte Beispiele:

TDS:

```json
{
  "tds_ppm": 350.0
}
```

pH:

```json
{
  "ph_level": 7.12
}
```

PH4502C board temperature:

```json
{
  "ph_board_temperature_c": 26.0
}
```

## Common problem: formatter syntax error

Symptom in TTN:

```text
as.up.data.decode.fail
SyntaxError
```

Fix:

1. Generator-Seite neu laden.
2. **Generate payload formatter** drücken.
3. Den vollständigen generierten Formatter kopieren.
4. Den JavaScript Formatter in TTN ersetzen.
5. Speichern und auf den nächsten Uplink warten.

## Common problem: wrong decoded fields

Symptom:

TTN dekodiert ein Feld, das nicht mehr existieren sollte, zum Beispiel `payload_raw` oder TDS-Temperatur.

Ursache:

Der installierte Formatter ist veraltet.

Fix:

`payload-formatter.js` neu generieren und neu installieren.

## Common problem: GPS waits forever

Symptom:

```text
RadioLib experiment device: Waiting for GPS
```

Mögliche Ursachen:

- GPS hat keine Sicht zum Himmel
- Test findet innen statt
- RX/TX sind vertauscht
- GPS-Modul ist nicht versorgt
- falsche Baudrate oder Pins

Fix:

- draußen oder nahe an einem Fenster testen
- GPS TX zu ESP32 RX prüfen
- GPS RX zu ESP32 TX prüfen
- Versorgungsspannung prüfen
- länger auf ersten Fix warten

## Common problem: pH unrealistic

Mögliche Ursachen:

- fehlende Kalibrierung
- Sonde nicht stabil
- falscher Analogpin
- Spannungsteiler geändert
- pH-Board-Potentiometer geändert
- Sonde trocken, verschmutzt, alt oder schlecht gelagert

Fix:

pH-4-, pH-7- und pH-10-Pufferlösungen verwenden und rohe ADC-Werte im Generator aktualisieren.

## Common problem: TDS zero or too low

Mögliche Ursachen:

- Sensor nicht versorgt
- falscher ADC-Pin
- schlechte Kalibrierpunkte
- Sonde nicht in Flüssigkeit
- Fallback-Temperatur unrealistisch

Fix:

TDS-Kalibriermodus verwenden und rohe ADC-Werte für bekannte Referenzlösungen aufzeichnen.

## Common problem: turbidity inverted

Manche Trübungsboards erzeugen niedrigere ADC-Werte für trüberes Wasser. Andere können sich unterscheiden.

Die Firmware kann beide Richtungen behandeln, solange die Kalibrierpunkte so eingetragen werden, wie sie gemessen wurden.

## Common problem: join failure after nonce reset

LoRaWAN-Nonces dürfen nicht wiederverwendet werden. Wenn das Gerät lokale Nonces löscht, TTN aber alte Nonces noch kennt, kann der OTAA Join fehlschlagen.

Fix:

- Factory Reset für normalen Session Reset verwenden
- Dangerous Nonce Reset nur mit TTN Nonce Reset oder neuer DevEUI verwenden

## Debugging discipline

In Schichten debuggen:

```text
1. Does the firmware build?
2. Does upload work?
3. Does serial output start?
4. Does LoRaWAN join or restore session?
5. Does the selected sensor read something plausible?
6. Does TTN receive the fPort?
7. Does TTN decode the payload correctly?
```

Nicht fünf Dinge gleichzeitig ändern. Eine Sache ändern, testen, dann weitermachen.
