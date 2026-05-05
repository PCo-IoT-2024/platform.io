# Systemüberblick

**Primäre Workstreams:** Student:in 1 — ESP32-Firmware, Student:in 2 — Sensoren, Student:in 3 — TTN-Cloud-Setup

**Schnittstellen:** fPort-Zuordnung, dekodierte Feldnamen, Pin-Mapping, Kalibrierwerte

Dieses Kapitel beschreibt den konkreten Bojenknoten und sein unmittelbares Datenmodell. Die vollständige übergreifende Systemarchitektur wurde bereits im Kapitel zur Ende-zu-Ende-Architektur erklärt. Hier geht es enger um die Frage, was die ESP32-Firmware misst, wie Messungen eingeplant werden, welche fPorts verwendet werden und welche Felder später in TTN, MQTT, MariaDB und Dashboard konsistent bleiben müssen.

Der Bojenknoten ist ein verteiltes Messsystem. Er beginnt mit physikalischen Größen im Wasser und erzeugt dekodierte Messfelder in TTN.

```text
water / environment
  -> sensor
  -> ESP32 pin or digital bus
  -> firmware measurement code
  -> calibration model
  -> LoRaWAN uplink
  -> TTN payload formatter
  -> decoded field
```

Jede Stufe kann Fehler verursachen. Ein falscher Pin, ein falscher Kalibrierwert, ein falscher LoRaWAN-Schlüssel, ein falscher fPort oder ein veralteter Payload Formatter kann das gesamte System scheinbar kaputt machen.

## Hauptblöcke auf Geräteseite

| Block | Rolle |
|---|---|
| ESP32 | Mikrocontroller, auf dem die Firmware läuft |
| LoRa-Funkmodul | Funkverbindung zum LoRaWAN-Gateway |
| GPS | Position der Boje |
| DS18B20 | Wassertemperatur |
| PH4502C pH-Kanal | pH-Wert |
| PH4502C-Boardtemperaturkanal | Onboard-/Lufttemperaturwert |
| Gravity TDS | Schätzwert für gelöste Stoffe |
| Trübungssensor | Schätzwert für Wassertrübung |
| Akku-/Solarsystem | Energieversorgung im Außenbetrieb |

Die Backend-Blöcke werden hier nicht im Detail wiederholt. Sie werden in den Kapiteln zu Raspberry Pi, MQTT, MariaDB und Dashboard behandelt.

## Laufzeitmodell der Firmware

Die Firmware folgt normalerweise diesem Zyklus:

```text
wake up
  -> initialize hardware
  -> restore LoRaWAN session or join
  -> check calibration/reset buttons
  -> read one selected sensor
  -> apply calibration if needed
  -> send one uplink on one fPort
  -> put radio to sleep
  -> enter ESP32 deep sleep
```

Pro Aufwachen wird nur ein Messkanal gesendet. Die Firmware rotiert durch die aktivierten Messkanäle. Das hält Payloads klein und reduziert die aktive Zeit.

Beispiel mit allen aktuellen Messkanälen:

```text
wake 1 -> fPort 1 -> GPS
wake 2 -> fPort 2 -> water temperature
wake 3 -> fPort 3 -> pH
wake 4 -> fPort 4 -> TDS
wake 5 -> fPort 5 -> turbidity
wake 6 -> fPort 6 -> PH4502C board temperature
wake 7 -> fPort 1 -> GPS again
```

## fPort-Vertrag

Der fPort-Vertrag ist eine der wichtigsten Schnittstellen im gesamten Projekt.

| fPort | Messung | Dekodiertes Feld |
|---:|---|---|
| 1 | GPS-Position | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | DS18B20-Wassertemperatur | `temperature_c` |
| 3 | PH4502C pH | `ph_level` |
| 4 | Gravity TDS | `tds_ppm` |
| 5 | Trübung | `turbidity_ntu` |
| 6 | PH4502C-Boardtemperatur | `ph_board_temperature_c` |

Dieselbe Bedeutung muss in allen Schichten gelten:

```text
ESP32 firmware
TTN payload formatter
TTN live data
local MQTT messages
MariaDB f_port column
dashboard interpretation
final report
```

## Payload-Philosophie

Die aktuelle Firmware sendet für jeden Mess-fPort nur den endgültigen Messwert.

Sie sendet keine zusätzlichen rohen ADC-Werte, Zwischen-Spannungen oder die pH-Boardtemperatur gemeinsam mit dem pH-Wert.

Beispiele:

```text
fPort 3 -> only ph_level
fPort 4 -> only tds_ppm
fPort 5 -> only turbidity_ntu
fPort 6 -> only ph_board_temperature_c
```

Die serielle Ausgabe im Kalibriermodus ist etwas anderes. Während der Kalibrierung gibt der ESP32 rohe ADC-Werte auf der seriellen Konsole aus, damit Studierende Kalibrierwerte in den Generator eintragen können. Diese Rohwerte sind nicht Teil des normalen LoRaWAN-Messpayloads.

## Fertig, wenn

Dieses Kapitel ist verstanden, wenn die Gruppe erklären kann:

```text
[ ] welcher Sensor welchen fPort verwendet
[ ] warum pro Aufwachen nur eine Messung gesendet wird
[ ] warum rohe ADC-Werte für Kalibrierung verwendet, aber nicht normal gesendet werden
[ ] warum Firmware, Formatter, Datenbank und Dashboard dieselben Feldbedeutungen verwenden müssen
```
