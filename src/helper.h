#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "bigint.h"
#include "key_fwd.h"

// Converts a little-endian byte array to a 64-bit word array
[[nodiscard]] inline ByteArray bytesToByteArray(const std::vector<uint8_t>& bytes) {
    if (bytes.empty()) return {0};
    ByteArray data;
    data.reserve((bytes.size() + 7) / 8);
    for (size_t i = 0; i < bytes.size(); i += 8) {
        uint64_t val = 0;
        for (size_t j = 0; j < 8 && (i + j) < bytes.size(); ++j) {
            val |= (static_cast<uint64_t>(bytes[i + j]) << (j * 8));
        }
        data.push_back(val);
    }
    // Remove trailing zeros to normalize representation
    while (data.size() > 1 && data.back() == 0) {
        data.pop_back();
    }
    return data;
}

// Converts a 64-bit word array to a little-endian byte array
[[nodiscard]] inline std::vector<uint8_t> byteArrayToBytes(const ByteArray& data,
                                                           size_t targetSize = 0) {
    std::vector<uint8_t> bytes;
    bytes.reserve(data.size() * 8);
    for (uint64_t word : data) {
        for (int i = 0; i < 8; ++i) {
            bytes.push_back(static_cast<uint8_t>((word >> (i * 8)) & 0xFF));
        }
    }
    if (targetSize == 0) {
        // Trim trailing zeros but keep at least 1 byte if the number is 0
        while (bytes.size() > 1 && bytes.back() == 0) {
            bytes.pop_back();
        }
    } else {
        // Adjust array size to match the exact block or key size constraint
        if (bytes.size() < targetSize) {
            bytes.resize(targetSize, 0);
        } else if (bytes.size() > targetSize) {
            while (bytes.size() > targetSize && bytes.back() == 0) {
                bytes.pop_back();
            }
        }
    }
    return bytes;
}
