# 04 — Firmware-Architektur

Die Firmware ist um eine Hauptanwendungsdatei und mehrere Sensorklassen organisiert.

Hauptanwendung:

```text
src/LoRaWANTemplate.cpp
```

Sensorklassen:

```text
src/sensors/
```

LoRaWAN-Unterstützung:

```text
src/LoRaWAN.hpp
```

GPS-Unterstützung:

```text
src/GPS.cpp
src/GPS.h
```

## Compile-Time-Konfiguration

Der Generator schreibt C/C++-Präprozessordefinitionen in `platformio.ini`.

Beispiele:

```ini
-D APP_HAS_GPS=1
-D APP_HAS_TEMPERATURE=1
-D APP_HAS_PH=1
-D APP_HAS_TDS=1
-D APP_HAS_TURBIDITY=1
-D APP_HAS_PH_BOARD_TEMPERATURE=1
```

Der C++-Code verwendet diese Definitionen mit `#if` und `#endif`.

Das bedeutet, dass nicht verwendeter Sensorcode nicht Teil des aktiven Build-Pfads ist.

## Konstruktion der Sensorobjekte

Sensorobjekte werden nur erzeugt, wenn ihre Feature-Flags aktiviert sind.

Beispiele:

```cpp
#if APP_HAS_TEMPERATURE
static temperature::DS18B20 temp(DALLAS_TEMPERATURE_PIN);
#endif
```

Das PH4502C-Objekt wird sowohl für pH als auch für Boardtemperatur verwendet:

```text
pH-Wert                    -> fPort 3
PH4502C-Boardtemperatur    -> fPort 6
```

## Vorbereiteter Uplink

Die Firmware verwendet eine kleine Struktur für den nächsten Uplink:

```cpp
struct PreparedUplink {
    uint8_t fPort = 221;
    std::string payload;
};
```

Jede Sensor-Vorbereitungsfunktion schreibt:

```text
fPort
Payload-String
```

Beispiele:

```text
pH       -> fPort 3, payload = "7.120000"
TDS      -> fPort 4, payload = "350.000000"
Trübung  -> fPort 5, payload = "42.000000"
```

## Sensor-Slot-Tabelle

Aktivierte Sensoren werden in eine Compile-Time-Tabelle eingetragen.

Konzeptionell:

```text
sensorSlots[] = {
  GPS,
  Temperatur,
  pH,
  TDS,
  Trübung,
  PH4502C-Boardtemperatur
}
```

Die tatsächlichen Einträge hängen von den Generator-Flags ab.

Der aktuelle Sensor wird mit dem RTC-Boot-Count ausgewählt:

```text
currentSensor = (bootCount - 1) % sensorCount
```

Dadurch entsteht eine deterministische Rotation durch die aktivierten Sensoren.

## Warum nicht alles auf einmal messen?

Alle Sensoren bei jedem Wake-up zu messen wäre einfacher, würde aber Folgendes erhöhen:

- Wake-Zeit
- Sensorstromverbrauch
- LoRa-Payload-Größe
- TTN-Airtime
- Batterieverbrauch

Die Strategie „ein Sensor pro Wake-up“ ist besser für Batterie- und Solarbetrieb.

## Kalibriermodus

Kalibriermodi werden vor dem normalen LoRaWAN-Betrieb betreten.

Startsequenz, vereinfacht:

```text
serielle Schnittstelle starten
Wake-Grund ausgeben
Kalibrierbuttons prüfen
wenn Kalibrierung angefordert: Kalibrierschleife ausführen
sonst normales LoRaWAN-Setup fortsetzen
```

Kalibrierschleifen geben rohe ADC-Werte und den aktuell berechneten kalibrierten Wert aus.

Beispiel:

```text
[CAL] sensor=tds, raw_adc=1800.00, calibrated_ppm=1000.000
```

Wenn der Kalibrierbutton losgelassen wird, startet der ESP32 neu.

## LoRaWAN-Persistenz

Die Firmware speichert LoRaWAN-Session-Informationen, damit das Gerät nach jedem Deep Sleep keinen vollständigen OTAA Join durchführen muss.

Wichtige Konzepte:

| Konzept | Bedeutung |
|---|---|
| Session | aktiver LoRaWAN-Zustand nach Join |
| Nonce | Wert zum Verhindern von Replay und doppelten Joins |
| Frame Counter | von LoRaWAN verfolgter Uplink-/Downlink-Zähler |

Factory Reset löscht die Session, erhält aber Nonces. Dangerous Nonce Reset löscht mehr Zustand und muss vorsichtig verwendet werden.

## Deep Sleep

Nachdem der LoRaWAN-Loop den Uplink gesendet hat, legt die Firmware das Funkmodul schlafen und startet ESP32 Deep Sleep.

Die Wake-up-Quelle ist normalerweise der Timer, der aus dem generierten Uplink-Intervall konfiguriert wird.

Deep Sleep erhält RTC-Speicher, sodass Boot Count und die letzte gültige Wassertemperatur den Schlaf überleben können.

## Letzte Wassertemperatur

Der DS18B20-Wassertemperaturwert kann gespeichert und für die TDS-Temperaturkompensation wiederverwendet werden.

Das ist nützlich, weil TDS und Temperatur eventuell nicht im selben Wake-up gemessen werden.

Wenn keine gültige Wassertemperatur verfügbar ist, verwendet TDS den Fallback-Wert aus dem Generator.
