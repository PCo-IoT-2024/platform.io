# 01 — Technologie-Grundlagen für Manager:innen

Dieses Kapitel ist bewusst für Studierende geschrieben, die keine starke Vorbildung in Elektronik oder Embedded Systems haben. Ziel ist, ein mentales Modell aufzubauen, bevor Pins, Firmware und Kalibrierwerte betrachtet werden.

Eine Wasserüberwachungsboje ist nicht nur ein Gadget. Sie ist ein kleines cyber-physisches System. Sie beobachtet die physische Welt, wandelt Beobachtungen in elektrische Signale um, wandelt elektrische Signale in digitale Werte um, sendet diese Werte durch ein Kommunikationsnetzwerk und macht sie schließlich als Daten sichtbar.

Die vollständige Kette ist:

```text
water condition
  -> sensor
  -> electrical signal
  -> microcontroller reading
  -> firmware interpretation
  -> radio transmission
  -> network server
  -> application decoder
  -> data field in TTN
  -> human interpretation
```

Jeder Pfeil in dieser Kette ist eine mögliche Quelle für Fehler oder Unsicherheit.

---

## 1. Was ist ein Embedded System?

Ein Embedded System ist ein Computer, der in ein größeres Objekt oder einen Prozess eingebaut ist. Es sieht normalerweise nicht wie ein Laptop oder Telefon aus. Es hat keinen Bildschirm, keine Tastatur und keine normale Benutzeroberfläche.

In diesem Projekt ist das Embedded System der ESP32-basierte Bojenknoten.

Er hat drei Hauptaufgaben:

1. Sensoren steuern
2. Messwerte verarbeiten
3. Daten kommunizieren

Eine nützliche Analogie ist ein sehr kleiner Außendienstmitarbeiter:

```text
wake up
look at one instrument
write down one number
send the number to headquarters
go back to sleep
```

Genau das macht die Firmware.

---

## 2. Was ist ein Mikrocontroller?

Ein Mikrocontroller ist ein kleiner Computer auf einem Chip. Er enthält:

- Prozessor
- Speicher
- Ein-/Ausgabepins
- Timer
- Kommunikationsschnittstellen
- Analog-Digital-Wandler
- Sleep-Modi

Der ESP32 ist der Mikrocontroller in diesem Projekt.

Er ist nicht so leistungsfähig wie ein Laptop, aber er ist sehr gut in der direkten Interaktion mit Hardware. Er kann Pins lesen, mit Sensoren kommunizieren, ein Funkmodul steuern und in Deep Sleep gehen, um Energie zu sparen.

Auf Management-Ebene kann man ihn so verstehen:

```text
A microcontroller is the operational controller at the edge of the system.
```

Er trifft lokale Entscheidungen, bevor ein Cloud-Dienst die Daten sieht.

---

## 3. Was ist ein Sensor?

Ein Sensor wandelt eine physikalische oder chemische Eigenschaft in ein elektrisches Signal um.

Beispiele:

| Physikalische / chemische Eigenschaft | Sensorausgabe |
|---|---|
| Wassertemperatur | digitale Zahl vom DS18B20 |
| pH | analoge Spannung vom PH4502C-Board |
| TDS | analoge Spannung von der Leitfähigkeitsschaltung |
| Trübung | analoge Spannung von der optischen Schaltung |
| Position | serielle digitale Nachrichten vom GPS |

Ein Sensor sendet nie direkt „Wahrheit“. Er sendet ein Signal, das interpretiert werden muss.

Diese Interpretation hängt ab von:

- Kalibrierung
- Rauschen
- Sensorqualität
- Umgebung
- Verdrahtung
- Stromversorgung
- Firmware-Modell

Deshalb sind Messsysteme nicht nur technische Systeme, sondern auch Interpretationssysteme.

---

## 4. Digitale versus analoge Sensoren

Manche Sensoren geben digitale Daten aus. Andere geben analoge Spannungen aus.

### Digitaler Sensor

Ein digitaler Sensor kommuniziert bereits Zahlen.

Beispiel:

```text
DS18B20 -> 21.75 °C
GPS -> latitude and longitude
```

Der ESP32 liest den Wert über ein digitales Protokoll.

### Analoger Sensor

Ein analoger Sensor gibt eine Spannung aus. Der ESP32 muss diese Spannung in eine Zahl umwandeln.

Beispiel:

```text
PH4502C output voltage -> ESP32 ADC raw value -> calibrated pH
```

Analoge Sensoren benötigen mehr Sorgfalt, weil die Spannung durch viele Dinge beeinflusst werden kann:

- Stromversorgung
- Kabellänge
- elektrisches Rauschen
- ADC-Verhalten
- Spannungsteiler
- Temperatur
- Sensordrift

Aus diesem Grund verwenden alle analogen Sensoren in diesem Projekt Kalibrierung.

---

## 5. Was ist ein ADC?

ADC bedeutet:

```text
Analog-to-Digital Converter
```

Er wandelt eine Spannung in eine Zahl um.

Ein vereinfachtes Beispiel:

```text
0.0 V -> ADC value 0
1.65 V -> ADC value around 2048
3.3 V -> ADC value around 4095
```

Das reale ESP32-ADC-Verhalten ist nicht perfekt linear und nicht perfekt präzise. Das ist ein Grund, warum dieses Projekt mit rohen ADC-Werten kalibriert, statt so zu tun, als wäre die Spannung exakt.

Interpretation auf Management-Ebene:

```text
The ADC is the translation layer between the physical electronics world and the digital software world.
```

---

## 6. Was ist Kalibrierung?

Kalibrierung bedeutet, einen Sensorwert mit bekannten Referenzen zu vergleichen und eine Zuordnung zu erstellen.

Beispiel für pH:

```text
known pH 4 solution -> raw ADC value A
known pH 7 solution -> raw ADC value B
known pH 10 solution -> raw ADC value C
```

Die Firmware verwendet diese Punkte dann, um pH für unbekannte Wasserproben zu schätzen.

Kalibrierung beantwortet die Frage:

```text
When this sensor and this ESP32 produce this raw number, what real-world value should it mean?
```

Ohne Kalibrierung sind analoge Sensorwerte oft nur grobe Hinweise.

---

## 7. Was ist Firmware?

Firmware ist Software, die direkt auf einem Gerät läuft.

In diesem Projekt ist Firmware der C++-Code, der auf dem ESP32 läuft.

Sie definiert:

- welche Sensoren verwendet werden
- welche Pins verwendet werden
- wie Werte gelesen werden
- wie Kalibrierung angewendet wird
- welcher fPort verwendet wird
- wie LoRaWAN konfiguriert ist
- wann der ESP32 schläft

Die generierte `platformio.ini` enthält nicht die Firmware selbst. Sie konfiguriert, wie die Firmware kompiliert wird.

---

## 8. Was ist PlatformIO?

PlatformIO ist ein Build- und Upload-Werkzeug für Embedded-Projekte.

Es beantwortet Fragen wie:

```text
Which board are we building for?
Which libraries are needed?
Which compile-time flags are active?
Which serial port should be used for upload?
```

In diesem Projekt verwenden Studierende normalerweise:

```bash
pio run
pio run -t upload
pio device monitor
```

Diese Kommandos bedeuten:

| Kommando | Bedeutung |
|---|---|
| `pio run` | Firmware kompilieren |
| `pio run -t upload` | Firmware auf den ESP32 flashen |
| `pio device monitor` | Serial Monitor öffnen |

---

## 9. Was ist LoRaWAN?

LoRaWAN ist das Kommunikationssystem, mit dem Sensordaten über lange Distanzen mit wenig Energie gesendet werden.

Ein vereinfachter Kommunikationspfad:

```text
ESP32 + LoRa radio
  -> radio signal
  -> LoRaWAN gateway
  -> TTN network server
  -> TTN application
  -> decoded payload
```

Die Boje verbindet sich nicht mit WLAN. Sie sendet ein kleines LoRaWAN-Paket. Ein Gateway empfängt es und leitet es ins Internet weiter.

Dadurch ist LoRaWAN für Outdoor-Umweltsensorik geeignet.

---

## 10. Was ist TTN?

TTN bedeutet The Things Network. In diesem Kurs verwenden wir The Things Networks Sandbox / TTN als LoRaWAN-Backend.

TTN stellt bereit:

- Geräteregistrierung
- OTAA-Join-Behandlung
- Live-Data-Ansicht
- Payload Formatter
- dekodierte Anwendungsdaten

Der ESP32 sendet Bytes. TTN empfängt Bytes. Der Payload Formatter erklärt, was diese Bytes bedeuten.

---

## 11. Was ist ein Payload Formatter?

Ein Payload Formatter ist JavaScript-Code, der in TTN installiert wird.

Er wandelt rohe Uplink-Bytes in menschenlesbare Felder um.

Beispiel:

```text
raw payload bytes -> "350.000000" -> tds_ppm = 350
```

Firmware und Payload Formatter müssen sich über fPorts einig sein.

Beispiel:

```text
fPort 4 means TDS
fPort 6 means PH4502C board temperature
```

Wenn TTN einen alten Formatter verwendet, können dekodierte Felder falsch sein, auch wenn die Gerätefirmware korrekt ist.

---

## 12. Was ist Deep Sleep?

Deep Sleep ist ein Low-Power-Zustand des ESP32.

Statt dauerhaft zu laufen, macht das Gerät Folgendes:

```text
wake briefly
measure
send
sleep for a long time
```

Das ist für Batterie- und Solarbetrieb wesentlich.

Eine nützliche Analogie ist ein Leuchtturm, der nur kurz aufblitzt. Er lässt nicht ständig einen Scheinwerfer eingeschaltet.

---

## 13. Warum das für Manager:innen wichtig ist

Manager:innen müssen oft Entscheidungen auf Basis technischer Systeme treffen, ohne jede Schaltung selbst zu entwerfen. In diesem Kurs geht es darum, genug technisches Verständnis aufzubauen, um bessere Fragen stellen zu können.

Beispiele für bessere Fragen:

- Ist dieser Wert kalibriert?
- Was misst der Sensor tatsächlich?
- Sind das absolute Daten oder Trenddaten?
- Wie oft wird der Wert aktualisiert?
- Was passiert, wenn die Batterie schwach ist?
- Welcher Teil des Systems kann ausfallen?
- Ist der Cloud-Wert korrekt dekodiert?
- Sehen wir einen echten Umwelteffekt oder ein Sensorartefakt?

Das Ziel ist nicht, jede:n Studierende:n zu Embedded Engineers zu machen. Das Ziel ist, technische Systeme verständlich genug zu machen, um Projekte mit solchen Systemen verantwortungsvoll führen zu können.
