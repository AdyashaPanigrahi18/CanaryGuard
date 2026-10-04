# Project Overview

The system uses decoy canary files as early-warning indicators. It records SHA-256 hashes as a baseline and continuously checks the files. If a canary is modified or deleted, the event is logged and the known-good content is restored.

## Security Concepts Demonstrated
1. File Integrity Monitoring
2. SHA-256 hashing
3. Canary files
4. Detection and response
5. Security logging
6. Safe attack simulation

## Cross-Platform Architecture
The application targets Windows and Linux using C++17 standard library facilities. Platform-specific console input and local-time conversion are isolated in `ConsoleInput.cpp` and `Logger.cpp`; SHA-256 is implemented without OS-specific cryptography APIs.
