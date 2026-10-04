#pragma once
#include <string>
#include <cstdint>

struct CanaryFile {
    std::string path;
    std::string baseline_hash;
    std::uintmax_t baseline_size{};
    long long baseline_mtime{};
    std::string original_content;
};
