# 10 — Installing Qt Creator, PlatformIO, and CP2102 Drivers on Windows and macOS

This chapter explains how to prepare a Windows or macOS computer for the LoRaWAN water-monitoring buoy project.

It is written for students who may not install embedded-development tools every day. The goal is not only to list steps, but also to explain why each tool is needed.

The required software stack is:

```text
Qt Creator          -> editor / IDE
PlatformIO Core     -> build, upload, serial monitor
Python              -> required by PlatformIO Core
CP210x USB driver   -> lets the computer talk to many ESP32 boards over USB
Git                 -> optional but strongly recommended for working with the repository
```

The project itself is a PlatformIO project. Qt Creator is used as a comfortable code editor and project environment. PlatformIO does the actual embedded build and upload work.

---

## 1. What each tool does

| Tool | Role in this course |
|---|---|
| Qt Creator | code editor / IDE for reading and editing project files |
| PlatformIO Core | command-line build system for compiling, uploading, and monitoring |
| Python | runtime needed by PlatformIO Core |
| CP210x driver | USB serial driver for ESP32 boards with CP2102 / CP210x bridge chips |
| Git | version control and repository workflow |

A useful mental model:

```text
Qt Creator is the workbench.
PlatformIO is the build and upload machine.
The CP2102 driver is the USB cable translator.
Git is the project history and collaboration tool.
```

---

## 2. Before installing: identify your ESP32 USB bridge

Many ESP32 development boards do not connect the ESP32 chip directly to USB. They use a small USB-to-serial bridge chip.

Common chips are:

| Chip family | Typical driver |
|---|---|
| CP2102 / CP210x | Silicon Labs CP210x VCP driver |
| CH340 / CH341 | WCH CH34x driver |
| FTDI FT232 | FTDI VCP driver |
| native USB boards | often no separate USB-UART driver needed |

This chapter focuses on CP2102 / CP210x because many ESP32 DevKit boards use it.

If the board is not detected after plugging it in, the USB bridge driver is one of the first things to check.

---

# Part A — Windows Installation

## 3. Install Git on Windows

Git is recommended for cloning the repository and working with branches.

Recommended installation:

1. Download Git for Windows from the official Git website.
2. Run the installer.
3. Keep the default options unless you know what you are changing.
4. After installation, open **Git Bash** or **PowerShell**.
5. Test:

```powershell
git --version
```

Expected result:

```text
git version ...
```

If the command is not found, restart the terminal or check whether Git was added to PATH.

---

## 4. Install Python on Windows

PlatformIO Core is written in Python, so Python must be installed.

Recommended installation:

1. Download Python 3 from the official Python website.
2. Start the installer.
3. Very important: enable **Add python.exe to PATH**.
4. Choose **Install Now**.
5. Open PowerShell.
6. Test:

```powershell
python --version
```

or:

```powershell
py --version
```

Expected result:

```text
Python 3.x.x
```

If `python` opens the Microsoft Store instead of Python, install Python properly from the official installer or use the `py` launcher.

---

## 5. Install PlatformIO Core on Windows

PlatformIO Core provides the `pio` command.

The official PlatformIO documentation recommends installing PlatformIO Core using its installer script or Python-based installation workflow.

A simple Windows workflow is:

1. Download the PlatformIO installer script `get-platformio.py` from the official PlatformIO Core installation documentation.
2. Open PowerShell.
3. Change into the download directory.
4. Run:

```powershell
python get-platformio.py
```

or:

```powershell
py get-platformio.py
```

After installation, PlatformIO may need shell command integration or a PATH update.

Test:

```powershell
pio --version
```

Expected result:

```text
PlatformIO Core, version ...
```

If `pio` is not found, close and reopen PowerShell. If it still does not work, follow the PlatformIO documentation section about installing shell commands / adding PlatformIO to PATH.

---

## 6. Install Qt Creator on Windows

Qt Creator is available for Windows.

For this course, Qt Creator is mainly used as an editor and project environment. We are not primarily building a Qt GUI application. Therefore, the most important requirement is that Qt Creator can open the project folder and use the generated `compile_commands.json` or project files where available.

Recommended installation options:

### Option A — Qt official installer

1. Download the Qt online installer or Qt Creator installer from the official Qt website.
2. Run the installer.
3. Install Qt Creator.
4. A full Qt framework installation is not strictly required just to edit this PlatformIO firmware project, but installing a normal Qt Creator package is fine.

### Option B — Chocolatey

If Chocolatey is installed:

```powershell
choco install qtcreator
```

Qt documentation mentions Chocolatey as a possible package-manager approach on Windows.

After installation:

1. Start Qt Creator.
2. Open the repository or the `LoRaWAN-HelloWorld-radiolib` folder.
3. Use PlatformIO commands in an external terminal or Qt Creator terminal if configured.

---

## 7. Install CP2102 / CP210x driver on Windows

Many ESP32 boards use a Silicon Labs CP2102 / CP210x USB-to-UART bridge.

Without the driver, Windows may not create a COM port for the board.

Installation:

1. Open the official Silicon Labs CP210x VCP driver download page.
2. Download the Windows CP210x VCP driver package.
3. Run the installer.
4. Unplug and replug the ESP32 board.
5. Open Device Manager.
6. Look under **Ports (COM & LPT)**.

Expected result:

```text
Silicon Labs CP210x USB to UART Bridge (COMx)
```

The COM number may be different on every computer, for example COM3, COM5, or COM12.

### Windows driver troubleshooting

If no COM port appears:

- try another USB cable
- make sure the cable supports data, not only charging
- try another USB port
- check Device Manager for unknown devices
- reinstall the CP210x driver
- press the ESP32 reset button after connecting

If upload fails but the COM port exists:

- close serial monitor programs
- check whether another application uses the COM port
- try holding BOOT while upload starts, depending on board type

---

## 8. Test the Windows installation

Open PowerShell in:

```text
LoRaWAN-HelloWorld-radiolib/
```

Check PlatformIO:

```powershell
pio --version
```

Build:

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

If the monitor opens and prints ESP32 output, the USB driver and PlatformIO installation are basically working.

---

# Part B — macOS Installation

## 9. Install Git on macOS

Git is often installed together with Apple command-line tools.

Open Terminal and run:

```bash
git --version
```

If Git is missing, macOS may ask to install command-line developer tools. Accept the installation.

Alternatively, install Git using Homebrew:

```bash
brew install git
```

---

## 10. Install Homebrew on macOS

Homebrew is not strictly required, but it is convenient for installing developer tools.

Check whether Homebrew is installed:

```bash
brew --version
```

If it is not installed, install it from the official Homebrew website.

After installation, restart Terminal and run:

```bash
brew doctor
```

---

## 11. Install Python on macOS

macOS includes some Python-related system components, but for development it is better to install a current Python 3.

Using Homebrew:

```bash
brew install python
```

Test:

```bash
python3 --version
```

Expected result:

```text
Python 3.x.x
```

---

## 12. Install PlatformIO Core on macOS

The official PlatformIO documentation describes several installation methods. For macOS, the installer script is a practical choice.

Run:

```bash
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
```

After installation, test:

```bash
pio --version
```

If `pio` is not found, follow the PlatformIO documentation section about installing shell commands or adding PlatformIO to your shell PATH.

Depending on your shell, you may need to update:

```text
~/.zshrc
```

or:

```text
~/.bash_profile
```

Most modern macOS installations use zsh.

---

## 13. Install Qt Creator on macOS

Qt Creator is available for macOS.

### Option A — Qt official installer

1. Download Qt Creator or the Qt online installer from the official Qt website.
2. Open the downloaded `.dmg` or installer.
3. Install Qt Creator.
4. Start Qt Creator from Applications.

### Option B — Homebrew cask

Qt documentation lists Homebrew as a possible package-manager method for macOS:

```bash
brew install --cask qt-creator
```

For this firmware project, Qt Creator is mainly used as a source-code editor and project environment. PlatformIO still performs the embedded build and upload.

---

## 14. Install CP2102 / CP210x driver on macOS

Many ESP32 boards use a Silicon Labs CP2102 / CP210x USB-to-UART bridge.

Modern macOS versions often include or support many USB serial devices, but CP210x driver installation may still be necessary depending on board, macOS version, and system security settings.

Installation:

1. Open the official Silicon Labs CP210x VCP driver download page.
2. Download the macOS CP210x VCP driver package.
3. Run the installer.
4. If macOS asks for permission to allow a system extension, approve it in **System Settings**.
5. Restart if requested.
6. Unplug and reconnect the ESP32 board.

Check serial devices:

```bash
ls /dev/cu.*
```

Typical CP210x device names look like:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.usbserial-xxxx
```

The exact name depends on driver and board.

### macOS security note

macOS may block newly installed drivers until the user explicitly allows them in System Settings.

Look in:

```text
System Settings -> Privacy & Security
```

or the corresponding security panel for your macOS version.

If a driver was blocked, macOS usually shows an “Allow” button for a limited time after installation.

---

## 15. Test the macOS installation

Open Terminal in:

```text
LoRaWAN-HelloWorld-radiolib/
```

Check PlatformIO:

```bash
pio --version
```

Build:

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

If upload fails because the serial port is busy, close other serial monitors and try again.

---

# Part C — Qt Creator and PlatformIO Together

## 16. Recommended workflow

Use Qt Creator for:

- browsing project files
- editing Markdown documentation
- editing C++ source files if needed
- reading generated `platformio.ini`
- comparing code and documentation

Use PlatformIO CLI for:

- building
- uploading
- opening the serial monitor
- managing embedded dependencies

The core command sequence is:

```bash
pio run
pio run -t upload
pio device monitor
```

## 17. Opening the project in Qt Creator

Recommended approach:

1. Start Qt Creator.
2. Open the folder:

```text
LoRaWAN-HelloWorld-radiolib/
```

3. Use the file tree to inspect:

```text
src/
tools/platformio-ini-generator/www/
docs/
platformio.ini
```

PlatformIO remains the source of truth for building and uploading.

## 18. Optional: compile_commands.json

Some IDE features work better when a project provides `compile_commands.json`.

PlatformIO can generate compilation database information depending on setup. If code completion is weak in Qt Creator, generate or update compile commands from PlatformIO and point Qt Creator/clangd to it.

This is an optional improvement, not required for the basic course workflow.

---

# Part D — Troubleshooting Installation Problems

## 19. `pio` command not found

Possible causes:

- PlatformIO not installed
- terminal was not restarted
- PlatformIO path not added to shell PATH

Fix:

- reinstall PlatformIO Core
- restart terminal
- follow PlatformIO documentation for shell command installation

## 20. ESP32 board not visible

Possible causes:

- missing CP210x driver
- charge-only USB cable
- broken cable
- USB hub issue
- wrong board driver

Fix:

- install CP210x VCP driver
- try another cable
- try another USB port
- avoid unpowered USB hubs
- check Device Manager on Windows or `/dev/cu.*` on macOS

## 21. Upload fails

Possible causes:

- wrong serial port
- serial monitor still open
- board needs BOOT button during upload
- driver problem
- insufficient USB power

Fix:

- close serial monitors
- unplug/replug board
- press reset
- try holding BOOT while upload starts
- check the PlatformIO upload port if necessary

## 22. Serial monitor shows unreadable text

Possible causes:

- wrong baud rate
- board reset noise
- wrong monitor settings

The firmware uses:

```text
115200 baud
```

Use:

```bash
pio device monitor -b 115200
```

## 23. Qt Creator opens but build does not work

Remember: Qt Creator is not the main build system here. PlatformIO is.

Use:

```bash
pio run
```

If Qt Creator does not understand the project automatically, this is not fatal. You can still edit files in Qt Creator and build/upload with PlatformIO.

---

# 24. Final installation checklist

Windows:

```text
[ ] Git installed
[ ] Python installed and available in PowerShell
[ ] PlatformIO Core installed
[ ] Qt Creator installed
[ ] CP210x driver installed
[ ] ESP32 appears as COM port
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
[ ] pio run works
[ ] pio run -t upload works
[ ] pio device monitor works
```

A student computer is ready for the course when it can build, upload, and open the serial monitor for the ESP32 board.
