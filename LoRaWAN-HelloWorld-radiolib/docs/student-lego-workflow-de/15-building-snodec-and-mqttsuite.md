# SNode.C und MQTTSuite am Raspberry Pi bauen

**Primärer Workstream:** Student:in 4 — Raspberry-Pi-Backend, MQTT und MariaDB

**Schnittstellen:** Student:in 5 verwendet den daraus entstehenden Dashboard-/mqttcli-Dienst; Student:in 3 stellt die TTN-MQTT-Zugangsdaten für das Bridge-Kapitel bereit.

In diesem Kapitel werden die C++-Backend-Werkzeuge am Raspberry Pi gebaut und installiert.

| Projekt | Repository | Branch | Rolle |
|---|---|---|---|
| SNode.C | `https://github.com/SNodeC/snode.c` | `master` | C++-Networking-/HTTP-Framework |
| MQTTSuite | `https://github.com/SNodeC/mqttsuite` | `mqttcli-mariadb` | MQTT-Broker, Bridge, CLI, Dashboard-/Storage-Workflow |

Die Build-Verzeichnisse liegen parallel zu den geklonten Quellcode-Verzeichnissen, so wie es im Kurs-Setup verwendet wird.

## Projekt-Build-Pakete installieren

Beginnen Sie mit den Paketen aus dem Raspberry-Pi-Setup-Kapitel. Installieren Sie danach die Entwicklungspakete, die SNode.C/MQTTSuite unter Raspberry Pi OS Bookworm benötigt:

```bash
sudo apt update
sudo apt install -y \
  git \
  build-essential \
  cmake \
  pkg-config \
  libssl-dev \
  zlib1g-dev \
  libmariadb-dev \
  mariadb-client
```

Wenn CMake ein weiteres fehlendes Paket meldet, installieren Sie das passende Debian-`-dev`-Paket und führen Sie CMake erneut aus.

## SNode.C bauen und installieren

Klonen Sie das Repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/snode.c.git snode.c
cd snode.c
git checkout master
```

Erstellen Sie das parallele Build-Verzeichnis, konfigurieren Sie das Projekt, bauen Sie es und installieren Sie es:

```bash
cd ~/water-buoy
mkdir -p snode.c-build
cd snode.c-build
cmake ../snode.c
make
sudo make install
```

Aktualisieren Sie nach der Installation von SNode.C den Cache des dynamischen Linkers, bevor Sie MQTTSuite konfigurieren:

```bash
sudo ldconfig
```

Dieser Schritt ist wichtig, weil MQTTSuite gegen die installierten SNode.C-Bibliotheken linkt.

SNode.C ist fertig, wenn Build und Installation ohne Fehler abgeschlossen wurden und `sudo ldconfig` ausgeführt wurde.

## MQTTSuite bauen und installieren

Klonen Sie das Repository:

```bash
cd ~/water-buoy
git clone https://github.com/SNodeC/mqttsuite.git mqttsuite
cd mqttsuite
git checkout mqttcli-mariadb
```

Erstellen Sie das parallele Build-Verzeichnis, konfigurieren Sie das Projekt, bauen Sie es und installieren Sie es:

```bash
cd ~/water-buoy
mkdir -p mqttsuite-build
cd mqttsuite-build
cmake ../mqttsuite
make
sudo make install
```

Die Programme werden durch `sudo make install` in systemweite Verzeichnisse installiert. Die Studierenden müssen die Programme daher nicht suchen und keine symbolischen Links händisch anlegen.

Prüfen Sie die installierten Werkzeuge:

```bash
mqttbroker --help
mqttbridge --help
mqttcli --help
```

Wenn ein Kommando nicht gefunden wird, öffnen Sie eine neue Shell und prüfen Sie das von CMake verwendete Installationspräfix. Bei einer normalen systemweiten Installation sollten die Kommandos über den Shell-`PATH` verfügbar sein.

## Was die einzelnen MQTTSuite-Werkzeuge tun

| Werkzeug | Rolle im Kurs |
|---|---|
| `mqttbroker` | lokaler MQTT-Broker auf Port 1883 |
| `mqttbridge` | verbindet TTN-MQTT über `bridge-config.json` mit dem lokalen Broker |
| `mqttcli` | abonniert/veröffentlicht MQTT-Nachrichten und speichert im Kurs-Branch Werte in MariaDB bzw. stellt das Dashboard auf Port 8080 bereit |

Die finale Backend-Kette ist:

```text
TTN MQTT
  -> mqttbridge
  -> local mqttbroker on groupN.local:1883
  -> mqttcli storage/dashboard
  -> MariaDB + browser dashboard
```

## Fertig, wenn

```text
[ ] SNode.C ist auf Branch master geklont.
[ ] SNode.C baut in ~/water-buoy/snode.c-build.
[ ] SNode.C wurde mit sudo make install installiert.
[ ] sudo ldconfig wurde nach der Installation von SNode.C ausgeführt.
[ ] MQTTSuite ist auf Branch mqttcli-mariadb geklont.
[ ] MQTTSuite baut in ~/water-buoy/mqttsuite-build.
[ ] MQTTSuite wurde mit sudo make install installiert.
[ ] mqttbroker --help funktioniert.
[ ] mqttbridge --help funktioniert.
[ ] mqttcli --help funktioniert.
```
