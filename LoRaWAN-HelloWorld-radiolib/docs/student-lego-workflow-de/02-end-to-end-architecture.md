# Ende-zu-Ende-Architektur der Boje

Bis jetzt hat das Handbuch vor allem den ESP32-Bojenknoten und den TTN-Uplink-Pfad beschrieben. Ein vollständiges Kurssystem benötigt mehr: Die gemessenen Daten sollen nicht in TTN enden. Sie sollen in ein lokales Backend auf einem Raspberry Pi fließen, in einer Datenbank gespeichert und in einem Dashboard angezeigt werden.

Die vollständige Zielarchitektur ist:

```text
water sensors
  -> ESP32 firmware
  -> LoRa radio
  -> LoRaWAN gateway
  -> The Things Network / The Things Stack
  -> TTN MQTT integration
  -> mqttbridge on Raspberry Pi
  -> local mqttbroker on Raspberry Pi
  -> mqttcli storage process
  -> MariaDB database
  -> SNode.C / MQTTSuite dashboard backend
  -> browser dashboard
```

Dieses Kapitel gibt die Gesamtübersicht. Die folgenden Kapitel erklären jeden Teil genauer.

---

## Was läuft wo?

Eine häufige Ursache für Verwirrung ist, dass dieses Projekt mehrere Computer und Dienste verwendet. Die folgende Tabelle ist der wichtigste Orientierungspunkt.

| Komponente | Läuft auf | Zweck |
|---|---|---|
| ESP32 firmware | ESP32-Bojenknoten | liest Sensoren und sendet LoRaWAN-Uplinks |
| payload formatter | TTN cloud | dekodiert Uplink-Bytes in Felder wie `ph_level` |
| TTN MQTT integration | TTN cloud | stellt dekodierte Uplinks als MQTT-Nachrichten bereit |
| mqttbridge | Raspberry Pi | verbindet TTN MQTT mit dem lokalen MQTT-Broker |
| mqttbroker | Raspberry Pi | lokaler MQTT-Nachrichtenbroker / Message Hub |
| mqttcli | Raspberry Pi | abonniert Nachrichten und speichert sie in MariaDB |
| MariaDB | Raspberry Pi | dauerhafte Speicherung von Messwerten |
| SNode.C / dashboard server | Raspberry Pi | stellt API und Web-Dashboard bereit |
| browser | Laptop der Studierenden | zeigt Dashboard und TTN Console an |

Der Raspberry Pi ist der lokale Backend-Server. Der ESP32 ist das Feldgerät. TTN ist der Cloud-/Netzwerkteil.

---

## Warum TTN und einen lokalen Raspberry Pi verwenden?

TTN eignet sich sehr gut, um LoRaWAN-Uplinks zu empfangen und LoRaWAN-Geräte zu verwalten, aber TTN ist in diesem Kurs nicht die finale Datenplattform. Studierende sollen den vollständigen Weg vom Sensor zur lokalen Anwendung lernen.

Der Raspberry Pi ergänzt:

- einen lokalen MQTT-Broker
- eine lokale Datenbank
- lokale Verarbeitungswerkzeuge
- einen Dashboard-Server
- einen Ort, an dem Studierende Logs inspizieren und Kommandos ausführen können

Dadurch wird das System verständlicher als ein reines Black-Box-Cloud-Dashboard.

---

## Datenfluss in Stufen

Das System kann in Stufen aufgebaut werden.

| Level | Ziel | Fertig, wenn ... |
|---:|---|---|
| 1 | ESP32 sendet Daten an TTN | TTN Live Data dekodierte Felder zeigt |
| 2 | TTN stellt MQTT-Daten bereit | ein MQTT-Client TTN-Uplinks abonnieren kann |
| 3 | Raspberry Pi empfängt TTN-Daten | mqttbridge Uplinks zum lokalen Broker weiterleitet |
| 4 | Lokaler Broker verteilt Nachrichten | mqttcli lokal abonnieren kann |
| 5 | Datenbank speichert Historie | MariaDB Messwertzeilen enthält |
| 6 | Dashboard zeigt Daten | Browser aktuelle und historische Messwerte zeigt |

Dieser stufenweise Workflow ist wichtig: Versuchen Sie nicht, das Dashboard zu debuggen, bevor TTN-Uplinks korrekt dekodiert werden.

---

## Nachricht versus Messwert

Studierende sollen zwischen einer Nachricht und einem Messwert unterscheiden.

Eine Nachricht ist eine Transporteinheit. Sie kann Metadaten, Gerätekennungen, Zeitstempel, fPort und dekodierte Payload enthalten.

Ein Messwert ist der wissenschaftliche oder umweltbezogene Wert, der uns interessiert, zum Beispiel:

```text
ph_level = 7.12
tds_ppm = 350
turbidity_ntu = 42
```

MQTT transportiert Nachrichten. MariaDB speichert ausgewählte Teile dieser Nachrichten als Messwerte.

---

## Warum MQTT zweimal vorkommt

In diesem System gibt es zwei MQTT-Welten:

1. TTN MQTT, bereitgestellt von The Things Stack.
2. Local MQTT, bereitgestellt durch `mqttbroker` auf dem Raspberry Pi.

Die Bridge verbindet beide:

```text
TTN MQTT -> mqttbridge -> local mqttbroker
```

Diese Trennung ist nützlich, weil lokale Werkzeuge den lokalen Broker abonnieren können, ohne dass jedes Werkzeug TTN-Zugangsdaten benötigt.

---

## Warum MariaDB benötigt wird

MQTT ist ein Live-Nachrichtensystem. Es eignet sich sehr gut zum Verteilen frischer Nachrichten, ist aber keine historische Datenbank.

MariaDB speichert Historie. Das ermöglicht:

- Zeitbereichsabfragen
- Dashboards mit historischen Diagrammen
- Vergleich mehrerer Bojen
- Export von Daten zur Analyse
- Prüfung von Kalibrieränderungen über die Zeit

Kurz gesagt:

```text
MQTT = live stream
MariaDB = memory
Dashboard = human view
```

---

## Verantwortungsmodell im Kurs

Das Kurssystem enthält mehrere Verantwortlichkeiten:

| Verantwortung | Werkzeug / Komponente |
|---|---|
| physikalische Messung | Sensoren und Kalibrierung |
| Embedded-Steuerung | ESP32 firmware |
| Langstreckenkommunikation | LoRaWAN / TTN |
| Nachrichtenintegration | MQTT / mqttbridge |
| lokale Nachrichtenverteilung | mqttbroker |
| Persistenz | mqttcli + MariaDB |
| Benutzeroberfläche | SNode.C / dashboard frontend |

Ein vollständiges IoT-System ist nur so gut wie das schwächste Glied in dieser Kette.
