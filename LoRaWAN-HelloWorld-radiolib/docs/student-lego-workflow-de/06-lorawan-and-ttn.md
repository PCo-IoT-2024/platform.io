# 02 — LoRaWAN und TTN-Theorie

LoRaWAN ist eine Low-Power-Wide-Area-Network-Technologie. Sie ist für Geräte gedacht, die kleine Datenmengen über große Distanzen senden und dabei wenig Energie verbrauchen.

Dieses Projekt verwendet LoRaWAN, weil eine Wasserüberwachungsboje kein WLAN benötigen und mit Batterie und Solarpanel laufen können soll.

Für Management-Studierende ist die wichtigste Idee:

```text
LoRaWAN is not just radio. It is an operating model for low-power distributed sensing.
```

Es definiert, wie ein kleines Gerät sicher und effizient Daten in eine Anwendung senden kann, ohne selbst eine permanente Internetverbindung zu haben.

---

## 1. Warum nicht Wi-Fi?

Wi-Fi ist gut, wenn ein Gerät nahe an einem Access Point ist und mehr Energie verwenden kann. Eine Boje oder ein Umwelt-Sensorknoten hat andere Anforderungen.

| Anforderung | Wi-Fi | LoRaWAN |
|---|---|---|
| kurze Reichweite um Gebäude | sehr gut | möglich, aber nicht Hauptstärke |
| Langstrecken-Outdoor-Sensorik | begrenzt | stark |
| hohe Datenrate | sehr gut | niedrig |
| niedrige Energie | mittel bis schlecht | sehr gut |
| kleine Sensornachrichten | möglich | ideal |
| Batterie-/Solarbetrieb | schwieriger | vorgesehener Anwendungsfall |

Die Boje sendet kleine Nachrichten wie:

```text
pH = 7.12
TDS = 350 ppm
turbidity = 42 NTU
```

Sie sendet kein Video, Audio oder große Dateien. Dadurch passt LoRaWAN gut.

---

## 2. LoRa versus LoRaWAN

LoRa und LoRaWAN sind nicht dasselbe.

| Begriff | Bedeutung |
|---|---|
| LoRa | Funkmodulation für Langstreckenkommunikation |
| LoRaWAN | Netzwerkprotokoll und Systemarchitektur auf Basis von LoRa |

Eine Analogie:

```text
LoRa is like the acoustic properties of a language.
LoRaWAN is like the postal system, addresses, rules, envelopes, and delivery process.
```

LoRa definiert, wie das Funksignal geformt wird. LoRaWAN definiert, wie Geräte dem Netzwerk beitreten, wie Nachrichten verschlüsselt werden, wie Zähler geprüft werden, wie Gateways Daten weiterleiten und wie die Anwendung Uplinks erhält.

---

## 3. LoRaWAN als Mehrschichtsystem

Ein LoRaWAN-Uplink durchläuft mehrere Schichten:

```text
sensor value
  -> ESP32 firmware payload
  -> LoRaWAN encrypted frame
  -> LoRa radio transmission
  -> gateway reception
  -> network server processing
  -> application server
  -> payload formatter
  -> decoded application data
```

Jede Schicht hat eine andere Verantwortung.

| Schicht | Frage, die sie beantwortet |
|---|---|
| sensor layer | Welcher physikalische Wert wurde gemessen? |
| firmware layer | Wie kodieren wir diesen Wert? |
| LoRaWAN layer | Darf dieses Gerät senden? Ist der Frame gültig? |
| gateway layer | Wer hat die Funknachricht gehört? |
| network server | Ist der Frame Counter gültig? Welche Anwendung gehört dazu? |
| application layer | Was bedeutet diese Payload? |
| payload formatter | Welche JSON-Felder soll TTN anzeigen? |

Diese Trennung ist mächtig, bedeutet aber auch, dass Debugging systematisch erfolgen muss.

---

## 4. Wichtige LoRaWAN-Komponenten

| Komponente | Rolle |
|---|---|
| End device | der ESP32-Bojenknoten |
| Gateway | empfängt LoRa-Funkpakete und leitet sie ins Internet weiter |
| Network Server | behandelt LoRaWAN-MAC-Schicht, Frame Counter, Routing, Security |
| Join Server | behandelt OTAA Join und Session-Key-Erzeugung |
| Application Server | stellt Anwendungsdaten für Benutzer:innen bereit |

In diesem Projekt stellt TTN / The Things Stack die serverseitige Infrastruktur bereit.

Das Endgerät weiß nicht, welches Gateway das Paket empfangen wird. Es sendet einfach. Ein oder mehrere Gateways können es hören. Danach entscheidet der Network Server, was mit dem Paket passiert.

Das unterscheidet sich von Wi-Fi, wo ein Gerät mit einem bestimmten Access Point verbunden ist.

---

## 5. Uplink und Downlink

Eine Nachricht vom Gerät zu TTN ist ein Uplink.

Eine Nachricht von TTN zum Gerät ist ein Downlink.

In diesem Kursprojekt ist die Hauptrichtung Uplink:

```text
buoy -> TTN
```

Downlinks sind möglich, sollten aber vorsichtig verwendet werden. Sie verbrauchen Gateway-Airtime und Geräteenergie. LoRaWAN ist nicht für dauernden Zwei-Wege-Chat gedacht.

Das System ist am besten zu verstehen als:

```text
small sensor reports, sent occasionally
```

nicht als:

```text
continuous live connection
```

---

## 6. OTAA: dem Netzwerk beitreten

Die Firmware verwendet OTAA:

```text
Over-The-Air Activation
```

Bei OTAA speichert das Gerät nicht dauerhaft alle finalen Session Keys. Stattdessen tritt es dem Netzwerk mit Root Keys bei. Während des Join werden Session Keys erzeugt.

Eine vereinfachte OTAA-Geschichte:

```text
Device: Hello, I am this DevEUI and I know the correct secret.
Network: I verify you and create session keys.
Device: From now on I use these session keys for data messages.
```

Wichtige OTAA-Kennungen und Schlüssel:

| Name | Zweck |
|---|---|
| DevEUI | eindeutige Geräteidentität |
| JoinEUI | Join-/Application-Identität; in diesem Kurs-Setup null |
| AppKey | Application Root Key |
| NwkKey | Network Root Key für LoRaWAN 1.1.0 |

Für dieses Kurs-Setup:

```text
JoinEUI = 0000000000000000
```

---

## 7. DevEUI, AppKey und NwkKey als Management-Konzepte

Eine nützliche Sicht auf die Kennungen:

| Element | Analogie |
|---|---|
| DevEUI | Geräte-Passnummer |
| AppKey | Geheimnis zum Nachweis der Application-Identität |
| NwkKey | Geheimnis für Netzwerkidentität in LoRaWAN 1.1.0 |
| JoinEUI | Join-/Application-Domain, zu der das Gerät gehört |

Die DevEUI ist nicht geheim. Die Keys sind geheim.

Verwenden Sie echte AppKeys oder NwkKeys nicht in öffentlichen Screenshots, öffentlichen Repositories, Berichten oder Folien.

---

## 8. LoRaWAN 1.1.0 versus 1.0.x

Der Generator bietet:

```text
LoRaWAN 1.1.0
LoRaWAN 1.0.x
```

Für LoRaWAN 1.1.0 verwendet der Generator:

```text
AppKey + NwkKey
```

Für LoRaWAN 1.0.x verwendet der Generator:

```text
AppKey only
```

Darum ist die NwkKey-Eingabe deaktiviert, wenn `LoRaWAN 1.0.x` ausgewählt ist.

Bedeutung auf Management-Ebene:

```text
Different protocol versions have different security/key models.
The device configuration and TTN configuration must match.
```

Wenn die Firmware LoRaWAN 1.1.0 annimmt, TTN aber anders konfiguriert ist, kann der Join fehlschlagen.

---

## 9. Regionen und Frequency Plans

LoRaWAN-Regionen definieren Frequency Plans und Datenratenregeln.

Für den Kurs in Europa verwenden Sie:

```text
EU868
```

Der Generator bietet außerdem andere RadioLib-unterstützte Regionen wie:

```text
US915
EU433
AU915
CN470
AS923
AS923_2
AS923_3
AS923_4
KR920
IN865
```

Das sind nicht nur Bezeichnungen. Sie beschreiben legales und technisches Funkverhalten.

Ein Gerät mit falscher Region kann:

- auf falschen Frequenzen senden
- nicht joinen
- lokale Regeln verletzen
- von nahegelegenen Gateways nicht gehört werden

Für Studierende in Österreich / Europa ist EU868 der richtige Kursstandard.

---

## 10. fPorts

Ein LoRaWAN-fPort identifiziert den Typ der Application Payload.

In diesem Projekt:

```text
fPort 1 -> GPS
fPort 2 -> water temperature
fPort 3 -> pH
fPort 4 -> TDS
fPort 5 -> turbidity
fPort 6 -> PH4502C board temperature
```

Der TTN Payload Formatter schaltet anhand von `input.fPort` um.

Eine hilfreiche Analogie:

```text
The fPort is like the label on a small envelope.
The payload formatter opens the envelope differently depending on the label.
```

Wenn das Label „TDS“ sagt, erwartet der Formatter einen TDS-Wert. Wenn das Label „pH“ sagt, erwartet er einen pH-Wert.

---

## 11. Payloads und Payload Formatter

Die Firmware sendet Bytes. TTN kann nicht automatisch wissen, was diese Bytes bedeuten.

Beispiel für rohen Payload-Text:

```text
350.000000
```

Ohne Kontext könnte das bedeuten:

- 350 ppm TDS
- 350 Meter Höhe
- 350 NTU Trübung
- 350 Sekunden
- eine Diagnosenummer

fPort und Payload Formatter liefern den Kontext.

Aktuelles Formatter-Verhalten:

| fPort | Dekodiertes Ergebnis |
|---:|---|
| 3 | `ph_level` |
| 4 | `tds_ppm` |
| 5 | `turbidity_ntu` |
| 6 | `ph_board_temperature_c` |

Der aktuelle Mess-Formatter gibt absichtlich kein `payload_raw` aus. Studierende sollen saubere dekodierte Messfelder sehen.

---

## 12. Payload-Größe und Airtime

LoRaWAN ist für kleine Nachrichten optimiert. Zu viele Daten zu senden ist schlecht für:

- Batterielebensdauer
- Funk-Airtime
- Fairness im Netzwerk
- Gateway-Kapazität

Deshalb sendet die aktuelle Firmware im Normalbetrieb nur den Messwert:

```text
fPort 3 -> pH only
fPort 4 -> TDS ppm only
fPort 5 -> turbidity NTU only
fPort 6 -> board temperature only
```

Rohe ADC-Kalibrierwerte bleiben auf dem Serial Monitor. Sie sind während der Kalibrierung wichtig, sollten aber nicht in jedem Feld-Uplink gesendet werden.

---

## 13. Frame Counter und Nonces

LoRaWAN schützt gegen Replay-Angriffe. Ein Replay-Angriff bedeutet, dass jemand eine gültige Funknachricht aufzeichnet und später erneut sendet.

Zum Schutz dagegen verwendet LoRaWAN Counter und Nonces.

Vereinfacht:

```text
The network remembers what it has already seen.
Old or repeated values are rejected.
```

Darum sind Resets wichtig.

| Reset-Typ | Bedeutung |
|---|---|
| Factory reset | löscht gespeicherte Session, erhält aber Nonces |
| Dangerous nonce reset | löscht Session und Nonces |

Verwenden Sie Dangerous Nonce Reset nur, wenn TTN ebenfalls passend zurückgesetzt wurde oder wenn eine neue DevEUI verwendet wird.

Sonst kann das Gerät versuchen, Werte wiederzuverwenden, die TTN ablehnt.

---

## 14. TTN Live Data als Kontrollraum des Projekts

TTN Live Data ist die wichtigste Debugging-Ansicht. Sie zeigt:

- Join-Versuche
- akzeptierte Joins
- Uplinks
- fPorts
- rohe Payloads
- dekodierte Payload-Felder
- Payload-Formatter-Fehler

Beim Debugging immer beide Seiten prüfen:

```text
serial monitor on the ESP32
TTN live data in the browser
```

Der Serial Monitor zeigt, was das Gerät versucht hat. TTN zeigt, was das Netzwerk tatsächlich empfangen und dekodiert hat.

---

## 15. Typische LoRaWAN-Fehlerkategorien

| Symptom | Mögliche Kategorie |
|---|---|
| kein Join | falsche Keys, falsche LoRaWAN-Version, falsche Region, schlechte Funkverdrahtung |
| Join gelingt, aber keine dekodierten Daten | Formatter fehlt oder ist falsch |
| dekodierte Felder falsch | veralteter Formatter oder falsche fPort-Zuordnung |
| sporadische Uplinks | Abdeckung, Antenne, Stromversorgung, Timing, Gateway-Verfügbarkeit |
| Join hat vor Reset funktioniert, danach nicht | Nonce-/Session-Problem |

Die wichtige Fähigkeit ist nicht Raten. Arbeiten Sie Schicht für Schicht.

---

## 16. Management-Takeaway

Für Manager:innen ist LoRaWAN interessant, weil es Verantwortlichkeiten trennt:

| Verantwortung | Technische:r Owner |
|---|---|
| physikalische Messung | Sensor-/Sondendesign |
| lokale Steuerung | Firmware |
| Funkzugang | LoRa-Funk + Antenne |
| Netzwerkbetrieb | Gateways + Network Server |
| Anwendungsinterpretation | Payload Formatter + Datenbank/Dashboard |
| Entscheidungsfindung | Menschen und Organisationen |

Ein zuverlässiges IoT-Projekt braucht alle Schichten passend zueinander. Ein perfektes Dashboard kann schlechte Kalibrierung nicht reparieren. Ein perfekter Sensor hilft nicht, wenn der Payload Formatter das falsche Feld dekodiert. Eine gute Projektleitung muss die Schnittstellen zwischen den Schichten verstehen.
