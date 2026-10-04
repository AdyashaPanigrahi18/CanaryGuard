#include "FileMonitor.h"
#include "HashUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "ConsoleInput.h"
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

FileMonitor::FileMonitor(std::vector<CanaryFile> files,
                         Logger& logger)
    : files_(std::move(files)),
      logger_(logger) {}

bool FileMonitor::restoreFile(CanaryFile& file) {
    try {
        std::ofstream out(
            file.path,
            std::ios::binary | std::ios::trunc);

        if (!out)
            return false;

        out << file.original_content;
        out.close();

        if (sha256File(file.path) != file.baseline_hash)
            return false;

        file.baseline_size = fs::file_size(file.path);

        file.baseline_mtime =
            static_cast<long long>(
                fs::last_write_time(file.path)
                    .time_since_epoch()
                    .count());

        return true;
    }
    catch (...) {
        return false;
    }
}

void FileMonitor::checkFile(CanaryFile& file) {
    fs::path p(file.path);

    if (!fs::exists(p)) {
        ++alertCount_;
        ++deletedCount_;

        logger_.critical(
            "DELETED | " + file.path);

        if (restoreFile(file)) {
            ++restoredCount_;

            logger_.info(
                "RESTORED | " + file.path +
                " | original content restored successfully");
        }
        else {
            logger_.critical(
                "RESTORE_FAILED | " + file.path);
        }

        return;
    }

    try {
        const auto currentHash =
            sha256File(file.path);

        const auto currentSize =
            fs::file_size(p);

        if (currentHash != file.baseline_hash ||
            currentSize != file.baseline_size) {

            ++alertCount_;
            ++modifiedCount_;

            logger_.alert(
                "MODIFIED | " + file.path +
                " | baseline_sha256=" +
                file.baseline_hash +
                " | current_sha256=" +
                currentHash);

            if (restoreFile(file)) {
                ++restoredCount_;

                logger_.info(
                    "RESTORED | " + file.path +
                    " | integrity returned to baseline");
            }
            else {
                logger_.critical(
                    "RESTORE_FAILED | " + file.path);
            }
        }
    }
    catch (const std::exception& e) {
        ++alertCount_;

        logger_.critical(
            "ERROR | " + file.path +
            " | " + e.what());
    }
}

void FileMonitor::checkOnce() {
    for (auto& file : files_)
        checkFile(file);
}

void FileMonitor::showStatus() {
    console::clearScreen();

    std::cout
    << "\n============================================================\n"
    << "                 CANARYGUARD\n"
    << "          ANTI-RANSOMWARE FILE MONITOR\n"
    << "============================================================\n\n";

    std::cout << "Status       : "
              << (running_ ? "RUNNING [OK]" : "STOPPED")
              << '\n';

    std::cout << "Canary Files : "
              << files_.size() << '\n';

    std::cout << "Alerts       : "
              << alertCount_ << '\n';

    std::cout << "Modified     : "
              << modifiedCount_ << '\n';

    std::cout << "Deleted      : "
              << deletedCount_ << '\n';

    std::cout << "Restored     : "
              << restoredCount_ << '\n';

    std::cout
        << "\n------------------------------------------------------------\n"
        << "FILE                 STATUS\n"
        << "------------------------------------------------------------\n";

    for (const auto& file : files_) {
        const std::string name =
            fs::path(file.path).filename().string();

        std::string status;

        try {
            if (!fs::exists(file.path)) {
                status = "DELETED [ALERT]";
            }
            else if (sha256File(file.path) != file.baseline_hash) {
                status = "MODIFIED [ALERT]";
            }
            else {
                status = "NORMAL [OK]";
            }
        }
        catch (...) {
            status = "ERROR";
        }

        std::cout << name;

        if (name.size() < 20)
            std::cout << std::string(20 - name.size(), ' ');

        std::cout << status << '\n';
    }

    std::cout
        << "------------------------------------------------------------\n"
        << "S = Status | T = Safe Test | Q = Quit\n"
        << "============================================================\n";
}

void FileMonitor::simulateRansomware() {
    logger_.warning(
        "SIMULATION | Safe ransomware test started");

    // This test only touches files inside this project's
    // canary_files directory. It does not touch personal files.

    if (!files_.empty()) {
        std::ofstream out(
            files_[0].path,
            std::ios::trunc);

        out << "SIMULATED-RANSOMWARE-CONTENT";
    }

    if (files_.size() > 1) {
        std::error_code ec;
        fs::remove(files_[1].path, ec);
    }

    logger_.warning(
        "SIMULATION | Test completed. Monitor should detect changes.");
}

void FileMonitor::run(int intervalSeconds) {
    logger_.info("Monitoring started.");
    logger_.info(
        "Controls: S=Status | T=Safe Test | Q=Quit");

    showStatus();

    while (running_) {
        checkOnce();

        if (console::keyAvailable()) {
            char key = console::readKey();

            if (key == 's' || key == 'S') {
                showStatus();
            }
            else if (key == 't' || key == 'T') {
                simulateRansomware();

                std::this_thread::sleep_for(
                    std::chrono::seconds(1));

                showStatus();
            }
            else if (key == 'q' || key == 'Q') {
                stop();
                break;
            }
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(intervalSeconds));
    }
}

void FileMonitor::stop() {
    running_ = false;
}
