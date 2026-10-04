# CanaryGuard – Linux-Based Anti-Ransomware File Integrity Monitor

CanaryGuard is a **Linux-based defensive cybersecurity application written in C++17**. It uses specially created canary files and SHA-256 hashing to detect unauthorized file modifications or deletions.

When a monitored canary file changes, CanaryGuard detects the integrity violation, generates a security alert, logs the event, and can restore the affected file.

## Features

* Canary file creation and monitoring
* SHA-256 integrity baseline
* File modification detection
* File deletion detection
* Automatic file restoration
* Security event logging
* Real-time status display
* Safe ransomware simulation/test
* Configurable monitoring interval
* Linux-based C++17 implementation

## How It Works

CanaryGuard follows a simple integrity-monitoring process:

```text
        Start CanaryGuard
               │
               ▼
      Create/Load Canary Files
               │
               ▼
       Calculate SHA-256 Hash
               │
               ▼
        Store Baseline Hash
               │
               ▼
       Start File Monitoring
               │
               ▼
       Check Files Periodically
               │
        ┌──────┴──────┐
        │             │
     No Change     Change/Delete
        │             │
        ▼             ▼
    Continue       Generate Alert
                      │
                      ▼
                 Log Security Event
                      │
                      ▼
                Restore File
```

The main idea is that ransomware or unauthorized activity may modify or delete files. Since the canary files are monitored continuously, an unexpected change can be detected through comparison with their original SHA-256 integrity values.

## Technologies Used

* **Language:** C++17
* **Operating System:** Linux / Ubuntu
* **Build System:** CMake and GNU Make
* **Hashing:** SHA-256
* **File Monitoring:** C++17 filesystem APIs
* **Version Control:** Git / GitHub

## Project Structure

```text
CanaryGuard/
├── CMakeLists.txt
├── Makefile
├── README.md
├── .gitignore
│
├── canary_files/
│   ├── canary_1.txt
│   ├── canary_2.txt
│   └── canary_3.txt
│
├── config/
│   └── monitor.conf
│
├── docs/
│   ├── PROJECT_OVERVIEW.md
│   └── SYSTEM_ARCHITECTURE.md
│
├── include/
│   ├── CanaryFile.h
│   ├── CanaryManager.h
│   ├── ConsoleInput.h
│   ├── FileMonitor.h
│   ├── HashUtils.h
│   └── Logger.h
│
├── logs/
│   └── .gitkeep
│
├── src/
│   ├── CanaryManager.cpp
│   ├── ConsoleInput.cpp
│   ├── FileMonitor.cpp
│   ├── HashUtils.cpp
│   ├── Logger.cpp
│   └── main.cpp
│
└── tests/
    └── README.md
```

## Requirements

Before building CanaryGuard, make sure Ubuntu/Linux has:

* C++17-compatible compiler
* CMake
* GNU Make

Example tools:

```bash
g++ --version
cmake --version
make --version
```

## Build Using CMake

Open Ubuntu and navigate to the project directory:

```bash
cd /mnt/c/Users/panig/Downloads/Canary_linux
```

Configure the project:

```bash
cmake -S . -B build
```

Build the application:

```bash
cmake --build build
```

## Run CanaryGuard

After a successful build:

```bash
./build/canary_monitor
```

The application displays the monitoring status and the available controls.

Example:

```text
Host-Based Anti-Ransomware Canary File Integrity Monitor

Canary directory: ./canary_files

Created/loaded 3 canary files.
BASELINE | canary_1.txt | SHA-256=...
BASELINE | canary_2.txt | SHA-256=...
BASELINE | canary_3.txt | SHA-256=...

Monitoring started.

Controls:
S = Status
T = Safe Test
Q = Quit
```

## Controls

| Key | Function                          |
| --- | --------------------------------- |
| `S` | Display current monitoring status |
| `T` | Run the built-in safe test        |
| `Q` | Quit the application              |

## Testing

CanaryGuard can be tested by modifying one of the monitored canary files while the application is running.

For example:

1. Start CanaryGuard.
2. Open the `canary_files` directory.
3. Modify `canary_1.txt`.
4. Save the file.
5. Return to the running application.
6. Observe the integrity alert and monitoring response.

The project also provides a built-in **Safe Test** using the `T` control.

The test is restricted to the project's own canary files and is intended only for defensive cybersecurity demonstration.

## Logging

Security and monitoring events are written to:

```text
logs/alerts.log
```

The log can contain information such as:

* Application startup
* Canary file creation
* SHA-256 baseline values
* Monitoring status
* File modification events
* File deletion events
* Restoration events

Runtime log files are excluded from Git tracking using `.gitignore`.

## SHA-256 Integrity Checking

At startup, CanaryGuard calculates a SHA-256 hash for each canary file and stores the resulting value as its baseline.

During monitoring, the application checks the current file state against the expected baseline.

Conceptually:

```text
Original File
     │
     ▼
SHA-256 Hash
     │
     ▼
Baseline
     │
     ▼
Periodic Check
     │
     ▼
Current SHA-256
     │
     ├── Same ──────► File unchanged
     │
     └── Different ─► Integrity violation detected
```

## Defensive Purpose

CanaryGuard is designed as a **defensive cybersecurity training project**.

The application demonstrates how file-integrity monitoring can help identify suspicious activity such as unexpected modification or deletion of protected files.

The project does not perform ransomware activity. Its built-in test functionality is limited to the project's own canary files.

## Documentation

Additional project documentation is available in the `docs/` directory:

* `PROJECT_OVERVIEW.md`
* `SYSTEM_ARCHITECTURE.md`

These documents describe the project design and system architecture in more detail.

## Build with Make

The project also provides a Makefile.

Build:

```bash
make
```

Run:

```bash
./canary_monitor
```

If a CMake build is preferred, use the CMake instructions above.

## Future Scope

Possible future improvements include:

* Linux kernel-level monitoring integration
* More advanced event reporting
* Improved configuration management
* Additional security monitoring features
* Centralized alert management
* More extensive automated testing

## Project Status

**Current Status:** Working Linux-based prototype

CanaryGuard has been developed and tested in an Ubuntu/Linux environment using C++17.

## Author

**Adyasha Panigrahi**

GitHub: [AdyashaPanigrahi18](https://github.com/AdyashaPanigrahi18)
