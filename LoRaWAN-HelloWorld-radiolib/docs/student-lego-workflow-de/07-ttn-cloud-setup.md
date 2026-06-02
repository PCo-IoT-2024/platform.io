# TTN-Cloud-Setup

Dieses Kapitel beschreibt den Cloud-Teil des Projekts: The Things Network / The Things Stack so einzurichten, dass die ESP32-Boje joinen und dekodierte Uplinks senden kann.

Ziel ist, dass Studierende eine TTN-Anwendung erstellen, ein Gerät registrieren, den Payload Formatter installieren und den MQTT-Zugang für das Raspberry-Pi-Backend vorbereiten können.

---

## Was TTN in diesem Projekt macht

TTN ist das LoRaWAN-Netzwerk-Backend. Es ist verantwortlich für:

- Empfangen von Paketen von LoRaWAN-Gateways
- Behandlung von OTAA Joins
- Prüfung von Frame Countern und Security
- Routing von Uplinks zur richtigen Anwendung
- Ausführen des Payload Formatter
- Bereitstellen von Daten über Live-Data-Ansichten und MQTT

TTN ist nicht die ESP32-Firmware und nicht die lokale Datenbank. Es ist die Cloud-/Netzwerkschicht zwischen LoRaWAN-Funk und Anwendungssoftware.

---

## Eine Anwendung erstellen

In der TTN Console:

1. Bei The Things Stack / TTN einloggen.
2. **Applications** öffnen.
3. Eine neue Application erstellen.
4. Eine klare Application ID wählen, zum Beispiel:

```text
water-buoy-course
```

Die Application ist der Container für alle Bojengeräte, die zu diesem Projekt gehören.

Für Management-Studierende ist eine gute Analogie:

```text
TTN application = project folder for LoRaWAN devices
TTN end device = one physical buoy
```

---

## Ein Endgerät erstellen

Innerhalb der Application:

1. Ein neues End Device erstellen.
2. OTAA activation wählen.
3. Die LoRaWAN-Version passend zum Generator auswählen.
4. Die regional parameters / frequency plan passend zur Firmware-Region auswählen.
5. Für den Kurs in Europa EU868 verwenden.
6. Eine DevEUI setzen oder erzeugen.
7. Die Kurs-JoinEUI verwenden:

```text
0000000000000000
```

8. AppKey setzen oder erzeugen.
9. NwkKey setzen oder erzeugen, falls LoRaWAN 1.1.0 verwendet wird.

Generator und TTN müssen übereinstimmen bei:

```text
DevEUI
JoinEUI
AppKey
NwkKey, if LoRaWAN 1.1.0
LoRaWAN version
region
```

---

## Wahl der LoRaWAN-Version

Der Generator bietet:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

Für LoRaWAN 1.1.0 werden beide Schlüssel verwendet:

```text
AppKey
NwkKey
```

Für LoRaWAN 1.0.x wird in diesem Workflow nur der AppKey verwendet. Der Generator deaktiviert NwkKey.

Wenn das Gerät nicht joinen kann, prüfen Sie zuerst die LoRaWAN-Version. Eine Nichtübereinstimmung zwischen Generator und TTN kann verwirrende Join-Fehler verursachen.

---

## Payload Formatter installieren

Nach dem Erzeugen von `payload-formatter.js` im lokalen Generator:

1. Die TTN-Application- oder End-Device-Einstellungen öffnen.
2. Zu **Payload formatters** gehen.
3. **Uplink** auswählen.
4. **Custom JavaScript formatter** wählen.
5. Die generierte `payload-formatter.js` einfügen.
6. Speichern.

Der Formatter muss zur generierten Firmware-Konfiguration passen.

Die erwarteten Messfelder sind:

| fPort | Dekodiertes Feld |
|---:|---|
| 1 | `latitude`, `longitude`, `altitude`, `hdop` |
| 2 | `temperature_c` |
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

Der Payload Formatter ist fertig, wenn TTN Live Data dekodierte Messfelder und keinen JavaScript-Formatter-Fehler zeigt.

---

## MQTT API Key erstellen

Das Raspberry-Pi-Backend benötigt MQTT-Zugang zu TTN.

In TTN:

1. Die Application öffnen.
2. **API keys** öffnen.
3. Einen neuen API Key erstellen.
4. Einen klaren Namen vergeben, zum Beispiel:

```text
raspberry-pi-mqtt-bridge
```

5. Rechte vergeben, die zum Lesen von Application Traffic benötigt werden.
6. Den erzeugten Key sofort kopieren.

Behandeln Sie diesen Key wie ein Passwort. Committen Sie ihn nicht nach GitHub.

---

## TTN-MQTT-Verbindungsinformationen

Studierende benötigen diese Werte für die mqttbridge-Konfiguration:

| Wert | Bedeutung |
|---|---|
| TTN MQTT server | MQTT-Endpunkt des The-Things-Stack-Clusters |
| application ID | TTN-Application-Identifier |
| username | normalerweise Application ID plus Tenant-Suffix, abhängig vom TTN-Cluster |
| password | API Key |
| uplink topic | Topic Pattern für Uplink-Nachrichten |

Ein typisches The-Things-Stack-Uplink-Topic-Pattern ist konzeptionell:

```text
v3/<application-id>@ttn/devices/<device-id>/up
```

Der genaue Username und Server hängen vom TTN-Cluster und Tenant ab. Studierende sollen sie aus der TTN-Integrationsdokumentation oder MQTT-Integrationsseite für den ausgewählten Cluster kopieren.

---

## Live-Data-Test

Bevor der Raspberry Pi verbunden wird, prüfen Sie TTN selbst.

Sie sind fertig, wenn TTN Live Data zeigt:

- erfolgreichen Join oder wiederhergestellte Session
- Uplinks vom richtigen Gerät
- erwarteten fPort
- dekodierte Payload-Felder
- keinen Formatter-Fehler

Beispiel für dekodierte TDS-Payload:

```json
{
  "tds_ppm": 350.0
}
```

Beispiel für dekodierte PH4502C-Boardtemperatur-Payload:

```json
{
  "ph_board_temperature_c": 26.0
}
```

---

## Häufige TTN-Probleme

| Symptom | Wahrscheinliche Ursache |
|---|---|
| kein Join | falsche Schlüssel, falsche Region, falsche LoRaWAN-Version, Funkverdrahtung |
| Join funktioniert, aber keine dekodierten Felder | fehlender oder veralteter Payload Formatter |
| Formatter-Syntaxfehler | unvollständiges JavaScript nach TTN kopiert |
| unerwarteter fPort | Firmware und Formatter aus unterschiedlichen Einstellungen generiert |
| Join hat vor Reset funktioniert, jetzt nicht | Nonce-/Session-Mismatch |

Debug-Reihenfolge:

```text
serial monitor -> TTN live data -> payload formatter -> MQTT integration
```

Beginnen Sie nicht mit MQTT-Debugging, bevor TTN Live Data korrekt ist.
