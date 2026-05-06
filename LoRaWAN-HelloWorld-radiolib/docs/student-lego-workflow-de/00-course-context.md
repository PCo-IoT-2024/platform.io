# Kurskontext und Lernziele

Dieses Projekt ist ein Lehrprojekt zum Aufbau einer Internet-of-Things-Wasserüberwachungsboje. Das Ziel ist nicht nur, dass ein ESP32 Werte an TTN sendet, sondern zu verstehen, wie ein solches System vom Sensor bis zur Cloud-Datenansicht entworfen wird.

Studierende arbeiten mit einem realistischen, aber noch gut handhabbaren System:

```text
sensors -> ESP32 -> LoRa radio -> LoRaWAN -> TTN -> decoded data
```

Das System ist bewusst modular. Jeder Sensor ist ein Baustein, und der Generator wählt aus, welche Bausteine für eine bestimmte Boje verwendet werden.

## Was Studierende lernen sollen

Nach Abschluss dieses Projekts sollen Studierende diese Aufgaben erklären und durchführen können:

- die Rolle eines Mikrocontrollers in einem IoT-Sensorknoten beschreiben
- erklären, warum LoRaWAN für energiesparende Langstrecken-Sensorik nützlich ist
- zwischen Gerätefirmware, Netzwerkinfrastruktur und Anwendungsdaten unterscheiden
- ein TTN-OTAA-Gerät konfigurieren
- aus Geräteeinstellungen eine PlatformIO-Konfiguration erzeugen
- eine ESP32-Firmware flashen
- serielle Diagnoseausgaben lesen
- einen TTN Payload Formatter installieren und testen
- verstehen, warum analoge Sensoren Kalibrierung benötigen
- rohe ADC-Werte für die Kalibrierung verwenden
- zwischen Wassertemperatur und Boardtemperatur unterscheiden
- über Low-Power-Design und Sensor-Power-Gating argumentieren

## Warum das ein Lego-Workflow ist

Das Projekt heißt „lego“, weil die Firmware aus auswählbaren Bausteinen zusammengesetzt werden kann.

Beispiele:

```text
GPS only
GPS + temperature
pH + temperature
pH + TDS + turbidity
full water-quality buoy
```

Der aktuelle Generator versteckt den Großteil der Compile-Time-Komplexität. Studierende wählen Sensoren und Pins in der Benutzeroberfläche aus. Der Generator schreibt dann die notwendigen Build-Flags in `platformio.ini`.

## Wichtige Engineering-Idee

Die wichtige Engineering-Idee ist, dass Hardware, Firmware und Cloud-Decoding zusammenpassen müssen.

Diese drei Dinge gehören zusammen:

```text
selected sensors in generator
build flags in platformio.ini
fPort cases in payload-formatter.js
```

Wenn ein Teil geändert wird, die anderen aber nicht neu generiert werden, kann das System zwar weiterhin kompilieren und Uplinks senden, aber TTN kann sie falsch dekodieren.

## Aktueller Kurs-Branch

Verwenden Sie:

```text
course/lego-configurable
```

Beginnen Sie nicht von `main` oder einem älteren experimentellen Branch, außer Sie werden ausdrücklich dazu aufgefordert.

## Deliverables der Studierenden

Ein sinnvolles Deliverable einer Studierendengruppe könnte enthalten:

- funktionierender Fork und Branch
- generierte `platformio.ini`
- dokumentiertes Pin-Mapping
- aufgezeichnete Kalibrierwerte
- Screenshots oder Exporte von dekodierten TTN-Uplinks
- kurze Erklärung der ausgewählten Sensoren
- kurze Diskussion von Grenzen und erwarteter Genauigkeit
- optionale Notizen zu Power Design und Deep Sleep

## Sicherheit und Verantwortung

Veröffentlichen Sie keine echten AppKey- oder NwkKey-Werte. Behandeln Sie LoRaWAN-Root-Keys als Secrets.

Bringen Sie Elektronik nicht direkt ins Wasser. Verwenden Sie geeignete Wasserdichtung und Zugentlastung.

Gehen Sie nicht davon aus, dass günstige Sensoren Laborinstrumente sind. Sie sind nützlich zum Lernen, zur Trendbeobachtung und für relative Änderungen, aber die meisten benötigen Kalibrierung und sorgfältige Interpretation.
