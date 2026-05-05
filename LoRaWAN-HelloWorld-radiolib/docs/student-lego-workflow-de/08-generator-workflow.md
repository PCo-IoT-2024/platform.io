# Generator-Workflow

**Primäre Workstreams:** Student:in 1 — Firmware-Konfiguration, Student:in 2 — Sensoren und Kalibrierung, Student:in 3 — TTN-Daten

Der Generator ist das zentrale Werkzeug, mit dem die Gruppe aus ihren Entscheidungen eine konkrete Firmware-Konfiguration erzeugt.

Er erzeugt vor allem zwei Dateien:

```text
platformio.ini
payload-formatter.js
```

Diese beiden Dateien müssen zusammenpassen. Die `platformio.ini` konfiguriert die Firmware. Der Payload Formatter dekodiert später in TTN genau die Payloads, die diese Firmware sendet.

## Warum ein Generator?

Ohne Generator müssten Studierende viele Build-Flags, Pins, Kalibrierwerte und LoRaWAN-Parameter manuell schreiben. Das ist fehleranfällig.

Der Generator macht aus Eingaben wie:

```text
Sensorpins
Kalibrierwerte
LoRaWAN-Version
TTN-Schlüssel
fPort-Modell
```

konkrete Konfigurationsdateien.

## Typischer Ablauf

```text
1. TTN-Daten sammeln.
2. Sensorpins festlegen.
3. Kalibrierwerte erfassen.
4. Generator ausfüllen.
5. platformio.ini erzeugen.
6. payload-formatter.js erzeugen.
7. Firmware bauen und flashen.
8. Formatter in TTN einfügen.
9. TTN Live Data prüfen.
```

## Eingaben aus den Workstreams

| Eingabe | Kommt von |
|---|---|
| LoRaWAN-Version, DevEUI, JoinEUI, Keys | Student:in 3 |
| Sensorpins | Student:in 2 |
| Kalibrierwerte | Student:in 2 |
| gewünschte Firmware-Profile | Student:in 1 |
| fPort-Erwartung | gesamte Gruppe |

## Analoge Kalibrierwerte

Für analoge Sensoren verwendet der Generator rohe ESP32-ADC-Werte.

Wichtig:

```text
Keine Spannungen eintragen.
Keine berechneten Millivolt eintragen.
Nur rohe ADC-Werte aus dem Kalibriermodus verwenden.
```

Das gilt für:

- pH
- TDS
- Trübung
- PH4502C-Boardtemperatur

## Wartungs- und Kalibrierpins

Der Generator enthält Pins für Wartung und Kalibrierung.

Dazu gehören:

```text
APP_FACTORY_RESET_PIN
APP_DANGEROUS_NONCE_RESET_PIN
pH calibration pin
TDS calibration pin
Turbidity calibration pin
pH board temperature calibration pin
```

Wenn beim Start ein Kalibrierpin aktiv ist, startet die Firmware den Kalibriermodus für den entsprechenden Sensor. Im Kalibriermodus werden fortlaufend rohe ADC-Werte und der berechnete Sensorwert auf der seriellen Konsole ausgegeben.

## Ausgaben des Generators

### platformio.ini

Die `platformio.ini` enthält Build-Flags für die Firmware.

Beispiele für Inhalte:

```text
LoRaWAN-Version
Region
Sensorpins
Kalibrierwerte
fPort-Konfiguration
Debug-Einstellungen
```

### payload-formatter.js

Der Payload Formatter wird in TTN eingefügt. Er muss die fPorts 1 bis 6 korrekt dekodieren.

## Konsistenzprüfung

Nach dem Erzeugen der Dateien prüfen:

```text
[ ] LoRaWAN-Version im Generator passt zu TTN.
[ ] Region/Frequency Plan passt zu TTN.
[ ] fPort 1 ist GPS.
[ ] fPort 2 ist Wassertemperatur.
[ ] fPort 3 ist pH.
[ ] fPort 4 ist TDS.
[ ] fPort 5 ist Trübung.
[ ] fPort 6 ist PH4502C-Boardtemperatur.
[ ] Kalibrierwerte sind rohe ADC-Werte.
[ ] payload-formatter.js enthält keine Syntaxfehler.
```

## Häufige Fehler

| Fehler | Auswirkung |
|---|---|
| falscher Pin | Sensorwert bleibt konstant oder unsinnig |
| falsche LoRaWAN-Version | Join schlägt fehl |
| falscher Payload Formatter | TTN Decode-Fehler |
| Spannung statt ADC-Wert | Kalibrierung falsch |
| alter Formatter in TTN | TTN zeigt falsche Felder |

## Fertig, wenn

```text
[ ] platformio.ini ist erzeugt und im Projekt abgelegt.
[ ] payload-formatter.js ist erzeugt und in TTN eingetragen.
[ ] Firmware baut erfolgreich.
[ ] TTN dekodiert die erwarteten Felder.
```
