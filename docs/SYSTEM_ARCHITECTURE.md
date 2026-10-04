# Canary — System Architecture and UML

## 1. System Overview

Canary is a C++17 host-based file-integrity monitoring application. It creates or loads a small set of canary files, calculates SHA-256 baseline hashes, periodically checks the files, logs integrity events, and restores the original content when a canary file is modified or deleted.

The implementation is organized around five main responsibilities:

- `main.cpp` — application startup, command-line arguments, logger/manager creation, signal handling, and monitor startup.
- `CanaryManager` — creates the canary directory/files and builds the initial baseline records.
- `FileMonitor` — periodically checks canary files, detects modification/deletion, performs restoration, displays status, and handles the safe test simulation.
- `HashUtils` — calculates SHA-256 hashes.
- `Logger` — writes timestamped security/application events to the log and console.
- `ConsoleInput` — isolates interactive keyboard/screen handling between Windows and Linux.

## 2. High-Level Architecture

```mermaid
flowchart TD
    A[User / Terminal] --> B[main.cpp]
    B --> C[CanaryManager]
    C --> D[Canary Files]
    C --> E[Baseline Records]
    B --> F[FileMonitor]
    F --> G[HashUtils - SHA-256]
    F --> D
    F --> H[Logger]
    H --> I[logs/alerts.log]
    F --> J[ConsoleInput]
    J --> A
    F --> K[Restore Original Content]
    K --> D
```

## 3. Runtime Data Flow

```mermaid
flowchart LR
    A[Start Canary] --> B[Create / load canary files]
    B --> C[Read file content and metadata]
    C --> D[Calculate SHA-256]
    D --> E[Store baseline in CanaryFile]
    E --> F[Monitoring loop]
    F --> G{File exists?}
    G -- No --> H[Log deletion]
    H --> I[Restore original content]
    G -- Yes --> J[Calculate current SHA-256 and size]
    J --> K{Matches baseline?}
    K -- Yes --> F
    K -- No --> L[Log modification alert]
    L --> I
    I --> M[Verify restored SHA-256]
    M --> F
```

## 4. Component Diagram

```mermaid
flowchart TB
    subgraph Application[Canary Application]
        Main[main.cpp]
        Manager[CanaryManager]
        Monitor[FileMonitor]
        Hash[HashUtils]
        Log[Logger]
        Console[ConsoleInput]
    end

    Files[(canary_files)]
    LogFile[(logs/alerts.log)]
    OS[Operating System]

    Main --> Manager
    Main --> Monitor
    Main --> Log
    Manager --> Hash
    Manager --> Files
    Monitor --> Hash
    Monitor --> Files
    Monitor --> Log
    Monitor --> Console
    Console --> OS
    Log --> LogFile
```

## 5. UML Class Diagram

```mermaid
classDiagram
    class CanaryFile {
        +string path
        +string baseline_hash
        +uintmax_t baseline_size
        +long long baseline_mtime
        +string original_content
    }

    class CanaryManager {
        -string directory_
        -void ensureDirectory()
        -void createFile(string path, string content)
        +CanaryManager(string directory)
        +vector~CanaryFile~ createAndBaseline()
    }

    class FileMonitor {
        -vector~CanaryFile~ files_
        -Logger& logger_
        -atomic~bool~ running_
        -int alertCount_
        -int modifiedCount_
        -int deletedCount_
        -int restoredCount_
        -void checkFile(CanaryFile& file)
        -bool restoreFile(CanaryFile& file)
        -void showStatus()
        -void simulateRansomware()
        +FileMonitor(vector~CanaryFile~ files, Logger& logger)
        +void run(int intervalSeconds)
        +void checkOnce()
        +void stop()
    }

    class Logger {
        -string path_
        -void write(string level, string message)
        +Logger(string path)
        +void info(string message)
        +void alert(string message)
        +void warning(string message)
        +void critical(string message)
    }

    class ConsoleInput {
        <<namespace>>
        +bool keyAvailable()
        +char readKey()
        +void clearScreen()
    }

    class HashUtils {
        <<utility>>
        +string sha256File(string path)
    }

    CanaryManager ..> CanaryFile : creates
    CanaryManager ..> HashUtils : uses
    FileMonitor o-- CanaryFile : monitors
    FileMonitor --> Logger : writes events
    FileMonitor ..> HashUtils : uses
    FileMonitor ..> ConsoleInput : uses
```

## 6. UML Sequence Diagram — Normal Startup

```mermaid
sequenceDiagram
    actor User
    participant Main as main.cpp
    participant Manager as CanaryManager
    participant Hash as HashUtils
    participant Monitor as FileMonitor
    participant Logger
    participant Files as File System

    User->>Main: Start canary_monitor
    Main->>Logger: Create logger
    Main->>Manager: createAndBaseline()
    Manager->>Files: Create canary directory/files if needed
    loop For each canary file
        Manager->>Files: Read content and metadata
        Manager->>Hash: sha256File(path)
        Hash-->>Manager: SHA-256 hash
    end
    Manager-->>Main: vector<CanaryFile>
    Main->>Monitor: Create monitor with baseline
    Main->>Monitor: run(interval)
    Monitor->>Logger: Monitoring started
    loop Monitoring cycle
        Monitor->>Files: Check existence
        Monitor->>Hash: Calculate current SHA-256
        Hash-->>Monitor: Current hash
        Monitor->>Monitor: Compare with baseline
    end
```

## 7. UML Sequence Diagram — Modification Detection and Recovery

```mermaid
sequenceDiagram
    actor User
    participant Files as Canary File
    participant Monitor as FileMonitor
    participant Hash as HashUtils
    participant Logger

    User->>Files: Modify canary file
    Monitor->>Files: Read file
    Monitor->>Hash: sha256File(path)
    Hash-->>Monitor: Current hash
    Monitor->>Monitor: Compare current hash with baseline
    Monitor->>Logger: ALERT MODIFIED
    Monitor->>Files: Restore original content
    Monitor->>Hash: Verify restored hash
    Hash-->>Monitor: Baseline hash
    Monitor->>Logger: RESTORED
```

## 8. State Diagram

```mermaid
stateDiagram-v2
    [*] --> Starting
    Starting --> BaselineReady: Baseline created
    BaselineReady --> Monitoring
    Monitoring --> Normal: File matches baseline
    Normal --> Monitoring: Next interval
    Monitoring --> ModifiedDetected: Hash/size mismatch
    Monitoring --> DeletedDetected: File missing
    ModifiedDetected --> Restoring
    DeletedDetected --> Restoring
    Restoring --> Monitoring: Restore verified
    Restoring --> Error: Restore failed
    Error --> Monitoring: Continue monitoring
    Monitoring --> Stopped: Q / SIGINT / SIGTERM
    Stopped --> [*]
```

## 9. Main Data Structure

`CanaryFile` is the central record used by the monitor. It contains:

| Field | Purpose |
|---|---|
| `path` | Path of the monitored canary file |
| `baseline_hash` | Known-good SHA-256 value |
| `baseline_size` | Known-good file size |
| `baseline_mtime` | Baseline modification-time value |
| `original_content` | Content used for recovery |

`FileMonitor` stores a `vector<CanaryFile>` and checks each record during every monitoring cycle.

## 10. Platform Boundary

Most of the application uses portable C++17 facilities such as `std::filesystem`, streams, containers, threads, atomics, and standard signal handling.

Platform-specific terminal behavior is isolated in `ConsoleInput.cpp`:

- Windows uses `_kbhit()` / `_getch()` and Windows console APIs.
- Linux uses `select()`, `termios`, `read()`, and ANSI terminal control.

Time conversion is also separated with the platform-specific `localtime_s` / `localtime_r` calls in `Logger.cpp`.

## 11. Build Architecture

```mermaid
flowchart LR
    S[src/*.cpp] --> C[C++17 Compiler]
    H[include/*.h] --> C
    C --> O[Object / Build Files]
    O --> E[canary_monitor]
    CMake[CMakeLists.txt] --> C
    Make[Makefile] --> C
```
