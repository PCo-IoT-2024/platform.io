# Installation von Qt Creator, PlatformIO und CP2102-Treibern

Dieses Kapitel beschreibt die Vorbereitung der Studierenden-Laptops. Die Firmware wird mit PlatformIO gebaut. Qt Creator kann als Entwicklungsumgebung verwendet werden. Zusätzlich muss bei vielen ESP32-Boards ein USB-zu-UART-Treiber installiert sein.

Die Installation ist ein kritischer Workshop-Schritt. Wenn USB-Treiber, Python, PlatformIO oder Git nicht funktionieren, verliert die Gruppe sehr viel Zeit.

## Ziel dieses Kapitels

Nach diesem Kapitel soll jede Gruppe mindestens einen Laptop haben, der Folgendes kann:

```text
[ ] Repository klonen
[ ] PlatformIO-Befehle ausführen
[ ] ESP32 über USB erkennen
[ ] Firmware bauen
[ ] Firmware flashen
[ ] seriellen Monitor öffnen
```

## Was muss installiert werden?

| Werkzeug | Zweck |
|---|---|
| Git | Repository klonen und mit Branches arbeiten |
| Python | Voraussetzung für PlatformIO |
| PlatformIO Core | Firmware bauen und flashen |
| Qt Creator | komfortabler Code-Editor und IDE |
| CP2102-Treiber | USB-Kommunikation mit vielen ESP32-Boards |

## Installation auf Windows

### Git installieren

Git kann von der offiziellen Website installiert werden:

```text
https://git-scm.com/downloads
```

Während der Installation können die Standardoptionen meistens übernommen werden.

Nach der Installation prüfen:

```powershell
git --version
```

### Python installieren

Python von der offiziellen Website installieren:

```text
https://www.python.org/downloads/
```

Wichtig: Beim Installer die Option aktivieren:

```text
Add python.exe to PATH
```

Danach prüfen:

```powershell
python --version
pip --version
```

### PlatformIO Core installieren

PlatformIO Core wird über `pip` installiert:

```powershell
pip install platformio
```

Prüfen:

```powershell
pio --version
```

Falls `pio` nicht gefunden wird, ist Python beziehungsweise das Scripts-Verzeichnis nicht im PATH.

### CP2102-Treiber installieren

Viele ESP32-Boards verwenden einen Silicon Labs CP210x USB-to-UART Bridge Chip.

Treiber:

```text
https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers
```

Nach der Installation ESP32 anstecken und im Geräte-Manager prüfen, ob ein COM-Port erscheint.

Typischer Name:

```text
Silicon Labs CP210x USB to UART Bridge
```

### Qt Creator installieren

Qt Creator kann über den Qt Online Installer installiert werden:

```text
https://www.qt.io/download/
```

Für diesen Kurs reicht Qt Creator als Editor. Es ist nicht notwendig, eine vollständige Qt-Anwendungsentwicklung zu starten.

## Installation auf macOS

### Homebrew installieren

Homebrew ist ein Paketmanager für macOS:

```text
https://brew.sh/
```

Nach der Installation prüfen:

```bash
brew --version
```

### Git und Python installieren

```bash
brew install git python
```

Prüfen:

```bash
git --version
python3 --version
pip3 --version
```

### PlatformIO Core installieren

```bash
pip3 install platformio
```

Prüfen:

```bash
pio --version
```

Falls `pio` nicht gefunden wird, muss eventuell das Python-Binärverzeichnis in den PATH aufgenommen werden.

### CP2102-Treiber installieren

Aktuelle macOS-Versionen erkennen manche USB-Seriell-Chips automatisch. Falls der ESP32 nicht erscheint, den Silicon-Labs-Treiber installieren:

```text
https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers
```

Nach dem Anstecken prüfen:

```bash
ls /dev/cu.*
```

Typische Gerätenamen:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.usbserial-...
/dev/cu.usbmodem...
```

### Qt Creator installieren

Qt Creator kann über den Qt Online Installer installiert werden:

```text
https://www.qt.io/download/
```

Alternativ kann auch ein anderer Editor verwendet werden, solange PlatformIO auf der Kommandozeile funktioniert.

## PlatformIO-Projekt mit Qt Creator verwenden

PlatformIO kann Projektdateien für Qt Creator erzeugen:

```bash
pio project init --ide qtcreator
```

Das ist hilfreich, damit Qt Creator die Projektdateien, Include-Pfade und Autovervollständigung besser versteht.

Wichtig: Der eigentliche Build bleibt PlatformIO:

```bash
pio run
pio run -t upload
pio device monitor
```

Qt Creator ist hier vor allem Editor und Navigationshilfe.

## Seriellen Port finden

Auf Linux/macOS:

```bash
pio device list
```

Auf Windows:

```powershell
pio device list
```

Typische Ports:

```text
Windows: COM3, COM4, ...
macOS:   /dev/cu.SLAB_USBtoUART
Linux:   /dev/ttyUSB0 oder /dev/ttyACM0
```

## Häufige Probleme

| Problem | Mögliche Ursache |
|---|---|
| `pio` wird nicht gefunden | Python Scripts-Verzeichnis nicht im PATH |
| ESP32 erscheint nicht | USB-Kabel nur Ladekabel oder Treiber fehlt |
| Upload schlägt fehl | falscher Port, Boot-Taste nötig, Board blockiert |
| serieller Monitor bleibt leer | falsche Baudrate oder falscher Port |
| Qt Creator findet Includes nicht | `pio project init --ide qtcreator` fehlt |

## Fertig, wenn

```text
[ ] git --version funktioniert.
[ ] python/python3 funktioniert.
[ ] pio --version funktioniert.
[ ] ESP32 wird als serielles Gerät erkannt.
[ ] pio device list zeigt den ESP32.
[ ] Qt Creator kann das Projekt öffnen.
[ ] Ein PlatformIO-Testbuild funktioniert.
```
