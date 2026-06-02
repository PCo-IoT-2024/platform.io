# MQTT-Grundlagen für das Bojen-Backend

MQTT ist das Nachrichtensystem zwischen TTN, dem Raspberry Pi und den lokalen Backend-Werkzeugen.

Die wichtige Idee ist:

```text
MQTT verteilt Nachrichten zwischen Programmen.
MQTT speichert Messhistorie nicht dauerhaft selbst.
```

Für dieses Projekt ist MQTT der Live-Message-Bus. MariaDB ist das Langzeitgedächtnis.

---

## Warum MQTT verwendet wird

MQTT ist im IoT verbreitet, weil es einfach, leichtgewichtig und auf einem Broker-Modell basiert.

Ein Bojensystem erzeugt kleine Nachrichten:

```text
neuer pH-Wert
neuer TDS-Wert
neuer Trübungswert
neue GPS-Position
```

MQTT ist gut geeignet, diese Nachrichten an mehrere Consumer zu verteilen.

Beispiele für Consumer:

- Datenbank-Storage-Prozess
- Live-Dashboard
- Debugging-Terminal
- Alerting-Prozess
- Datenexport-Werkzeug

---

## Broker, Publisher, Subscriber

MQTT hat drei Grundrollen.

| Rolle | Bedeutung | Beispiel in diesem Projekt |
|---|---|---|
| Broker | Message Hub | `mqttbroker` auf Raspberry Pi |
| Publisher | sendet Nachrichten an Broker | `mqttbridge` |
| Subscriber | empfängt Nachrichten vom Broker | `mqttcli`, Dashboard-Backend |

Eine nützliche Analogie:

```text
Broker = Postamt
Topic = Name des Postfachs
Publisher = Person, die Briefe sendet
Subscriber = Person, die Briefe aus ausgewählten Postfächern empfängt
```

Publisher und Subscriber müssen einander nicht direkt kennen. Sie müssen sich nur über Topic-Namen einig sein.

---

## Topics

Ein Topic ist ein hierarchischer Name für Nachrichten.

Beispiel-Topic-Ideen:

```text
ttn/water-buoy-course/sx1262-v11-v104-01/up
water/sx1262-v11-v104-01/ph
water/sx1262-v11-v104-01/tds
water/sx1262-v11-v104-01/turbidity
```

Topic-Design ist wichtig. Eine gute Topic-Struktur erleichtert Debugging und Datenverarbeitung.

Eine einfache Kurskonvention sollte enthalten:

- Quellsystem, zum Beispiel `ttn`
- Application ID
- Device ID
- Nachrichtentyp, zum Beispiel `up`

---

## Payload

Die Payload ist der Nachrichteninhalt.

In TTN MQTT ist die Payload normalerweise JSON. Sie enthält viel mehr als nur den Messwert:

- Gerätekennungen
- Application ID
- Zeit
- fPort
- decoded payload
- Funkmetadaten
- Gateway-Informationen

Der lokale Storage-Prozess sollte die Teile auswählen, die für die Datenbank nützlich sind.

---

## Wildcards

MQTT unterstützt Wildcards für Subscriptions.

| Wildcard | Bedeutung |
|---|---|
| `+` | eine Topic-Ebene |
| `#` | alle verbleibenden Topic-Ebenen |

Beispiele:

```text
ttn/+/+/up
```

passt zu Uplinks vieler Applications/Devices, wenn die lokale Topic-Konvention genau diese Ebenen hat.

```text
#
```

abonniert alles. Das ist für Debugging nützlich, aber für Produktion zu breit.

---

## QoS

QoS bedeutet Quality of Service.

Häufige MQTT-QoS-Stufen:

| QoS | Bedeutung |
|---:|---|
| 0 | höchstens einmal |
| 1 | mindestens einmal |
| 2 | genau einmal, komplexer |

Für ein Lehr-Setup sind QoS 0 oder QoS 1 normalerweise ausreichend. QoS repariert keine schlechte Sensorkalibrierung und ersetzt keine Datenbanktransaktionen.

---

## Retained Messages

Eine MQTT-Retained-Message ist die letzte Nachricht, die der Broker für ein Topic hält. Ein neuer Subscriber erhält sie sofort.

Retained Messages sind nützlich für Zustand, zum Beispiel:

```text
system/status
```

Für rohe Messwertströme sind sie meist weniger nützlich, weil die Datenbank Historie speichern sollte.

---

## Lokaler Broker versus TTN-Broker

Es gibt zwei Broker-Kontexte:

| Broker | Ort | Zweck |
|---|---|---|
| TTN MQTT broker | TTN cloud | stellt TTN-Application-Traffic bereit |
| local mqttbroker | Raspberry Pi | lokaler Message Hub für das Kurs-Backend |

Die Bridge verbindet sie:

```text
TTN MQTT broker -> mqttbridge -> local mqttbroker
```

Dadurch braucht nicht jedes lokale Werkzeug direkte TTN-Zugangsdaten.

---

## Warum eine Bridge verwendet wird

Eine Bridge leitet Nachrichten zwischen zwei MQTT-Systemen weiter.

In diesem Kurs:

```text
mqttbridge abonniert TTN-Uplinks
mqttbridge veröffentlicht sie lokal erneut
```

Vorteile:

- ein Ort für TTN-Zugangsdaten
- lokales Backend kann lokale Topics verwenden
- einfacheres Debugging
- lokaler Broker kann mehrere Subscriber bedienen
- Architektur ist sichtbar und lehrbar

---

## Fertig, wenn ...

Sie verstehen MQTT ausreichend, wenn Sie erklären können:

```text
[ ] was der Broker macht
[ ] was ein Topic ist
[ ] was Publisher und Subscriber bedeuten
[ ] warum TTN MQTT und lokales MQTT unterschiedlich sind
[ ] warum mqttbridge existiert
[ ] warum MQTT nicht die Datenbank ist
```

Die nächsten Kapitel verwenden diese Konzepte mit den MQTTSuite-Werkzeugen.
