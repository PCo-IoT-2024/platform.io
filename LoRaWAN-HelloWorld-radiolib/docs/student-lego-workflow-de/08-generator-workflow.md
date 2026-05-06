# 03 — Generator-Workflow

Der Generator ist das zentrale Werkzeug für diesen Branch. Er erzeugt die Firmware-Konfiguration und den passenden TTN Payload Formatter.

Öffnen Sie:

```text
LoRaWAN-HelloWorld-radiolib/tools/platformio-ini-generator/www/index.html
```

Der Generator läuft lokal im Browser. Es wird kein Server benötigt.

## Warum es den Generator gibt

Ohne den Generator müssten Studierende viele PlatformIO-Build-Flags manuell bearbeiten. Das ist fehleranfällig, weil folgende Dinge konsistent sein müssen:

```text
selected sensors
ESP32 pins
LoRaWAN version
TTN keys
calibration values
payload formatter fPorts
```

Der Generator reduziert diese Komplexität, indem er alle Entscheidungen auf einer Seite sammelt.

## Generierte Dateien

Der Generator erzeugt zwei Dateien:

| Datei | Verwendet von | Zweck |
|---|---|---|
| `platformio.ini` | PlatformIO / Firmware-Build | Compile-Time-Konfiguration |
| `payload-formatter.js` | TTN | Uplink-Dekodierung |

Diese Dateien müssen aus denselben Einstellungen erzeugt werden.

## Device and LoRaWAN card

Diese Karte konfiguriert:

- Environment-Name
- Funkmodul
- LoRaWAN-Version
- Region
- Uplink-Intervall

Der Environment-Name wird zum PlatformIO-Environment:

```ini
[env:water_buoy_sx1262]
```

Der Generator schreibt außerdem:

```ini
[platformio]
default_envs = water_buoy_sx1262
```

Daher können Studierende normalerweise bauen mit:

```bash
pio run
```

und hochladen mit:

```bash
pio run -t upload
```

## TTN credentials card

Die Credential-Karte enthält:

```text
DevEUI
AppKey
NwkKey
```

Der NwkKey ist nur für LoRaWAN 1.1.0 aktiviert. Für LoRaWAN 1.0.x wird das ältere Schlüsselmodell verwendet und das generierte INI gibt keinen separaten NwkKey aus.

Unterstützte Eingabeformate für Schlüssel:

| Format | Beschreibung |
|---|---|
| Direct hex | durchgehender Hex-String |
| MSB comma bytes | Byte-Liste in normaler Reihenfolge |
| LSB comma bytes | Byte-Liste in umgekehrter Byte-Reihenfolge |

Die generierten Firmware-Flags verwenden das Format, das vom C++-Code und RadioLib erwartet wird.

## Maintenance and calibration pins

Die Maintenance-Karte enthält Reset-Pins und analoge Kalibrierpins.

Reset-Pins:

| Pin | Zweck |
|---|---|
| Factory reset | gespeicherte Session löschen, Nonces erhalten |
| Dangerous nonce reset | Session und Nonces löschen |

Analoge Kalibrierpins:

| Pin | Modus |
|---|---|
| pH calibration pin | raw ADC und pH |
| pH board temp. calibration pin | raw ADC und Boardtemperatur |
| TDS calibration pin | raw ADC und ppm |
| Turbidity calibration pin | raw ADC und NTU |

Die elektrische Idee ist einfach:

```text
ESP32 GPIO ---- button ---- GND
```

Die Firmware verwendet interne Pull-ups. Wenn der Button während des Starts gedrückt ist, liest der Pin LOW.

## Sensor cards

Jede Sensorkarte enthält:

- Enable-Checkbox, wenn der Sensor optional ist
- ESP32-Pin-Felder
- Kalibrierwerte, falls nötig
- kurze Erklärung
- fPort-Badge

Der PH4502C-Boardtemperaturkanal ist derzeit verpflichtend. Er gehört zum PH4502C-Board und wird auf fPort 6 dekodiert.

## PlatformIO INI output

Die `platformio.ini`-Ausgabe ist beim Laden der Seite leer, bis auf den Hinweistext:

```text
Press “Generate platformio.ini”.
```

Das verhindert, dass Studierende veraltete generierte Inhalte verwenden.

Nach dem Drücken des Buttons kopieren oder laden Sie die generierte Datei herunter und legen Sie sie ab unter:

```text
LoRaWAN-HelloWorld-radiolib/platformio.ini
```

## Payload formatter output

Die Payload-Formatter-Ausgabe wird ebenfalls bei Bedarf erzeugt.

Sie enthält nur die ausgewählten optionalen Sensor-fPorts sowie den verpflichtenden fPort 6 und die Diagnose-fPorts.

Der aktuelle generierte Formatter enthält kein `payload_raw` für Measurement-Payloads.

## Regeneration rule

Regenerieren Sie beide Dateien, sobald sich eines davon ändert:

- ausgewählte Sensoren
- Pins
- LoRaWAN-Version
- Zugangsdaten
- Region
- Kalibrierwerte
- Uplink-Intervall

Eine gute Regel für Studierende ist:

```text
If you change anything in the generator, regenerate both files.
```
