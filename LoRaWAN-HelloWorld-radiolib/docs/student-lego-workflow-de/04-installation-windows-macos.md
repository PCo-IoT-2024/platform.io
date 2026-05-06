# Installation von Qt Creator, PlatformIO und CP2102-Treibern unter Windows und macOS

Dieses Kapitel erklärt, wie ein Windows- oder macOS-Computer für das LoRaWAN-Wasserüberwachungsbojen-Projekt vorbereitet wird.

Es steht früh im Handbuch, weil der Workshop-Workflow mit einem funktionierenden Laptop-Setup beginnt, bevor Studierende das ESP32-Gerät bauen, flashen, überwachen und kalibrieren können.

Der benötigte Software-Stack ist:

```text
Qt Creator          -> editor / IDE
PlatformIO Core     -> build, upload, serial monitor
Python              -> required by PlatformIO Core
CP210x USB driver   -> lets the computer talk to many ESP32 boards over USB
Git                 -> repository workflow
```

Offizielle Download- und Dokumentationslinks:

| Werkzeug | Offizieller Link |
|---|---|
| Git | [Git downloads](https://git-scm.com/downloads) |
| Git for Windows | [Git for Windows](https://gitforwindows.org/) |
| Python | [Python downloads](https://www.python.org/downloads/) |
| PlatformIO Core | [PlatformIO Core installation](https://docs.platformio.org/en/latest/core/installation/index.html) |
| PlatformIO installer script | [PlatformIO installer script documentation](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html) |
| Qt Creator installation | [Qt Creator installation documentation](https://doc.qt.io/qtcreator/creator-how-to-install.html) |
| Qt download page | [Qt downloads](https://www.qt.io/download/) |
| Homebrew | [Homebrew](https://brew.sh/) |
| Homebrew installation | [Homebrew installation documentation](https://docs.brew.sh/Installation.html) |
| Silicon Labs CP210x driver | [CP210x USB to UART Bridge VCP drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers) |

Das Projekt selbst ist ein PlatformIO-Projekt. Qt Creator wird als komfortabler Editor und als Projektumgebung verwendet. PlatformIO erledigt die eigentliche Embedded-Build- und Upload-Arbeit.

---

## Was jedes Werkzeug macht

| Werkzeug | Rolle in diesem Kurs |
|---|---|
| Qt Creator | Editor / IDE zum Lesen und Bearbeiten von Projektdateien |
| PlatformIO Core | Kommandozeilen-Build-System zum Kompilieren, Uploaden und Überwachen |
| Python | Laufzeitumgebung, die PlatformIO Core benötigt |
| CP210x driver | USB-Seriell-Treiber für ESP32-Boards mit CP2102-/CP210x-Bridge-Chips |
| Git | Versionskontrolle und Repository-Workflow |

Ein nützliches mentales Modell:

```text
Qt Creator is the workbench.
PlatformIO is the build and upload machine.
The CP2102 driver is the USB cable translator.
Git is the project history and collaboration tool.
```

---

## Vor der Installation: ESP32-USB-Bridge identifizieren

Viele ESP32-Development-Boards verbinden den ESP32-Chip nicht direkt mit USB. Sie verwenden einen kleinen USB-zu-Seriell-Bridge-Chip.

Häufige Chips sind:

| Chip-Familie | Typischer Treiber |
|---|---|
| CP2102 / CP210x | Silicon Labs CP210x VCP driver |
| CH340 / CH341 | WCH CH34x driver |
| FTDI FT232 | FTDI VCP driver |
| native USB boards | often no separate USB-UART driver needed |

Dieses Kapitel konzentriert sich auf CP2102 / CP210x, weil viele ESP32-DevKit-Boards diesen Chip verwenden.

Wenn das Board nach dem Einstecken nicht erkannt wird, ist der USB-Bridge-Treiber eines der ersten Dinge, die geprüft werden sollten.

---

## Windows-Installation

### Git unter Windows installieren

Git wird zum Klonen des Repositorys und zum Arbeiten mit Branches empfohlen.

Empfohlene Installation:

1. Laden Sie Git von [Git downloads](https://git-scm.com/downloads) oder direkt von [Git for Windows](https://gitforwindows.org/) herunter.
2. Führen Sie den Installer aus.
3. Behalten Sie die Standardoptionen bei, außer Sie wissen genau, was Sie ändern.
4. Öffnen Sie nach der Installation **Git Bash** oder **PowerShell**.
5. Testen Sie:

```powershell
git --version
```

Erwartetes Ergebnis:

```text
git version ...
```

Wenn das Kommando nicht gefunden wird, starten Sie das Terminal neu oder prüfen Sie, ob Git zum PATH hinzugefügt wurde.

---

### Python unter Windows installieren

PlatformIO Core ist in Python geschrieben, daher muss Python installiert sein.

Empfohlene Installation:

1. Laden Sie Python 3 von [Python downloads](https://www.python.org/downloads/) herunter.
2. Starten Sie den Installer.
3. Sehr wichtig: Aktivieren Sie **Add python.exe to PATH**.
4. Wählen Sie **Install Now**.
5. Öffnen Sie PowerShell.
6. Testen Sie:

```powershell
python --version
```

oder:

```powershell
py --version
```

Erwartetes Ergebnis:

```text
Python 3.x.x
```

Wenn `python` statt Python den Microsoft Store öffnet, installieren Sie Python korrekt mit dem offiziellen Installer oder verwenden Sie den `py`-Launcher.

---

### PlatformIO Core unter Windows installieren

PlatformIO Core stellt das Kommando `pio` bereit.

Verwenden Sie die offizielle [PlatformIO Core installation guide](https://docs.platformio.org/en/latest/core/installation/index.html). Die [installer script method](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html) ist die empfohlene plattformübergreifende Methode.

Ein einfacher Windows-Workflow ist:

1. Laden Sie das PlatformIO-Installer-Skript `get-platformio.py` aus der offiziellen PlatformIO-Installer-Skript-Dokumentation herunter.
2. Öffnen Sie PowerShell.
3. Wechseln Sie in das Download-Verzeichnis.
4. Führen Sie aus:

```powershell
python get-platformio.py
```

oder:

```powershell
py get-platformio.py
```

Nach der Installation benötigt PlatformIO eventuell Shell-Command-Integration oder ein PATH-Update.

Testen Sie:

```powershell
pio --version
```

Erwartetes Ergebnis:

```text
PlatformIO Core, version ...
```

Wenn `pio` nicht gefunden wird, schließen und öffnen Sie PowerShell erneut. Wenn es weiterhin nicht funktioniert, folgen Sie dem PlatformIO-Dokumentationsabschnitt über Shell Commands / PlatformIO im PATH.

---

### Qt Creator unter Windows installieren

Qt Creator ist für Windows verfügbar. Verwenden Sie die offizielle [Qt Creator installation documentation](https://doc.qt.io/qtcreator/creator-how-to-install.html) oder die [Qt download page](https://www.qt.io/download/).

Für diesen Kurs wird Qt Creator hauptsächlich als Editor und Projektumgebung verwendet. Wir bauen primär keine Qt-GUI-Anwendung. Deshalb ist die wichtigste Anforderung, dass Qt Creator den Projektordner öffnen kann.

#### Option A — offizieller Qt-Installer

1. Laden Sie den Qt Online Installer oder Qt Creator Installer von der offiziellen Qt-Website herunter.
2. Führen Sie den Installer aus.
3. Installieren Sie Qt Creator.
4. Eine vollständige Qt-Framework-Installation ist nicht unbedingt erforderlich, nur um dieses PlatformIO-Firmware-Projekt zu bearbeiten; eine normale Qt-Creator-Installation ist aber in Ordnung.

#### Option B — Chocolatey

Die Qt-Creator-Dokumentation erwähnt auch Paketmanager-Installationen. Wenn Chocolatey installiert ist:

```powershell
choco install qtcreator
```

Nach der Installation:

1. Starten Sie Qt Creator.
2. Öffnen Sie das Repository oder den Ordner `LoRaWAN-HelloWorld-radiolib`.
3. Verwenden Sie PlatformIO-Kommandos in einem externen Terminal oder im Qt-Creator-Terminal, falls konfiguriert.

---

### CP2102-/CP210x-Treiber unter Windows installieren

Viele ESP32-Boards verwenden eine Silicon Labs CP2102-/CP210x-USB-zu-UART-Bridge.

Ohne Treiber erzeugt Windows eventuell keinen COM-Port für das Board.

Installation:

1. Öffnen Sie die offizielle [Silicon Labs CP210x VCP driver page](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers).
2. Laden Sie das Windows-CP210x-VCP-Treiberpaket herunter.
3. Führen Sie den Installer aus.
4. Stecken Sie das ESP32-Board aus und wieder ein.
5. Öffnen Sie den Geräte-Manager.
6. Schauen Sie unter **Ports (COM & LPT)**.

Erwartetes Ergebnis:

```text
Silicon Labs CP210x USB to UART Bridge (COMx)
```

Die COM-Nummer kann auf jedem Computer anders sein, zum Beispiel COM3, COM5 oder COM12.

#### Windows-Treiber-Fehlersuche

Wenn kein COM-Port erscheint:

- anderes USB-Kabel versuchen
- sicherstellen, dass das Kabel Daten unterstützt und nicht nur lädt
- anderen USB-Port versuchen
- im Geräte-Manager nach unbekannten Geräten suchen
- CP210x-Treiber neu installieren
- nach dem Verbinden die ESP32-Reset-Taste drücken

Wenn der Upload fehlschlägt, obwohl der COM-Port existiert:

- serielle Monitorprogramme schließen
- prüfen, ob eine andere Anwendung den COM-Port verwendet
- je nach Boardtyp BOOT gedrückt halten, während der Upload startet

---

### Windows-Installation testen

Öffnen Sie PowerShell in:

```text
LoRaWAN-HelloWorld-radiolib/
```

PlatformIO prüfen:

```powershell
pio --version
```

Bauen:

```powershell
pio run
```

Upload:

```powershell
pio run -t upload
```

Monitor:

```powershell
pio device monitor
```

Wenn der Monitor öffnet und ESP32-Ausgaben anzeigt, funktionieren USB-Treiber und PlatformIO-Installation grundsätzlich.

---

## macOS-Installation

### Git unter macOS installieren

Git wird häufig zusammen mit den Apple Command Line Tools installiert.

Öffnen Sie Terminal und führen Sie aus:

```bash
git --version
```

Wenn Git fehlt, fragt macOS eventuell, ob Command-Line-Developer-Tools installiert werden sollen. Akzeptieren Sie die Installation.

Alternativ installieren Sie Git mit [Homebrew](https://brew.sh/):

```bash
brew install git
```

---

### Homebrew unter macOS installieren

Homebrew ist nicht zwingend erforderlich, aber praktisch zum Installieren von Entwicklerwerkzeugen.

Prüfen Sie, ob Homebrew installiert ist:

```bash
brew --version
```

Falls es nicht installiert ist, installieren Sie es von der offiziellen [Homebrew website](https://brew.sh/) oder lesen Sie die [Homebrew installation documentation](https://docs.brew.sh/Installation.html).

Nach der Installation Terminal neu starten und ausführen:

```bash
brew doctor
```

---

### Python unter macOS installieren

macOS enthält einige Python-bezogene Systemkomponenten, aber für Entwicklung ist es besser, ein aktuelles Python 3 zu installieren.

Mit Homebrew:

```bash
brew install python
```

Alternativ laden Sie Python von [Python downloads](https://www.python.org/downloads/) herunter.

Test:

```bash
python3 --version
```

Erwartetes Ergebnis:

```text
Python 3.x.x
```

---

### PlatformIO Core unter macOS installieren

Verwenden Sie die offizielle [PlatformIO Core installation guide](https://docs.platformio.org/en/latest/core/installation/index.html). Die [installer script method](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html) ist eine praktische Wahl.

Führen Sie aus:

```bash
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
```

Nach der Installation testen:

```bash
pio --version
```

Wenn `pio` nicht gefunden wird, folgen Sie dem PlatformIO-Dokumentationsabschnitt über Shell Commands oder das Hinzufügen von PlatformIO zum Shell-PATH.

Je nach Shell müssen Sie eventuell aktualisieren:

```text
~/.zshrc
```

oder:

```text
~/.bash_profile
```

Die meisten modernen macOS-Installationen verwenden zsh.

---

### Qt Creator unter macOS installieren

Qt Creator ist für macOS verfügbar. Verwenden Sie die offizielle [Qt Creator installation documentation](https://doc.qt.io/qtcreator/creator-how-to-install.html) oder die [Qt download page](https://www.qt.io/download/).

#### Option A — offizieller Qt-Installer

1. Laden Sie Qt Creator oder den Qt Online Installer von der offiziellen Qt-Website herunter.
2. Öffnen Sie die heruntergeladene `.dmg`-Datei oder den Installer.
3. Installieren Sie Qt Creator.
4. Starten Sie Qt Creator aus dem Programme-Ordner.

#### Option B — Homebrew cask

Die Qt-Dokumentation nennt Homebrew als mögliche Paketmanager-Methode für macOS:

```bash
brew install --cask qt-creator
```

Für dieses Firmware-Projekt wird Qt Creator hauptsächlich als Quellcode-Editor und Projektumgebung verwendet. PlatformIO führt weiterhin den Embedded-Build und Upload aus.

---

### CP2102-/CP210x-Treiber unter macOS installieren

Viele ESP32-Boards verwenden eine Silicon Labs CP2102-/CP210x-USB-zu-UART-Bridge.

Moderne macOS-Versionen enthalten oder unterstützen oft viele USB-Seriell-Geräte, aber abhängig von Board, macOS-Version und Sicherheitseinstellungen kann eine CP210x-Treiberinstallation trotzdem notwendig sein.

Installation:

1. Öffnen Sie die offizielle [Silicon Labs CP210x VCP driver page](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers).
2. Laden Sie das macOS-CP210x-VCP-Treiberpaket herunter.
3. Führen Sie den Installer aus.
4. Falls macOS um Erlaubnis für eine System Extension bittet, erlauben Sie diese in **System Settings**.
5. Starten Sie neu, falls verlangt.
6. Stecken Sie das ESP32-Board aus und wieder ein.

Serielle Geräte prüfen:

```bash
ls /dev/cu.*
```

Typische CP210x-Gerätenamen sehen so aus:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.usbserial-xxxx
```

Der genaue Name hängt von Treiber und Board ab.

#### macOS-Sicherheitshinweis

macOS kann neu installierte Treiber blockieren, bis Benutzer:innen sie ausdrücklich in den Systemeinstellungen erlauben.

Schauen Sie unter:

```text
System Settings -> Privacy & Security
```

oder im entsprechenden Sicherheitspanel Ihrer macOS-Version.

Wenn ein Treiber blockiert wurde, zeigt macOS normalerweise für begrenzte Zeit nach der Installation eine „Allow“-Schaltfläche.

---

### macOS-Installation testen

Öffnen Sie Terminal in:

```text
LoRaWAN-HelloWorld-radiolib/
```

PlatformIO prüfen:

```bash
pio --version
```

Bauen:

```bash
pio run
```

Upload:

```bash
pio run -t upload
```

Monitor:

```bash
pio device monitor
```

Wenn der Upload fehlschlägt, weil der serielle Port belegt ist, schließen Sie andere serielle Monitore und versuchen Sie es erneut.

---

## Qt Creator und PlatformIO gemeinsam verwenden

### Empfohlener Workflow

Verwenden Sie Qt Creator für:

- Durchsuchen der Projektdateien
- Bearbeiten der Markdown-Dokumentation
- Bearbeiten von C++-Quelldateien bei Bedarf
- Lesen der generierten `platformio.ini`
- Vergleichen von Code und Dokumentation

Verwenden Sie PlatformIO CLI für:

- Bauen
- Upload
- Öffnen des Serial Monitor
- Verwalten von Embedded-Abhängigkeiten

Die zentrale Kommandoabfolge ist:

```bash
pio run
pio run -t upload
pio device monitor
```

### Qt-Creator-Projektdateien mit PlatformIO erzeugen

PlatformIO kann Hilfsprojektdateien für Qt Creator erzeugen. Dadurch lässt sich das Projekt in Qt Creator leichter öffnen und inspizieren.

Führen Sie dieses Kommando aus dem PlatformIO-Projektverzeichnis aus:

```text
LoRaWAN-HelloWorld-radiolib/
```

Kommando:

```bash
pio project init --ide qtcreator
```

Dieses Kommando ersetzt PlatformIO nicht als Build-System. Es erstellt nur IDE-Hilfsdateien, damit Qt Creator die Projektstruktur besser versteht.

Typischer Workflow:

```bash
cd LoRaWAN-HelloWorld-radiolib
pio project init --ide qtcreator
```

Öffnen Sie danach die erzeugte Qt-Creator-Projektdatei oder den Projektordner in Qt Creator.

Wenn die erzeugten Dateien nach größeren Projektänderungen veraltet sind, führen Sie das Kommando erneut aus.

### Projekt in Qt Creator öffnen

Empfohlener Ablauf:

1. Qt-Creator-Projektdateien erzeugen:

```bash
pio project init --ide qtcreator
```

2. Qt Creator starten.
3. Die erzeugte Qt-Creator-Projektdatei oder den Ordner öffnen:

```text
LoRaWAN-HelloWorld-radiolib/
```

4. Über den Dateibaum prüfen:

```text
src/
tools/platformio-ini-generator/www/
docs/
platformio.ini
```

PlatformIO bleibt die maßgebliche Quelle für Build und Upload.

### Optional: compile_commands.json

Einige IDE-Funktionen funktionieren besser, wenn ein Projekt `compile_commands.json` bereitstellt.

PlatformIO kann je nach Setup Compilation-Database-Informationen erzeugen. Wenn Code Completion in Qt Creator schwach ist, erzeugen oder aktualisieren Sie Compile Commands aus PlatformIO und verweisen Qt Creator/clangd darauf.

Das ist eine optionale Verbesserung und für den grundlegenden Kurs-Workflow nicht erforderlich.

---

## Fehlersuche bei Installationsproblemen

### `pio` command not found

Mögliche Ursachen:

- PlatformIO nicht installiert
- Terminal wurde nicht neu gestartet
- PlatformIO-Pfad nicht zum Shell-PATH hinzugefügt

Behebung:

- PlatformIO Core neu installieren
- Terminal neu starten
- PlatformIO-Dokumentation zur Shell-Command-Installation befolgen

### ESP32 board not visible

Mögliche Ursachen:

- fehlender CP210x-Treiber
- USB-Kabel nur zum Laden
- defektes Kabel
- USB-Hub-Problem
- falscher Board-Treiber

Behebung:

- CP210x-VCP-Treiber installieren
- anderes Kabel versuchen
- anderen USB-Port versuchen
- unversorgte USB-Hubs vermeiden
- Geräte-Manager unter Windows oder `/dev/cu.*` unter macOS prüfen

### Upload fails

Mögliche Ursachen:

- falscher serieller Port
- Serial Monitor noch offen
- Board benötigt BOOT-Taste während des Uploads
- Treiberproblem
- unzureichende USB-Stromversorgung

Behebung:

- serielle Monitore schließen
- Board aus- und einstecken
- Reset drücken
- versuchen, BOOT gedrückt zu halten, während der Upload startet
- bei Bedarf PlatformIO-Upload-Port prüfen

### Serial monitor shows unreadable text

Mögliche Ursachen:

- falsche Baudrate
- Board-Reset-Rauschen
- falsche Monitor-Einstellungen

Die Firmware verwendet:

```text
115200 baud
```

Verwenden Sie:

```bash
pio device monitor -b 115200
```

### Qt Creator opens but build does not work

Denken Sie daran: Qt Creator ist hier nicht das Haupt-Build-System. PlatformIO ist es.

Verwenden Sie:

```bash
pio run
```

Wenn Qt Creator das Projekt nicht automatisch versteht, erzeugen Sie die Qt-Creator-Projektdateien erneut:

```bash
pio project init --ide qtcreator
```

Öffnen Sie danach das Projekt in Qt Creator erneut.

---

## Finale Installationscheckliste

Windows:

```text
[ ] Git installed
[ ] Python installed and available in PowerShell
[ ] PlatformIO Core installed
[ ] Qt Creator installed
[ ] CP210x driver installed
[ ] ESP32 appears as COM port
[ ] pio project init --ide qtcreator works
[ ] pio run works
[ ] pio run -t upload works
[ ] pio device monitor works
```

macOS:

```text
[ ] Git available
[ ] Python 3 available
[ ] PlatformIO Core installed
[ ] Qt Creator installed
[ ] CP210x driver installed or not needed
[ ] ESP32 appears under /dev/cu.*
[ ] pio project init --ide qtcreator works
[ ] pio run works
[ ] pio run -t upload works
[ ] pio device monitor works
```

Ein Studierendencomputer ist bereit für den Kurs, wenn er bauen, hochladen, den Serial Monitor für das ESP32-Board öffnen und das Projekt bequem in Qt Creator öffnen kann.
